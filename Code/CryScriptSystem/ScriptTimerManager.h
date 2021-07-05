#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/CryNetwork/ISerialize.h"

using ScriptTimerID = uint32_t;

class ScriptTimerManager
{
	struct Timer
	{
		bool exists = false;
		uint16_t serialNumber = 0;
		uint64_t milliseconds = 0;
		uint64_t endTime = 0;
		HSCRIPTFUNCTION pFunction = nullptr;
		IScriptTable *pData = nullptr;
		std::string functionName;  // alternative to pFunction
	};

	IScriptSystem *m_pScriptSystem = nullptr;
	std::vector<Timer> m_timers;

	long GetFreeTimerSlot();
	uint64_t GetCurrentTime();
	ScriptTimerID CreateTimer(uint64_t milliseconds, HSCRIPTFUNCTION pFunction,
	                          const char *functionName, IScriptTable *pData);
	void TriggerTimer(ScriptTimerID timerID);
	void ResetTimer(Timer & timer);

public:
	ScriptTimerManager(IScriptSystem *pScriptSystem);
	~ScriptTimerManager();

	ScriptTimerID AddTimer(uint64_t milliseconds, HSCRIPTFUNCTION pFunction, IScriptTable *pData = nullptr)
	{
		return CreateTimer(milliseconds, pFunction, nullptr, pData);
	}

	ScriptTimerID AddTimer(uint64_t milliseconds, const char *functionName, IScriptTable *pData = nullptr)
	{
		return CreateTimer(milliseconds, nullptr, functionName, pData);
	}

	void RemoveTimer(ScriptTimerID timerID);

	void Update();

	void Reset();

	void Serialize(TSerialize & ser);
};
