#pragma once

#include <cstddef>

void* CryMalloc(std::size_t size);
void CryFree(void* p);
