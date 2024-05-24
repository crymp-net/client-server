#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>  // _msize
#include <memory_resource>
#include <mutex>
#include <unordered_map>
//#include <mimalloc.h>
//#include <mimalloc-new-delete.h>

#include <tracy/Tracy.hpp>

#include "CryCommon/CrySystem/CryMalloc.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "Library/StringTools.h"
#include "Library/WinAPI.h"

#include "CryMemoryManager.h"

#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
static void Log(const char* format, ...)
{
	FILE* stream = stdout;

	va_list args;
	va_start(args, format);
	std::vfprintf(stream, format, args);
	va_end(args);

	std::fflush(stream);
}

static std::pmr::string CaptureCallstack(std::pmr::memory_resource* memory)
{
	constexpr unsigned int MAX_DEPTH = 32;

	void* callstack[MAX_DEPTH];
	const unsigned int count = RtlCaptureStackBackTrace(0, MAX_DEPTH, callstack, nullptr);

	const unsigned int resultLength = count * ((sizeof(void*) * 2) + 1);

	std::pmr::string result(memory);
	result.resize(resultLength);
	auto it = result.begin();

	for (unsigned int i = 0; i < count; i++)
	{
		for (int j = (sizeof(void*) * 8) - 4; j >= 0; j -= 4)
		{
			*it++ = "0123456789ABCDEF"[(reinterpret_cast<std::uintptr_t>(callstack[i]) >> j) & 0xf];
		}

		*it++ = '\n';
	}

	return result;
}

struct CustomHeap final : public std::pmr::memory_resource
{
	HANDLE heap = nullptr;

	CustomHeap()
	{
		this->heap = HeapCreate(0, 0, 0);
		if (!this->heap)
		{
			WinAPI::ErrorBox("CustomHeap: HeapCreate failed!");
			*reinterpret_cast<int*>(0) = 666;
		}
	}

	void* do_allocate(std::size_t bytes, std::size_t alignment) override
	{
		return HeapAlloc(this->heap, 0, bytes);
	}

	void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
	{
		HeapFree(this->heap, 0, p);
	}

	bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
	{
		const CustomHeap* otherSameType = dynamic_cast<const CustomHeap*>(&other);
		if (!otherSameType)
		{
			return false;
		}

		return this->heap == otherSameType->heap;
	}
};

struct DebugAllocator
{
	struct Block
	{
		void* begin = nullptr;
		bool is_allocated = false;
		std::size_t requested_size = 0;
		std::size_t total_size = 0;
		std::pmr::string callstack;

		explicit Block(std::pmr::memory_resource* memory) : callstack(memory) {}
	};

	CustomHeap heap;
	std::pmr::unordered_map<void*, Block> blocks{&heap};
	std::recursive_mutex mutex;
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
		std::pmr::string callstack = CaptureCallstack(&this->heap);

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
#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_DETECT_OVERFLOW_INSTEAD_OF_UNDERFLOW
		pointer = static_cast<uint8_t*>(pointer) + alignment;
#endif

		auto [it, added] = this->blocks.emplace(pointer, &this->heap);
		it->second.begin = begin;
		it->second.is_allocated = true;
		it->second.callstack = std::move(callstack);
		it->second.requested_size = size;
		it->second.total_size = total_size;

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

