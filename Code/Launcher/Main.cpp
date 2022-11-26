#include "Library/Error.h"
#include "Library/WinAPI.h"

// TODO: get rid of this garbage
#include "CryCommon/CryCore/platform_impl.h"

#include "Launcher.h"

////////////////////////////////////////////////////////////////////////////////

// request fast graphics card
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

////////////////////////////////////////////////////////////////////////////////

int __stdcall WinMain(void*, void*, char*, int)
{
	Launcher launcher;
	gLauncher = &launcher;

	try
	{
		launcher.Run();
	}
	catch (const Error & error)
	{
		WinAPI::ErrorBox(error);
		return 1;
	}

	return 0;
}
