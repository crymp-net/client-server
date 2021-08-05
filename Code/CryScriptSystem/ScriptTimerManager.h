#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISerialize;

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

	IScriptSystem *m_pSS = nullptr;
	std::vector<Timer> m_timers;

	uint64_t GetCurrentTime();
	long GetFreeTimerSlot();

	ScriptTimerID MakeTimerID(long slot);
	uint16_t ToSlot(ScriptTimerID timerID);
	uint16_t ToSerial(ScriptTimerID timerID);

	void UpdateTimerSerial(Timer & timer);
	void TriggerTimer(ScriptTimerID timerID);
	void DestroyTimer(Timer & timer);

public:
	ScriptTimerManager();
	~ScriptTimerManager();

	void Init(IScriptSystem *pSS);
	void Update();
	void Reset();

	ScriptTimerID AddTimer(uint64_t milliseconds, HSCRIPTFUNCTION pFunction, IScriptTable *pData = nullptr);
	ScriptTimerID AddTimer(uint64_t milliseconds, const char *functionName, IScriptTable *pData = nullptr);
	void StopTimer(ScriptTimerID timerID);

	void Serialize(ISerialize *pSer);
};