	std::pmr::string GetCallstack(void* address)
	{
		std::lock_guard lock(this->mutex);

		const auto to_32bit = [](const void* address) -> const void*
		{
			return reinterpret_cast<void*>(reinterpret_cast<std::size_t>(address) & 0xFFFFFFFFU);
		};

		const void* address_32bit = (address == to_32bit(address)) ? to_32bit(address) : nullptr;

		std::pmr::string callstack;

		for (const auto& [pointer, block] : this->blocks)
		{
			const void* end = static_cast<const uint8_t*>(block.begin) + block.total_size;

			if (address >= block.begin && address < end)
			{
				StringTools::FormatTo(callstack,
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
				StringTools::FormatTo(callstack,
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

static DebugAllocator& GetDebugAlloc()
{
	static DebugAllocator instance;
	return instance;
}
#endif

struct STLport_Allocator
{
	static constexpr std::size_t BLOCK_SIZE = 0x80000;
	static constexpr std::size_t BLOCK_COUNT = 2048;

	static_assert(BLOCK_SIZE >= sizeof(void*));
	static_assert(BLOCK_COUNT > 0);

	void* freeList = nullptr;
	void* freeListLast = nullptr;
	std::mutex mutex;
	std::array<void*, BLOCK_COUNT> blocks = {};

	STLport_Allocator()
	{
		for (std::size_t i = 0; i < BLOCK_COUNT; i++)
		{
			void* block = VirtualAlloc(nullptr, BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (!block)
			{
				WinAPI::ErrorBox("[CryMemoryManager] STLport_Allocator: VirtualAlloc failed!");
				return;
			}

			if (!this->freeList)
			{
				this->freeList = block;
				this->freeListLast = block;
			}
			else
			{
				*static_cast<void**>(this->freeListLast) = block;
				this->freeListLast = block;
			}

			blocks[i] = block;

			if ((reinterpret_cast<std::size_t>(block) + BLOCK_SIZE) > 0x100000000UL)
			{
				WinAPI::ErrorBox("[CryMemoryManager] STLport_Allocator: End beyond 4 GiB boundary!");
			}
		}
	}

	void* Allocate()
	{
		std::lock_guard lock(this->mutex);

		void* block = this->freeList;

		if (this->freeList == this->freeListLast)
		{
			this->freeList = nullptr;
			this->freeListLast = nullptr;
		}
		else
		{
			this->freeList = *static_cast<void**>(this->freeList);
		}

		if (!block)
		{
			WinAPI::ErrorBox("[CryMemoryManager] STLport_Allocator: Out of memory!");
			return nullptr;
		}

		return block;
	}

	void Deallocate(void* block)
	{
		std::lock_guard lock(this->mutex);

		if (!this->freeList)
		{
			this->freeList = block;
			this->freeListLast = block;
		}
		else
		{
			*static_cast<void**>(this->freeListLast) = block;
			this->freeListLast = block;
		}
	}

	bool IsBlock(void* x)
	{
		bool result = false;

		for (void* block : this->blocks)
		{
			result |= (block == x);
		}

		return result;
	}
};

STLport_Allocator* g_STLport_Allocator = nullptr;

static void* CryMalloc_hook(std::size_t size, std::size_t& allocatedSize)
{
	void* result = nullptr;

	const auto doCryMalloc = [&]()
	{
		if (g_STLport_Allocator && size == STLport_Allocator::BLOCK_SIZE)
		{
			result = g_STLport_Allocator->Allocate();
			TracyAllocN(result, size, "STLport");

			allocatedSize = size;
		}
		else
		{
#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
			result = GetDebugAlloc().Allocate(size);
#else
			result = std::calloc(1, size);
#endif
			TracyAllocN(result, size, "CryMalloc");

#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
			allocatedSize = size;
#else
			allocatedSize = _msize(result);
#endif
		}
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
			const bool isBlock = g_STLport_Allocator && g_STLport_Allocator->IsBlock(mem);
#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
			const std::size_t oldSize = isBlock ? STLport_Allocator::BLOCK_SIZE : GetDebugAlloc().GetSize(mem);
#else
			const std::size_t oldSize = isBlock ? STLport_Allocator::BLOCK_SIZE : _msize(mem);
#endif

			result = CryMalloc_hook(size, allocatedSize);

			std::memcpy(result, mem, (oldSize < size) ? oldSize : size);

			if (isBlock)
			{
				TracyFreeN(mem, "STLport");
				g_STLport_Allocator->Deallocate(mem);
			}
			else
			{
				TracyFreeN(mem, "CryMalloc");
#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
				GetDebugAlloc().Deallocate(mem);
#else
				std::free(mem);
#endif
			}
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
	const auto getMemSize = [](void* mem) -> std::size_t
	{
		if (g_STLport_Allocator && g_STLport_Allocator->IsBlock(mem))
		{
			return STLport_Allocator::BLOCK_SIZE;
		}
		else
		{
#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
			return GetDebugAlloc().GetSize(mem);
#else
			return _msize(mem);
#endif
		}
	};

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		return getMemSize(mem);
	}
	else
	{
		return getMemSize(mem);
	}
}

static std::size_t CryFree_hook(void* mem)
{
	std::size_t result = 0;

	const auto doCryFree = [&]()
	{
		if (g_STLport_Allocator && g_STLport_Allocator->IsBlock(mem))
		{
			result = STLport_Allocator::BLOCK_SIZE;

			TracyFreeN(mem, "STLport");
			g_STLport_Allocator->Deallocate(mem);
		}
		else
		{

			TracyFreeN(mem, "CryMalloc");
#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
			result = GetDebugAlloc().Deallocate(mem);
#else
			result = _msize(mem);
			std::free(mem);
#endif
		}
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
	std::size_t allocatedSize = 0;
	return CryMalloc_hook(size, allocatedSize);
}

static void CrySystemCrtFree_hook(void* mem)
{
	CryFree_hook(mem);
}

static void* calloc_hook(std::size_t count, std::size_t size)
{
	return CrySystemCrtMalloc_hook(count * size);
}

static void* realloc_hook(void* ptr, std::size_t size)
{
	std::size_t allocatedSize = 0;
	return CryRealloc_hook(ptr, size, allocatedSize);
}

static char* strdup_hook(const char* str)
{
	const std::size_t length = std::strlen(str);

	void* newstr = CrySystemCrtMalloc_hook(length + 1);
	if (!newstr)
	{
		return nullptr;
	}

	std::memcpy(newstr, str, length + 1);

	return static_cast<char*>(newstr);
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

// TODO: move this to MemoryPatch
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
	// TODO: 32-bit
#endif
}

void CryMemoryManager::Init(void* pCrySystem)
{
	g_STLport_Allocator = new STLport_Allocator;

	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryMalloc"), CryMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryRealloc"), CryRealloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryGetMemSize"), CryGetMemSize_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryFree"), CryFree_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtFree"), CrySystemCrtFree_hook);

	FixHeapUnderflow(pCrySystem);
}

std::pmr::string CryMemoryManager::GetCallstack(void* address)
{
#ifdef CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
	return GetDebugAlloc().GetCallstack(address);
#else
	return {};
#endif
}

void CryMemoryManager::RedirectMalloc(void* pDLL)
{
	WinAPI::HookIATByName(pDLL, "msvcr80.dll", "malloc", CrySystemCrtMalloc_hook);
	WinAPI::HookIATByName(pDLL, "msvcr80.dll", "calloc", calloc_hook);
	WinAPI::HookIATByName(pDLL, "msvcr80.dll", "realloc", realloc_hook);
	WinAPI::HookIATByName(pDLL, "msvcr80.dll", "free", CrySystemCrtFree_hook);
	WinAPI::HookIATByName(pDLL, "msvcr80.dll", "strdup", strdup_hook);

	// TODO: operator new
	// TODO: operator delete
}

////////////////////////////////////////////////////////////////////////////////
// CryCommon/CrySystem/CryMalloc.h
////////////////////////////////////////////////////////////////////////////////

void* CryMalloc(std::size_t size)
{
	std::size_t allocatedSize = 0;
	return CryMalloc_hook(size, allocatedSize);
}

void CryFree(void* p)
{
	CryFree_hook(p);
}
