#pragma once

#include <map>
#include <string>

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct IConsole;

class ScriptCommands
{
	IConsole *m_pConsole = nullptr;
	IScriptSystem *m_pScriptSystem = nullptr;
	std::map<std::string, HSCRIPTFUNCTION> m_commands;

	void OnCommand(IConsoleCmdArgs *pArgs);

	static void OnCommandWrapper(IConsoleCmdArgs *pArgs);

public:
	ScriptCommands();
	~ScriptCommands();

	bool AddCommand(const char *name, HSCRIPTFUNCTION handler);
};
