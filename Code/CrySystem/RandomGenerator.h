#pragma once

#include <cstddef>
#include <cstdint>

namespace RandomGenerator
{
	void Init();

	float GenerateFloat(float min, float max);
	std::uint32_t GenerateUInt32(std::uint32_t min, std::uint32_t max);
	std::size_t GenerateIndex(std::size_t count);
}
