/**
 * @file
 * @brief CryMP Client main function.
 */

#include <cstdlib>
#include <cstring>

#include "CryCommon/CryModuleDefs.h"
#include "CryCommon/platform_impl.h"

#include "Launcher.h"
#include "Util.h"

// request discrete graphics card
extern "C"
{
	// nVidia
	DLL_EXPORT DWORD NvOptimusEnablement = 1;

	// AMD
	DLL_EXPORT int AmdPowerXpressRequestHighPerformance = 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SSystemInitParams params;
	std::memset(&params, 0, sizeof params);

	params.hInstance = hInstance;
	params.sLogFileName = "Game.log";

	// lpCmdLine does not contain program name
	const char *cmdLine = GetCommandLineA();
	const size_t cmdLineLength = std::strlen(cmdLine);

	if (cmdLineLength < sizeof params.szSystemCmdLine)
	{
		std::memcpy(params.szSystemCmdLine, cmdLine, cmdLineLength + 1);
	}
	else
	{
		Util::ErrorBox("Command line is too long!");
		return EXIT_FAILURE;
	}

	Launcher launcher;

	return launcher.run(params) ? EXIT_SUCCESS : EXIT_FAILURE;
}
