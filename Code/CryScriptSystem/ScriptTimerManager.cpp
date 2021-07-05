#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"

#include "ScriptTimerManager.h"

long ScriptTimerManager::GetFreeTimerSlot()
{
	for (size_t i = 0; i < m_timers.size(); i++)
	{
		if (!m_timers[i].exists)
		{
			return i;
		}
	}

	if (m_timers.size() >= 0xFFFF)  // timer index is uint16_t
	{
		// all timer slots are used
		return -1;
	}

	m_timers.resize(m_timers.size() + 1);

	return m_timers.size() - 1;
}

uint64_t ScriptTimerManager::GetCurrentTime()
{
	return gEnv->pTimer->GetFrameStartTime().GetMilliSecondsAsInt64();
}

ScriptTimerID ScriptTimerManager::CreateTimer(uint64_t milliseconds, HSCRIPTFUNCTION pFunction,
                                              const char *functionName, IScriptTable *pData)
{
	const long index = GetFreeTimerSlot();
	if (index < 0)
	{
		CryLogWarning("[ScriptTimerManager] Too many timers");
		return 0;
	}

	Timer & timer = m_timers[index];

	timer.exists = true;
	timer.serialNumber++;

	// make sure the serial number is never zero
	if (timer.serialNumber == 0)
		timer.serialNumber++;

	timer.milliseconds = milliseconds;
	timer.endTime = GetCurrentTime() + milliseconds;
	timer.pFunction = pFunction;

	if (functionName)
		timer.functionName = functionName;
	else
		timer.functionName.clear();

	timer.pData = pData;

	if (pData)
		pData->AddRef();

	const ScriptTimerID timerID = (index << 16) | timer.serialNumber;

	CryLogAlways("[ScriptTimerManager] Add   0x%08x %6lld ms 0x%p %s", timerID, milliseconds, pFunction, functionName);

	return timerID;
}

void ScriptTimerManager::TriggerTimer(ScriptTimerID timerID)
{
	const uint16_t index = timerID >> 16;

	HSCRIPTFUNCTION pFunction = m_timers[index].pFunction;
	IScriptTable *pData = m_timers[index].pData;

	if (!pFunction)
	{
		// use function name instead
		m_pScriptSystem->GetGlobalValue(m_timers[index].functionName.c_str(), pFunction);
	}

	// destroy the timer before calling its function because a new timer can be added there
	m_timers[index].exists = false;
	m_timers[index].pFunction = nullptr;
	m_timers[index].pData = nullptr;

	if (pFunction)
	{
		if (pData)
			Script::Call(m_pScriptSystem, pFunction, pData, timerID);
		else
			Script::Call(m_pScriptSystem, pFunction, timerID);
	}

	if (pFunction)
		m_pScriptSystem->ReleaseFunc(pFunction);

	if (pData)
		pData->Release();
}

void ScriptTimerManager::ResetTimer(Timer & timer)
{
	timer.exists = false;

	if (timer.pFunction)
	{
		m_pScriptSystem->ReleaseFunc(timer.pFunction);
		timer.pFunction = nullptr;
	}

	if (timer.pData)
	{
		timer.pData->Release();
		timer.pData = nullptr;
	}
}

ScriptTimerManager::ScriptTimerManager(IScriptSystem *pScriptSystem)
{
	m_pScriptSystem = pScriptSystem;

	m_timers.reserve(10);
}

ScriptTimerManager::~ScriptTimerManager()
{
	Reset();
}

void ScriptTimerManager::RemoveTimer(ScriptTimerID timerID)
{
	const uint16_t index = timerID >> 16;
	const uint16_t serialNumber = timerID & 0xFFFF;

	if (index < m_timers.size())
	{
		Timer & timer = m_timers[index];

		if (timer.exists && timer.serialNumber == serialNumber)
		{
			ResetTimer(timer);

			CryLogAlways("[ScriptTimerManager] Del   0x%08x", timerID);
		}
	}
}

void ScriptTimerManager::Update()
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SCRIPT);

	const uint64_t currentTime = GetCurrentTime();

	for (size_t i = 0; i < m_timers.size(); i++)
	{
		if (m_timers[i].exists && currentTime >= m_timers[i].endTime)
		{
			const ScriptTimerID timerID = (i << 16) | m_timers[i].serialNumber;

			TriggerTimer(timerID);

			CryLogAlways("[ScriptTimerManager] Tick  0x%08x", timerID);
		}
	}
}

void ScriptTimerManager::Reset()
{
	for (Timer & timer : m_timers)
	{
		ResetTimer(timer);
	}

	m_timers.clear();

	CryLogAlways("[ScriptTimerManager] Reset");
}

void ScriptTimerManager::Serialize(TSerialize & ser)
{
	if (ser.GetSerializationTarget() == eST_Network)
		return;

	ser.BeginGroup("ScriptTimers");

	if (ser.IsReading())
	{
		Reset();

		const uint64_t currentTime = GetCurrentTime();

		int timerCount = 0;
		ser.Value("count", timerCount);

		for (int i = 0; i < timerCount; i++)
		{
			ser.BeginGroup("timer");

			int timerID = 0;
			ser.Value("id", timerID);

			int64 milliseconds = 0;
			ser.Value("millis", milliseconds);

			CryStringT<char> functionName;
			ser.Value("func", functionName);

			const uint16_t index = timerID >> 16;
			const uint16_t serialNumber = timerID & 0xFFFF;

			if (m_timers.size() <= index)
				m_timers.resize(index + 1);

			Timer & timer = m_timers[index];

			timer.exists = true;
			timer.serialNumber = serialNumber;
			timer.milliseconds = milliseconds;
			timer.endTime = currentTime + milliseconds;
			timer.functionName = functionName.c_str();  // Crytek's string to std::string

			uint32 entityID = 0;
			ser.Value("entity", entityID);

			if (entityID && gEnv->pEntitySystem)
			{
				IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entityID);
				if (pEntity)
				{
					timer.pData = pEntity->GetScriptTable();

					if (timer.pData)
						timer.pData->AddRef();
				}
			}

			// timer
			ser.EndGroup();
		}
	}
	else
	{
		int timerCount = 0;

		for (size_t index = 0; index < m_timers.size(); index++)
		{
			const Timer & timer = m_timers[index];

			if (!timer.exists || timer.pFunction)  // do not save timers that have script handle callback
				continue;

			uint32 dataEntityID = 0;

			// save timer if there is either no user data or if the user data is an entity table
			if (timer.pData)
			{
				ScriptHandle handle;

				if (timer.pData->GetValue("id", handle))
				{
					dataEntityID = static_cast<uint32>(handle.n);
				}
				else
				{
					CryLogWarning("[ScriptTimerManager] Cannot to save timer with function '%s'",
						timer.functionName.c_str()
					);

					continue;
				}
			}

			ser.BeginGroup("timer");

			int timerID = (index << 16) | timer.serialNumber;
			ser.Value("id", timerID);

			int64 milliseconds = timer.milliseconds;
			ser.Value("millis", milliseconds);

			CryStringT<char> functionName = timer.functionName.c_str();  // std::string to Crytek's string
			ser.Value("func", functionName);

			if (dataEntityID)
				ser.Value("entity", dataEntityID);

			// timer
			ser.EndGroup();

			timerCount++;
		}

		ser.Value("count", timerCount);
	}

	// ScriptTimers
	ser.EndGroup();
}
