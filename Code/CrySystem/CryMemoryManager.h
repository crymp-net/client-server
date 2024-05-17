#pragma once

#include <string>

#define CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_ENABLED
#define CRY_MEMORY_MANAGER_DEBUG_ALLOCATOR_DETECT_OVERFLOW_INSTEAD_OF_UNDERFLOW

namespace CryMemoryManager
{
	void Init(void* pCrySystem);

	std::pmr::string GetCallstack(void* address);
	void RedirectMalloc(void* pDLL);
}
