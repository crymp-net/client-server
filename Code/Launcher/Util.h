/**
 * @file
 * @brief Launcher utilities.
 */

#pragma once

#include <stddef.h>  // size_t
#include <string>

#include "Library/Path.h"

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

	const char *FindStringNoCase(const char *buffer, const char *string);

	bool FillNOP(void *address, size_t length);
	bool FillMem(void *address, const void *data, size_t length);

	bool DirectoryExists(const Path & path);

	bool SetWorkingDirectory(const Path & path);
	bool AddDLLDirectory(const Path & path);

	std::string GetApplicationFileName();
	Path GetApplicationDirectory();

	unsigned long GetCurrentThreadID();

	int GetCrysisGameVersion(void *pDLL);

	void ErrorBox(const char *msg);

	inline void ErrorBox(const std::string & msg)
	{
		ErrorBox(msg.c_str());
	}
}
