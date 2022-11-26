#include <random>

#include "RandomGenerator.h"

static std::mt19937 g_engine;

////////////////////////////////////////////////////////////////////////////////
// RandomSeeder
////////////////////////////////////////////////////////////////////////////////

struct RandomSeeder
{
	using result_type = std::random_device::result_type;

	std::random_device generator;

	template<class It>
	void generate(It begin, It end)
	{
		for (auto it = begin; it != end; ++it)
		{
			*it = this->generator();
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
// RandomGenerator
////////////////////////////////////////////////////////////////////////////////

void RandomGenerator::Init()
{
	RandomSeeder seeder;
	g_engine.seed(seeder);
}

float RandomGenerator::GenerateFloat(float min, float max)
{
	return std::uniform_real_distribution<float>(min, max)(g_engine);
}

std::uint32_t RandomGenerator::GenerateUInt32(std::uint32_t min, std::uint32_t max)
{
	return std::uniform_int_distribution<std::uint32_t>(min, max)(g_engine);
}

std::size_t RandomGenerator::GenerateIndex(std::size_t count)
{
	if (count > 0)
	{
		return std::uniform_int_distribution<std::size_t>(0, count - 1)(g_engine);
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// CryEngine stuff
////////////////////////////////////////////////////////////////////////////////

std::uint32_t cry_rand32()
{
	return RandomGenerator::GenerateUInt32(0x0, 0xFFFFFFFF);
}

std::uint32_t cry_rand()
{
	return RandomGenerator::GenerateUInt32(0x0, 0x7FFF);
}

float cry_frand()
{
	return RandomGenerator::GenerateFloat(0, 1);
}
