/**
 * @file
 * @brief CryMP Client launcher.
 */

#include <cstdlib>
#include <cstring>

#include "CryCommon/CryModuleDefs.h"
#include "CryCommon/platform_impl.h"

#include "CryCommon/platform.h"
#include "CryCommon/ISystem.h"

#include "Patch.h"
#include "Util.h"
#include "CPU.h"


// tell to nVidia GPU driver that Crysis needs powerful graphics card and not the slow one (for multi-GPU laptops)
extern "C" DLL_EXPORT DWORD NvOptimusEnablement = 1;

// do the same as above for AMD GPU driver
// TODO: it seems this thing has no effect on modern AMD hardware, so user has to explicitly choose faster GPU
extern "C" DLL_EXPORT int AmdPowerXpressRequestHighPerformance = 1;


static void ErrorBox(const char *msg)
{
	MessageBoxA(NULL, msg, "Error", MB_OK | MB_DEFAULT_DESKTOP_ONLY);
}

static bool RunGame()
{
	SSystemInitParams params;
	std::memset(&params, 0, sizeof params);

	params.hInstance = GetModuleHandle(NULL);
	params.sLogFileName = "Game.log";

	const char *cmdLine = GetCommandLineA();
	const size_t cmdLineLength = std::strlen(cmdLine);

	if (cmdLineLength < sizeof params.szSystemCmdLine)
	{
		std::strcpy(params.szSystemCmdLine, cmdLine);
	}
	else
	{
		ErrorBox("Command line is too long!");
		return false;
	}

	// TODO

	return true;
}

static bool InstallMemoryPatches(void *pCryAction, void *pCryNetwork, void *pCrySystem)
{
	// CryAction

	if (!Patch::AllowDX9ImmersiveMultiplayer(pCryAction))
		return false;

	// CryNetwork

	if (!Patch::AllowSameCDKeys(pCryNetwork))  // useful for non-dedicated servers
		return false;

	// CrySystem

	if (!Patch::RemoveSecuROM(pCrySystem))
		return false;

	if (!Patch::AllowDX9VeryHighSpec(pCrySystem))
		return false;

	if (!Patch::AllowMultipleInstances(pCrySystem))
		return false;

	if (!Patch::UnhandledExceptions(pCrySystem))
		return false;

	if (CPU::IsAMD() && !CPU::Has3DNow())
	{
		if (!Patch::Disable3DNow(pCrySystem))
			return false;
	}

	return true;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *cmdLine, int cmdShow)
{
	HMODULE pCrySystem = LoadLibraryA("CrySystem.dll");
	if (!pCrySystem)
	{
		ErrorBox("Unable to load the CrySystem DLL!");
		return EXIT_FAILURE;
	}

	HMODULE pCryAction = LoadLibraryA("CryAction.dll");
	if (!pCryAction)
	{
		ErrorBox("Unable to load the CryAction DLL!");
		return EXIT_FAILURE;
	}

	HMODULE pCryNetwork = LoadLibraryA("CryNetwork.dll");
	if (!pCryNetwork)
	{
		ErrorBox("Unable to load the CryNetwork DLL!");
		return EXIT_FAILURE;
	}

	// obtain game build number from CrySystem DLL
	int gameVersion = Util::GetCrysisGameVersion(pCrySystem);
	if (gameVersion < 0)
	{
		ErrorBox("Unable to obtain game version from the CrySystem DLL!");
		return EXIT_FAILURE;
	}

	if (gameVersion != 6156)
	{
		ErrorBox("Unsupported version of the game!");
		return EXIT_FAILURE;
	}

	if (!InstallMemoryPatches(pCryAction, pCryNetwork, pCrySystem))
	{
		ErrorBox("Unable to apply memory patch!");
		return EXIT_FAILURE;
	}

	// launch the game
	return (RunGame()) ? EXIT_SUCCESS : EXIT_FAILURE;
}
