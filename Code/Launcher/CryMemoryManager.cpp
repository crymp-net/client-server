#include <malloc.h>  // _msize
#include <stdlib.h>
#include <string.h>

#include "Library/DLL.h"
#include "Library/WinAPI.h"

#include "CryMemoryManager.h"

namespace
{
	void *CryMalloc_hook(size_t size, size_t & allocatedSize)
	{
		void *result = nullptr;

		if (size)
		{
			// align to 8 bytes
			size += 7 & (8 - (size & 7));
			result = malloc(size);

			allocatedSize = _msize(result);
		}
		else
		{
			allocatedSize = 0;
		}

		return result;
	}

	void *CryRealloc_hook(void *mem, size_t size, size_t & allocatedSize)
	{
		void *result = nullptr;

		if (size)
		{
			// align to 8 bytes
			size += 7 & (8 - (size & 7));
			result = realloc(mem, size);

			allocatedSize = _msize(result);
		}
		else
		{
			if (mem)
			{
				free(mem);
			}

			allocatedSize = 0;
		}

		return result;
	}

	size_t CryFree_hook(void *mem)
	{
		size_t size = 0;

		if (mem)
		{
			size = _msize(mem);

			free(mem);
		}

		return size;
	}

	void *CrySystemCrtMalloc_hook(size_t size)
	{
		void *result = nullptr;

		if (size)
		{
			// align to 8 bytes
			size += 7 & (8 - (size & 7));
			result = malloc(size);
		}

		return result;
	}

	void CrySystemCrtFree_hook(void *mem)
	{
		if (mem)
		{
			free(mem);
		}
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
	Hook(CrySystem.GetSymbolAddress("CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(CrySystem.GetSymbolAddress("CrySystemCrtFree"), CrySystemCrtFree_hook);
}
