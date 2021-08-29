#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "CryCommon/CryNetwork/ISerialize.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"

#include "ScriptTimerManager.h"

uint64_t ScriptTimerManager::GetCurrentTime()
{
	return gEnv->pTimer->GetFrameStartTime().GetMilliSecondsAsInt64();
}

long ScriptTimerManager::GetFreeTimerSlot()
{
	for (size_t i = 0; i < m_timers.size(); i++)
	{
		if (!m_timers[i].exists)
		{
			return i;
		}
	}

	if (m_timers.size() >= 0xFFFF)  // timer slot is 16-bit unsigned integer
	{
		// all timer slots are used
		return -1;
	}

	m_timers.resize(m_timers.size() + 1);

	return m_timers.size() - 1;
}

ScriptTimerID ScriptTimerManager::MakeTimerID(long slot)
{
	return (static_cast<uint32_t>(slot) << 16) | m_timers[slot].serialNumber;
}

uint16_t ScriptTimerManager::ToSlot(ScriptTimerID timerID)
{
	return static_cast<uint16_t>(timerID >> 16);
}

uint16_t ScriptTimerManager::ToSerial(ScriptTimerID timerID)
{
	return static_cast<uint16_t>(timerID & 0xFFFF);
}

void ScriptTimerManager::UpdateTimerSerial(Timer & timer)
{
	timer.serialNumber++;

	// make sure the serial number is never zero
	// so the resulting timer ID is never zero as well
	// zero timer ID is reserved as invalid timer ID
	if (timer.serialNumber == 0)
		timer.serialNumber++;
}

void ScriptTimerManager::TriggerTimer(ScriptTimerID timerID)
{
	const uint16_t slot = ToSlot(timerID);

	HSCRIPTFUNCTION pFunction = m_timers[slot].pFunction;
	IScriptTable *pData = m_timers[slot].pData;

	if (!pFunction)
	{
		// use function name instead
		m_pSS->GetGlobalValue(m_timers[slot].functionName.c_str(), pFunction);
	}

	// destroy the timer before calling its function because a new timer can be added there
	m_timers[slot].exists = false;
	m_timers[slot].pFunction = nullptr;
	m_timers[slot].pData = nullptr;

	if (pFunction)
	{
		if (pData)
			Script::Call(m_pSS, pFunction, pData, timerID);
		else
			Script::Call(m_pSS, pFunction, timerID);
	}

	if (pFunction)
		m_pSS->ReleaseFunc(pFunction);

	if (pData)
		pData->Release();
}

void ScriptTimerManager::DestroyTimer(Timer & timer)
{
	timer.exists = false;

	if (timer.pFunction)
	{
		m_pSS->ReleaseFunc(timer.pFunction);
		timer.pFunction = nullptr;
	}

	if (timer.pData)
	{
		timer.pData->Release();
		timer.pData = nullptr;
	}

	timer.functionName.clear();
}

ScriptTimerManager::ScriptTimerManager()
{
}

ScriptTimerManager::~ScriptTimerManager()
{
	Reset();
}

void ScriptTimerManager::Init(IScriptSystem *pSS)
{
	m_pSS = pSS;

	m_timers.reserve(10);
}

void ScriptTimerManager::Update()
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SCRIPT);

	const uint64_t currentTime = GetCurrentTime();

	for (size_t i = 0; i < m_timers.size(); i++)
	{
		if (m_timers[i].exists && currentTime >= m_timers[i].endTime)
		{
			const ScriptTimerID timerID = MakeTimerID(i);

			TriggerTimer(timerID);

			CryLog("[Script] Timer TICK 0x%08x", timerID);
		}
	}
}

void ScriptTimerManager::Reset()
{
	for (Timer & timer : m_timers)
	{
		DestroyTimer(timer);
	}

	m_timers.clear();
}

