#include <cstdio>

#include "CrySystem/CryLog.h"

#include "PakLoader.h"
#include "ZipPak.h"

std::unique_ptr<IPak> PakLoader::LoadPak(std::string_view path)
{
	// TODO: path.data() might not be null-terminated
	std::FILE* file = std::fopen(path.data(), "rb");
	if (!file)
	{
		CryLogAlways("%s: Failed to open %s for reading!", __FUNCTION__, path.data());
		return {};
	}

	try
	{
		return std::make_unique<ZipPak>(file);
	}
	catch (const std::runtime_error& e)
	{
		CryLogAlways("%s: %s: %s", __FUNCTION__, path.data(), e.what());
		return {};
	}
}
