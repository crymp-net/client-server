#include "CryCommon/ISystem.h"
#include "CryCommon/IEntity.h"

#include "ScriptCallbacks.h"

ScriptCallbacks::ScriptCallbacks()
{
	m_pSS = gEnv->pScriptSystem;
}

ScriptCallbacks::~ScriptCallbacks()
{
}

bool ScriptCallbacks::setHandler(EScriptCallback callback, HSCRIPTFUNCTION handler)
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

void ScriptCallbacks::onUpdate(float deltaTime)
{
	call(SCRIPT_CALLBACK_ON_UPDATE, deltaTime);
}

void ScriptCallbacks::onDisconnect(int reason, const char *message)
{
	call(SCRIPT_CALLBACK_ON_DISCONNECT, reason, message);
}

void ScriptCallbacks::onSpawn(IEntity *pEntity)
{
	ScriptHandle entityId;
	entityId.n = pEntity->GetId();

	call(SCRIPT_CALLBACK_ON_SPAWN, entityId);
}
