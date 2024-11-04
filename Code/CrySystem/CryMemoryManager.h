#pragma once

#include <cstdio>

namespace CryMemoryManager
{
	void Init(void* pCrySystem);

	void ProvideHeapInfo(std::FILE* file, void* address);
}
