#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryEntitySystem/IEntity.h"

#include "ScriptCallbacks.h"

ScriptCallbacks::ScriptCallbacks()
{
	m_pSS = gEnv->pScriptSystem;
}

ScriptCallbacks::~ScriptCallbacks()
{
	for (HSCRIPTFUNCTION handler : m_handlers)
	{
		if (handler)
			m_pSS->ReleaseFunc(handler);
	}
}

bool ScriptCallbacks::SetHandler(EScriptCallback callback, HSCRIPTFUNCTION handler)
{
	if (callback < 0 || callback >= SCRIPT_CALLBACK_COUNT)
	{
		// invalid callback
		return false;
	}

	if (m_handlers[callback])
	{
		// handlers cannot be replaced
		return false;
	}

	m_handlers[callback] = handler;

	return true;
}

void ScriptCallbacks::OnUpdate(float deltaTime)
{
	Call(SCRIPT_CALLBACK_ON_UPDATE, deltaTime);
}

void ScriptCallbacks::OnDisconnect(int reason, const char* message)
{
	Call(SCRIPT_CALLBACK_ON_DISCONNECT, reason, message);
}

void ScriptCallbacks::OnSpawn(IEntity* pEntity)
{
	IScriptTable* pScript = pEntity->GetScriptTable();
	if (pScript)
	{
		Call(SCRIPT_CALLBACK_ON_SPAWN, pScript);
	}
}

void ScriptCallbacks::OnMasterResolved()
{
	Call(SCRIPT_CALLBACK_ON_MASTER_RESOLVED, true);
}

void ScriptCallbacks::OnLoadingStart()
{
	Call(SCRIPT_CALLBACK_ON_LOADING_START);
}

void ScriptCallbacks::OnGameRulesCreated(EntityId gameRulesId)
{
	ScriptHandle id;
	id.n = gameRulesId;
	Call(SCRIPT_CALLBACK_ON_GAME_RULES_CREATED, id);
}

void ScriptCallbacks::OnBecomeLocalActor(EntityId localActorId)
{
	ScriptHandle id;
	id.n = localActorId;
	Call(SCRIPT_CALLBACK_ON_BECOME_LOCAL_ACTOR, id);
}
