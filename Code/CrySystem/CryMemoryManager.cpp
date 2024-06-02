#include <cstdlib>
#include <cstring>
#include <malloc.h>  // _msize

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/WinAPI.h"

#include "CryMemoryManager.h"

#include <CryCommon/CrySystem/IConsole.h>
#include <stack>


const void* g_mem = NULL;
const void* g_mem_end = NULL;

size_t g_mem_cached = 0, g_mem_misses = 0;
std::vector<unsigned> g_pages;
std::mutex mtx;

struct chunk {
	size_t size;
	char mem[256];
};

#define MEM_CACHE

#ifdef MEM_CACHE
void MemoryFree(void* mem) {
	if (mem >= g_mem && mem <= g_mem_end) {
		chunk* ch = (chunk*)((const char*)mem - sizeof(chunk::size));
		unsigned page = ((uintptr_t)ch - (uintptr_t)g_mem) / sizeof(chunk);
		mtx.lock();
		g_pages.push_back(page);
		mtx.unlock();
	} else {
		std::free(mem);
	}
}

size_t GetAllocatedSize(void* mem) {
	if (mem >= g_mem && mem <= g_mem_end) {
		chunk* ch = (chunk*)((const char*)mem - sizeof(chunk::size));
		return ch->size;
	} else {
		return _msize(mem);
	}
}

void* MemoryAllocate(size_t size) {
	if (size <= sizeof(chunk::mem) && g_pages.size() > 0) {
		g_mem_cached++;
		mtx.lock();
		auto freePage = g_pages.back();
		g_pages.pop_back();
		mtx.unlock();
		chunk* ch = ((chunk*)g_mem) + freePage;
		ch->size = size;
		return &ch->mem;
	} else {
		g_mem_misses++;
		return std::calloc(1, size);
	}
}

#else

void MemoryFree(void* mem) {
	std::free(mem);
}

size_t GetAllocatedSize(void* mem) {
	return _msize(mem);
}

void* MemoryAllocate(size_t size) {
	return std::calloc(1, size);
}
#endif

static void* CryMalloc_hook(std::size_t size, std::size_t& allocatedSize)
{
	void* result = nullptr;

	const auto doCryMalloc = [&]()
	{
		result = MemoryAllocate(size);

		allocatedSize = GetAllocatedSize(result);
	};

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

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
			const std::size_t oldSize = GetAllocatedSize(mem);

			result = CryMalloc_hook(size, allocatedSize);

			std::memcpy(result, mem, (oldSize < size) ? oldSize : size);
			MemoryFree(mem);
		}
		else
		{
			result = CryMalloc_hook(size, allocatedSize);
		}
	};

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

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
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		return GetAllocatedSize(mem);
	}
	else
	{
		return GetAllocatedSize(mem);
	}
}

std::size_t CryFree_hook(void* mem)
{
	std::size_t result = 0;

	const auto doCryFree = [&]()
	{
		result = GetAllocatedSize(mem);

		MemoryFree(mem);
	};

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

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
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		return MemoryAllocate(size);
	}
	else
	{
		return MemoryAllocate(size);
	}
}

static void CrySystemCrtFree_hook(void* mem)
{
	size_t result = GetAllocatedSize(mem);
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		MemoryFree(mem);
	}
	else
	{
		MemoryFree(mem);
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
#ifdef MEM_CACHE
	size_t no_pages = 2000000;
	g_mem = malloc(sizeof(chunk) * no_pages);
	g_mem_end = ((const char*)g_mem) + sizeof(chunk) * no_pages;
	g_pages.reserve(no_pages);
	for (size_t i = 0; i < no_pages; i++) {
		g_pages.push_back(i);
	}
#endif

	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryMalloc"), CryMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryRealloc"), CryRealloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryGetMemSize"), CryGetMemSize_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryFree"), CryFree_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtFree"), CrySystemCrtFree_hook);
}
