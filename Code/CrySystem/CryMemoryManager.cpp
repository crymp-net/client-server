#include <cstdlib>
#include <cstring>
#include <malloc.h>  // _msize

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/WinAPI.h"

#include "CryMemoryManager.h"

static void* CryMalloc_hook(std::size_t size, std::size_t& allocatedSize)
{
	void* result = nullptr;

	const auto doCryMalloc = [&]()
	{
		result = std::calloc(1, size);

		allocatedSize = _msize(result);
	};

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_EDITOR);

		doCryMalloc();
	}
	else
	{
		doCryMalloc();
	}

	return result;
}

static void* CryRealloc_hook(void* mem, std::size_t size, std::size_t& allocatedSize)
{
	void* result = nullptr;

	const auto doCryRealloc = [&]()
	{
		if (mem)
		{
			const std::size_t oldSize = _msize(mem);

			result = CryMalloc_hook(size, allocatedSize);

			std::memcpy(result, mem, (oldSize < size) ? oldSize : size);
		}
		else
		{
			result = CryMalloc_hook(size, allocatedSize);
		}
	};

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_EDITOR);

		doCryRealloc();
	}
	else
	{
		doCryRealloc();
	}

	return result;
}

static std::size_t CryGetMemSize_hook(void* mem, std::size_t)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_EDITOR);

		return _msize(mem);
	}
	else
	{
		return _msize(mem);
	}
}

static std::size_t CryFree_hook(void* mem)
{
	std::size_t result = 0;

	const auto doCryFree = [&]()
	{
		result = _msize(mem);

		std::free(mem);
	};

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_EDITOR);

		doCryFree();
	}
	else
	{
		doCryFree();
	}

	return result;
}

static void* CrySystemCrtMalloc_hook(std::size_t size)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_EDITOR);

		return std::calloc(1, size);
	}
	else
	{
		return std::calloc(1, size);
	}
}

static void CrySystemCrtFree_hook(void* mem)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_EDITOR);

		std::free(mem);
	}
	else
	{
		std::free(mem);
	}
}

static void Hook(void* pFunc, void* pNewFunc)
{
	if (!pFunc)
	{
		return;
	}

#ifdef BUILD_64BIT
	unsigned char code[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xE0                                                   // jmp rax
	};

	std::memcpy(&code[2], &pNewFunc, 8);
#else
	unsigned char code[] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
		0xFF, 0xE0                     // jmp eax
	};

	std::memcpy(&code[1], &pNewFunc, 4);
#endif

	WinAPI::FillMem(pFunc, code, sizeof code);
}

void CryMemoryManager::Init(void* pCrySystem)
{
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryMalloc"), CryMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryRealloc"), CryRealloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryGetMemSize"), CryGetMemSize_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryFree"), CryFree_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtFree"), CrySystemCrtFree_hook);
}
