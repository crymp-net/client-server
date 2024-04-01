#pragma once

#include <memory>
#include <string_view>

struct IPak;

class PakLoader
{
public:
	std::unique_ptr<IPak> LoadPak(std::string_view path);
};
