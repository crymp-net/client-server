#pragma once

#include <map>
#include <string>

#include "CryCommon/IScriptSystem.h"

class ScriptCommands
{
	std::map<std::string, HSCRIPTFUNCTION> m_commands;

	void onCommand(IConsoleCmdArgs *pArgs);

	static void OnCommandWrapper(IConsoleCmdArgs *pArgs);

public:
	ScriptCommands();
	~ScriptCommands();

	bool addCommand(const char *name, HSCRIPTFUNCTION handler);
};
