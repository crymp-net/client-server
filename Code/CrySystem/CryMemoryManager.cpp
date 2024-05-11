#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <cstring>
#include <mutex>
#include <unordered_map>
//#include <mimalloc.h>
//#include <mimalloc-new-delete.h>

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/StringTools.h"
#include "Library/WinAPI.h"

#include "CryMemoryManager.h"

static void Log(const char* format, ...)
{
	FILE* stream = stdout;

	va_list args;
	va_start(args, format);
	std::vfprintf(stream, format, args);
	va_end(args);

	std::fflush(stream);
}

static std::string CaptureCallstack()
{
	constexpr unsigned int MAX_COUNT = 60;

	void* buffer[MAX_COUNT];
	const unsigned int count = RtlCaptureStackBackTrace(0, MAX_COUNT, buffer, nullptr);

	std::string result;

	for (unsigned int i = 0; i < count; i++)
	{
		StringTools::FormatTo(result, "%p\n", buffer[i]);
	}

	return result;
}

struct DebugAllocator
{
	struct Block
	{
		void* begin = nullptr;
		bool is_allocated = false;
		std::size_t requested_size = 0;
		std::size_t total_size = 0;
		std::string callstack;
	};

	std::unordered_map<void*, Block> blocks;
	std::mutex mutex;
	std::size_t page_size = 0;
	std::size_t alloc_granularity = 0;
	std::size_t last_address = 0x20000000000;

	DebugAllocator()
	{
		SYSTEM_INFO info = {};
		GetSystemInfo(&info);

		this->page_size = info.dwPageSize;
		this->alloc_granularity = info.dwAllocationGranularity;
	}

	void* Allocate(std::size_t size)
	{
		std::string callstack = CaptureCallstack();

		const std::size_t block_page_count = (size + this->page_size - 1) / this->page_size;
		const std::size_t total_size = (2 + block_page_count) * this->page_size;

		std::lock_guard lock(this->mutex);

		void* begin = VirtualAlloc(reinterpret_cast<void*>(last_address), total_size, MEM_COMMIT | MEM_RESERVE, PAGE_NOACCESS);
		if (!begin)
		{
			WinAPI::ErrorBox("Allocate: VirtualAlloc failed!");
			return nullptr;
		}

		last_address += (total_size + alloc_granularity - 1) & (~(alloc_granularity - 1));

		void* pointer = static_cast<uint8_t*>(begin) + this->page_size;

		DWORD old_protection;
		VirtualProtect(pointer, size, PAGE_READWRITE, &old_protection);

		std::memset(pointer, 0, block_page_count * this->page_size);

		const std::size_t remaining_size = size % this->page_size;
		const std::size_t alignment = (remaining_size > 0) ? this->page_size - remaining_size : 0;
		pointer = static_cast<uint8_t*>(pointer) + alignment;

		Block& block = this->blocks[pointer];
		block.begin = begin;
		block.is_allocated = true;
		block.callstack = std::move(callstack);
		block.requested_size = size;
		block.total_size = total_size;

		Log("%04x: Allocate(%zu) -> %p\n", GetCurrentThreadId(), size, pointer);

		return pointer;
	}

	std::size_t Deallocate(void* block)
	{
		std::lock_guard lock(this->mutex);

		const auto it = this->blocks.find(block);
		if (it != this->blocks.end())
		{
			const std::size_t size = it->second.requested_size;

			VirtualFree(it->second.begin, 0, MEM_RELEASE);
			it->second.is_allocated = false;

			Log("%04x: Deallocate(%p) -> %zu\n", GetCurrentThreadId(), block, size);

			return size;
		}

		WinAPI::ErrorBox("Deallocate: unknown block!");
		*reinterpret_cast<int*>(0) = 666;
		return 0;
	}

	std::size_t GetSize(void* block)
	{
		std::lock_guard lock(this->mutex);

		const auto it = this->blocks.find(block);
		if (it != this->blocks.end())
		{
			if (!it->second.is_allocated)
			{
				WinAPI::ErrorBox("GetSize: use after free!");
				*reinterpret_cast<int*>(0) = 666;
				return 0;
			}

			const std::size_t size = it->second.requested_size;

			Log("%04x: GetSize(%p) -> %zu\n", GetCurrentThreadId(), block, size);

			return size;
		}

		WinAPI::ErrorBox("GetSize: unknown block!");
		*reinterpret_cast<int*>(0) = 666;
		return 0;
	}

