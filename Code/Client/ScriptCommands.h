#pragma once

#include <map>
#include <string>

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptCommands
{
	std::map<std::string, HSCRIPTFUNCTION> m_commands;

	void OnCommand(IConsoleCmdArgs *pArgs);

	static void OnCommandWrapper(IConsoleCmdArgs *pArgs);

public:
	ScriptCommands();
	~ScriptCommands();

	bool AddCommand(const char *name, HSCRIPTFUNCTION handler);
};
