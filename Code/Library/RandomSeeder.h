#pragma once

#include <random>

// this is used to properly seed random number engines, see SeedSequence concept
struct RandomSeeder
{
	using result_type = std::random_device::result_type;

	std::random_device generator;

	template<class It>
	void generate(It begin, It end)
	{
		for (auto it = begin; it != end; ++it)
		{
			*it = generator();
		}
	}
};