ScriptTimerID ScriptTimerManager::AddTimer(uint64_t milliseconds, HSCRIPTFUNCTION pFunction, IScriptTable *pData)
{
	const long slot = GetFreeTimerSlot();
	if (slot < 0)
	{
		CryLogErrorAlways("[Script] Too many timers!");
		return 0;
	}

	Timer & timer = m_timers[slot];

	timer.exists = true;
	timer.milliseconds = milliseconds;
	timer.endTime = GetCurrentTime() + milliseconds;
	timer.pFunction = pFunction;
	timer.pData = pData;

	if (timer.pData)
		timer.pData->AddRef();

	UpdateTimerSerial(timer);

	const ScriptTimerID timerID = MakeTimerID(slot);

	CryLog("[Script] Timer ADD  0x%08x | %8llu ms | 0x%p", timerID, milliseconds, pFunction);

	return timerID;
}

ScriptTimerID ScriptTimerManager::AddTimer(uint64_t milliseconds, const char *functionName, IScriptTable *pData)
{
	const long slot = GetFreeTimerSlot();
	if (slot < 0)
	{
		CryLogErrorAlways("[Script] Too many timers!");
		return 0;
	}

	Timer & timer = m_timers[slot];

	timer.exists = true;
	timer.milliseconds = milliseconds;
	timer.endTime = GetCurrentTime() + milliseconds;
	timer.functionName = functionName;
	timer.pData = pData;

	if (timer.pData)
		timer.pData->AddRef();

	UpdateTimerSerial(timer);

	const ScriptTimerID timerID = MakeTimerID(slot);

	CryLog("[Script] Timer ADD  0x%08x | %8llu ms | %s", timerID, milliseconds, functionName);

	return timerID;
}

void ScriptTimerManager::StopTimer(ScriptTimerID timerID)
{
	const uint16_t slot = ToSlot(timerID);

	if (slot < m_timers.size())
	{
		Timer & timer = m_timers[slot];

		if (timer.exists && timer.serialNumber == ToSerial(timerID))
		{
			DestroyTimer(timer);

			CryLog("[Script] Timer STOP 0x%08x", timerID);
		}
	}
}

void ScriptTimerManager::Serialize(ISerialize *pSer)
{
	TSerialize ser(pSer);

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

			int64_t milliseconds = 0;
			ser.Value("millis", milliseconds);

			CryStringT<char> functionName;
			ser.Value("func", functionName);

			const uint16_t slot = ToSlot(timerID);
			const uint16_t serial = ToSerial(timerID);

			if (m_timers.size() <= slot)
				m_timers.resize(slot + 1);

			Timer & timer = m_timers[slot];

			timer.exists = true;
			timer.serialNumber = serial;
			timer.milliseconds = milliseconds;
			timer.endTime = currentTime + milliseconds;
			timer.functionName = functionName.c_str();  // Crytek's string to std::string

			uint32_t entityID = 0;
			ser.Value("entity", entityID);

			IEntitySystem *pEntitySystem = gEnv->pEntitySystem;
			if (entityID && pEntitySystem)
			{
				IEntity* pEntity = pEntitySystem->GetEntity(entityID);
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

		for (size_t slot = 0; slot < m_timers.size(); slot++)
		{
			const Timer & timer = m_timers[slot];

			if (!timer.exists || timer.pFunction)  // do not save timers that have script handle callback
				continue;

			uint32_t dataEntityID = 0;

			// save timer if there is either no user data or if the user data is an entity table
			if (timer.pData)
			{
				ScriptHandle handle;
				if (timer.pData->GetValue("id", handle))
				{
					dataEntityID = static_cast<uint32_t>(handle.n);
				}
				else
				{
					CryLogWarning("[Script] Cannot to save timer %s", timer.functionName.c_str());
					continue;
				}
			}

			ser.BeginGroup("timer");

			int timerID = MakeTimerID(slot);
			ser.Value("id", timerID);

			int64_t milliseconds = timer.milliseconds;
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
