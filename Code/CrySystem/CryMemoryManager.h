#pragma once

#include <string>

namespace CryMemoryManager
{
	void Init(void* pCrySystem);

	std::string GetCallstack(void* address);
	void RedirectMalloc(void* pDLL);
}
