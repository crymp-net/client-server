#include <cstdlib>
#include <cstring>
#include <malloc.h>  // _msize

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/WinAPI.h"

#include "CryMemoryManager.h"

#include <CryCommon/CrySystem/IConsole.h>
#include <stack>

#define BLOCK_SIZE 256

const void* g_mem = NULL;
const void* g_mem_end = NULL;

size_t g_mem_cached = 0, g_mem_misses = 0;
std::vector<unsigned> g_pages;
std::vector<unsigned> g_page_usage;
std::vector<unsigned> g_leaks_by_size;
std::mutex mtx;

struct chunk {
	size_t size;
	char mem[BLOCK_SIZE];
};

#define MEM_CACHE
#define LEAK_DETECTOR

#ifdef MEM_CACHE
void MemoryFree(void* mem) {
	if (mem >= g_mem && mem <= g_mem_end) {
		chunk* ch = (chunk*)((const char*)mem - sizeof(chunk::size));
		unsigned page = ((uintptr_t)ch - (uintptr_t)g_mem) / sizeof(chunk);
		mtx.lock();
		g_pages.push_back(page);
#ifdef LEAK_DETECTOR
		g_page_usage[page] &= 0x7FFFFFFFU;
#endif
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
#ifdef LEAK_DETECTOR
		g_page_usage[freePage] |= 0x80000000U;
#endif
		mtx.unlock();
		chunk* ch = ((chunk*)g_mem) + freePage;
		ch->size = size;
		return &ch->mem;
	} else {
		g_mem_misses++;
		return std::calloc(1, size);
	}
}

size_t FindLeaks() {
	size_t leaked = 0;
	mtx.lock();
	for (size_t i = 0; i < g_page_usage.size(); i++) {
		g_page_usage[i] &= 0x80000000U;
	}
	for (size_t i = 0; i < g_leaks_by_size.size(); i++) {
		g_leaks_by_size[i] = 0;
	}
	for (uintptr_t i = (uintptr_t)g_mem; i < (uintptr_t)g_mem_end - sizeof(uintptr_t); i++) {
		uintptr_t ptr = *(uintptr_t*)i;
		void* mem = (void*)ptr;
		if (mem >= g_mem && mem <= g_mem_end) {
			chunk* ch = (chunk*)((const char*)mem - sizeof(chunk::size));
			uintptr_t delta = ((uintptr_t)ch - (uintptr_t)g_mem);
			if (delta % sizeof(chunk) != 0) {
				continue;
			}
			unsigned page = delta / sizeof(chunk);
			if (page >= g_page_usage.size()) {
				continue;
			}
			g_page_usage[page]++;
			if (g_page_usage[page] < 0x80000000U) {
				size_t size = GetAllocatedSize(mem);
				if (size >= 0 && size < g_leaks_by_size.size()) {
					g_leaks_by_size[size]++;
					leaked += size;
				}
			}
		}
	}
	mtx.unlock();
	return leaked;
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

void FindLeaks() {

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
#ifdef LEAK_DETECTOR
	g_page_usage.resize(no_pages);
	g_leaks_by_size.resize(BLOCK_SIZE * 2);
#endif
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