	std::string GetCallstack(void* address)
	{
		std::lock_guard lock(this->mutex);

		const auto to_32bit = [](const void* address) -> const void*
		{
			return reinterpret_cast<void*>(reinterpret_cast<std::size_t>(address) & 0xFFFFFFFFU);
		};

		const void* address_32bit = to_32bit(address);

		std::string callstack;

		for (const auto& [pointer, block] : this->blocks)
		{
			const void* end = static_cast<const uint8_t*>(block.begin) + block.total_size;

			if (address >= block.begin && address < end)
			{
				callstack += StringTools::Format(
					"match: begin=%p, end=%p, is_allocated=%d, requested_size=%zu, total_size=%zu\n%s",
					block.begin,
					end,
					static_cast<int>(block.is_allocated),
					block.requested_size,
					block.total_size,
					block.callstack.c_str()
				);
			}
			else if (address_32bit && address_32bit >= to_32bit(block.begin) && address_32bit < to_32bit(end))
			{
				callstack += StringTools::Format(
					"possible match: begin=%p, end=%p, is_allocated=%d, requested_size=%zu, total_size=%zu\n%s",
					block.begin,
					end,
					static_cast<int>(block.is_allocated),
					block.requested_size,
					block.total_size,
					block.callstack.c_str()
				);
			}
		}

		return callstack;
	}
};

DebugAllocator g_allocator;

/*
extern "C" void* malloc(std::size_t size)
{
	return mi_malloc(size);
}

extern "C" void* calloc(std::size_t count, std::size_t size)
{
	return mi_calloc(count, size);
}

extern "C" void* realloc(void* p, std::size_t newsize)
{
	return mi_realloc(p, newsize);
}

extern "C" void free(void* p)
{
	return mi_free(p);
}
*/

static void* CryMalloc_hook(std::size_t size, std::size_t& allocatedSize)
{
	void* result = nullptr;

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		result = g_allocator.Allocate(size);
	}
	else
	{
		result = g_allocator.Allocate(size);
	}

	allocatedSize = size;

	return result;
}

static void* CryRealloc_hook(void* mem, std::size_t size, std::size_t& allocatedSize)
{
	const auto do_realloc = [](void* mem, std::size_t newsize) -> void*
	{
		void* result = nullptr;

		if (newsize == 0)
		{
			g_allocator.Deallocate(mem);
		}
		else
		{
			const std::size_t oldsize = g_allocator.GetSize(mem);

			if (newsize > oldsize)
			{
				result = g_allocator.Allocate(newsize);

				if (result)
				{
					std::memcpy(result, mem, oldsize);
				}

				g_allocator.Deallocate(mem);
			}
			else
			{
				result = mem;
			}
		}

		return result;
	};

	void* result = nullptr;

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		result = do_realloc(mem, size);
	}
	else
	{
		result = do_realloc(mem, size);
	}

	allocatedSize = size;

	return result;
}

static std::size_t CryGetMemSize_hook(void* mem, std::size_t)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		return g_allocator.GetSize(mem);
	}
	else
	{
		return g_allocator.GetSize(mem);
	}
}

static std::size_t CryFree_hook(void* mem)
{
	std::size_t result = 0;

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		result = g_allocator.Deallocate(mem);
	}
	else
	{
		result = g_allocator.Deallocate(mem);
	}

	return result;
}

static void* CrySystemCrtMalloc_hook(std::size_t size)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		return g_allocator.Allocate(size);
	}
	else
	{
		return g_allocator.Allocate(size);
	}
}

static void CrySystemCrtFree_hook(void* mem)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		g_allocator.Deallocate(mem);
	}
	else
	{
		g_allocator.Deallocate(mem);
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

static void FixHeapUnderflow(void* pCrySystem)
{
#ifdef BUILD_64BIT
	const unsigned char codeA[] = {
		0x33, 0xC0,  // xor eax, eax
		0x90,        // nop
		0x90,        // nop
	};

	const unsigned char codeB[] = {
		0x33, 0xC9,  // xor ecx, ecx
		0x90,        // nop
		0x90,        // nop
	};

	WinAPI::FillMem(static_cast<unsigned char*>(pCrySystem) + 0xDEE82, &codeA, sizeof(codeA));
	WinAPI::FillMem(static_cast<unsigned char*>(pCrySystem) + 0xDEF0F, &codeB, sizeof(codeB));
#else
	// TODO
#endif
}

void CryMemoryManager::Init(void* pCrySystem)
{
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryMalloc"), CryMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryRealloc"), CryRealloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryGetMemSize"), CryGetMemSize_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryFree"), CryFree_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtFree"), CrySystemCrtFree_hook);

	FixHeapUnderflow(pCrySystem);

#ifdef BUILD_64BIT
	WinAPI::FillNOP(static_cast<unsigned char*>(pCrySystem) + 0x30C3, 0x2);
	WinAPI::FillNOP(static_cast<unsigned char*>(pCrySystem) + 0x31A0, 0x2);
#else
	// TODO
#endif
}

std::string CryMemoryManager::GetCallstack(void* address)
{
	return g_allocator.GetCallstack(address);
}
