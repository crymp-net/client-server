#pragma once

#include <array>

#include "CryCommon/IScriptSystem.h"

struct IEntity;

enum EScriptCallback
{
	SCRIPT_CALLBACK_ON_UPDATE,
	SCRIPT_CALLBACK_ON_DISCONNECT,
	SCRIPT_CALLBACK_ON_SPAWN,

	// must be last
	SCRIPT_CALLBACK_COUNT
};

class ScriptCallbacks
{
	IScriptSystem *m_pSS = nullptr;
	std::array<HSCRIPTFUNCTION, SCRIPT_CALLBACK_COUNT> m_handlers = {};

	template<class... Params>
	void call(EScriptCallback callback, const Params &... params)
	{
		HSCRIPTFUNCTION handler = m_handlers[callback];

		if (handler && m_pSS->BeginCall(handler))
		{
			(m_pSS->PushFuncParam(params), ...);
			m_pSS->EndCall();
		}
	}

public:
	ScriptCallbacks();
	~ScriptCallbacks();

	bool setHandler(EScriptCallback callback, HSCRIPTFUNCTION handler);

	void onUpdate(float deltaTime);
	void onDisconnect(int reason, const char *message);
	void onSpawn(IEntity *pEntity);
};
