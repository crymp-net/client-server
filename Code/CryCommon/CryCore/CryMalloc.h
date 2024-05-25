#pragma once

#include <cstddef>

void* CryMalloc(std::size_t size);
void* CryRealloc(void* oldPtr, std::size_t newSize);
void CryFree(void* ptr);
