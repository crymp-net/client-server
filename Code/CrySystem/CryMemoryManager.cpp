#include <malloc.h>  // _msize
#include <stdlib.h>
#include <string.h>

#include "Library/DLL.h"
#include "Library/WinAPI.h"
#include "ThirdParty/jemalloc/jemalloc.h"

#include "CryMemoryManager.h"

namespace
{
	void *CryMalloc_hook(size_t size, size_t & allocatedSize)
	{
		void *result = je_malloc(size);
		allocatedSize = je_malloc_usable_size(result);
		return result;
	}

	void *CryRealloc_hook(void *mem, size_t size, size_t & allocatedSize)
	{
		void *result = je_realloc(mem, size);
		allocatedSize = je_malloc_usable_size(result);
		return result;
	}

	size_t CryFree_hook(void *mem)
	{
		size_t size = je_malloc_usable_size(mem);
		je_free(mem);
		return size;
	}

	size_t CryGetMemSize_hook(void* memblock, size_t sourceSize)
	{
		return je_malloc_usable_size(memblock);
	}

	// CRT------------------------------------

	void* CrySystemCrtMalloc_hook(size_t size)
	{
		return je_malloc(size);
	}

	void* CrySystemCrtRealloc_hook(void* mem, size_t size)
	{
		return je_realloc(mem, size);
	}

	void CrySystemCrtFree_hook(void *mem)
	{
		je_free(mem);
	}

	size_t CrySystemCrtSize_hook(void* mem)
	{
		return je_malloc_usable_size(mem);
	}

	void Hook(void *pFunc, void *pNewFunc)
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

		memcpy(&code[2], &pNewFunc, 8);
	#else
		unsigned char code[] = {
			0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
			0xFF, 0xE0                     // jmp eax
		};

		memcpy(&code[1], &pNewFunc, 4);
	#endif

		WinAPI::FillMem(pFunc, code, sizeof code);
	}
}

void CryMemoryManager::Init(const DLL & CrySystem)
{
	Hook(CrySystem.GetSymbolAddress("CryMalloc"), CryMalloc_hook);
	Hook(CrySystem.GetSymbolAddress("CryRealloc"), CryRealloc_hook);
	Hook(CrySystem.GetSymbolAddress("CryFree"), CryFree_hook);
	Hook(CrySystem.GetSymbolAddress("CryGetMemSize"), CryGetMemSize_hook);

	Hook(CrySystem.GetSymbolAddress("CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(CrySystem.GetSymbolAddress("CrySystemCrtRealloc"), CrySystemCrtRealloc_hook);
	Hook(CrySystem.GetSymbolAddress("CrySystemCrtFree"), CrySystemCrtFree_hook);
	Hook(CrySystem.GetSymbolAddress("CrySystemCrtSize"), CrySystemCrtSize_hook);

	Hook(malloc, je_malloc);
	Hook(realloc, je_realloc);
	Hook(free, je_free);
	Hook(_msize, je_malloc_usable_size);
	Hook(calloc, je_calloc);
}
