/**
 * @file
 * @brief Utilities.
 */

#pragma once

#include <stddef.h>  // size_t

namespace Util
{
	inline void *RVA(void *base, size_t offset)
	{
		return static_cast<unsigned char*>(base) + offset;
	}

	inline const void *RVA(const void *base, size_t offset)
	{
		return static_cast<const unsigned char*>(base) + offset;
	}

	int FillNOP(void *address, size_t length);
	int FillMem(void *address, const void *data, size_t length);

	int GetCrysisGameVersion(void *pDLL);
}
