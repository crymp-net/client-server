#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "ScriptCommands.h"
#include "Client.h"

void ScriptCommands::OnCommand(IConsoleCmdArgs *pArgs)
{
	const int argCount = pArgs->GetArgCount();
	if (argCount <= 0)
	{
		return;
	}

	const char *name = pArgs->GetArg(0);

	const auto it = m_commands.find(name);
	if (it == m_commands.end())
	{
		// unknown command
		return;
	}

	HSCRIPTFUNCTION handler = it->second;

	if (m_pScriptSystem->BeginCall(handler))
	{
		for (int i = 1; i < argCount; i++)
		{
			m_pScriptSystem->PushFuncParam(pArgs->GetArg(i));
		}

		m_pScriptSystem->EndCall();
	}
}

void ScriptCommands::OnCommandWrapper(IConsoleCmdArgs *pArgs)
{
	gClient->GetScriptCommands()->OnCommand(pArgs);
}

ScriptCommands::ScriptCommands()
{
	m_pConsole = gEnv->pConsole;
	m_pScriptSystem = gEnv->pScriptSystem;
}

ScriptCommands::~ScriptCommands()
{
	for (const auto & [name, handler] : m_commands)
	{
		m_pConsole->RemoveCommand(name.c_str());
		m_pScriptSystem->ReleaseFunc(handler);
	}
}

bool ScriptCommands::AddCommand(const char *name, HSCRIPTFUNCTION handler)
{
	if (!name || !*name || !handler)
	{
		return false;
	}

	const auto [it, added] = m_commands.try_emplace(name, handler);

	if (!added)
	{
		CryLogWarning("Console command '%s' already exists!", name);
		return false;
	}

	m_pConsole->AddCommand(name, OnCommandWrapper, VF_RESTRICTEDMODE);

	CryLog("Added console command '%s'", name);

	return true;
}
