#include "Library/Error.h"
#include "Library/WinAPI.h"

// TODO: get rid of this garbage
#include "CryCommon/CryCore/platform_impl.h"

#include "Launcher.h"

// request discrete graphics card
extern "C"
{
	// nVidia
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;

	// AMD
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
