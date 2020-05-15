#include <cstdlib>
#include <cstring>

#include "Launcher/Util.h"
#include "Library/DLL.h"

#include "CryMemoryManager.h"

namespace
{
	void *CryMalloc_hook(size_t size, size_t & allocatedSize)
	{
		void *result = nullptr;

		if (size)
		{
			result = std::malloc(size);
		}

		allocatedSize = size;

		return result;
	}

	void *CryRealloc_hook(void *mem, size_t size, size_t & allocatedSize)
	{
		void *result = nullptr;

		if (size)
		{
			result = std::realloc(mem, size);
		}
		else
		{
			if (mem)
			{
				std::free(mem);
			}
		}

		allocatedSize = size;

		return result;
	}

	size_t CryFree_hook(void *mem)
	{
		if (mem)
		{
			std::free(mem);
		}

		return 0;  // this is not so good
	}

	void *CrySystemCrtMalloc_hook(size_t size)
	{
		return (size) ? std::malloc(size) : nullptr;
	}

	void CrySystemCrtFree_hook(void *mem)
	{
		if (mem)
		{
			std::free(mem);
		}
	}

	bool Hook(void *pFunc, void *pNewFunc)
	{
		if (!pFunc)
		{
			return false;
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

		return Util::FillMem(pFunc, code, sizeof code);
	}
}

void CryMemoryManager::Init(const DLL & CrySystem)
{
	Hook(CrySystem.getSymbolAddress("CryMalloc"), CryMalloc_hook);
	Hook(CrySystem.getSymbolAddress("CryRealloc"), CryRealloc_hook);
	Hook(CrySystem.getSymbolAddress("CryFree"), CryFree_hook);
	Hook(CrySystem.getSymbolAddress("CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(CrySystem.getSymbolAddress("CrySystemCrtFree"), CrySystemCrtFree_hook);
}
