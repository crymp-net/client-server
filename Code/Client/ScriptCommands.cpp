#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"

#include "ScriptCommands.h"
#include "Client.h"

void ScriptCommands::onCommand(IConsoleCmdArgs *pArgs)
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

	HSCRIPTFUNCTION function = it->second;
	IScriptSystem *pScriptSystem = gEnv->pScriptSystem;

	if (pScriptSystem->BeginCall(function))
	{
		for (int i = 1; i < argCount; i++)
		{
			pScriptSystem->PushFuncParam(pArgs->GetArg(i));
		}

		pScriptSystem->EndCall();
	}
}

void ScriptCommands::OnCommandWrapper(IConsoleCmdArgs *pArgs)
{
	gClient->getScriptCommands()->onCommand(pArgs);
}

ScriptCommands::ScriptCommands()
{
}

ScriptCommands::~ScriptCommands()
{
}

bool ScriptCommands::addCommand(const char *name, HSCRIPTFUNCTION handler)
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

	gEnv->pConsole->AddCommand(name, OnCommandWrapper, VF_RESTRICTEDMODE);

	CryLog("Added console command '%s'", name);

	return true;
}
