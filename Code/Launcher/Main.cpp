#include "Library/Error.h"
#include "Library/WinAPI.h"

#include "Launcher.h"
#include "ThirdParty/jemalloc/jemalloc.h"

////////////////////////////////////////////////////////////////////////////////

// request fast graphics card
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

////////////////////////////////////////////////////////////////////////////////

void* operator new(std::size_t size)
{
	void* ptr = je_malloc(size);

	if (ptr)
	{
		return ptr;
	}
	else
	{
		throw std::bad_alloc();
	}
}

void operator delete(void* ptr) noexcept
{
	je_free(ptr);
}

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
