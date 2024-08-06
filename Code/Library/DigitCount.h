#pragma once

#include <concepts>
#include <cstdint>

template<std::integral T>
constexpr unsigned int DigitCount(T x);

template<>
constexpr unsigned int DigitCount(std::uint8_t x)
{
	if (x < 10U) return 1;
	if (x < 100U) return 2;

	return 3;
}

template<>
constexpr unsigned int DigitCount(std::uint16_t x)
{
	if (x < 10U) return 1;
	if (x < 100U) return 2;
	if (x < 1000U) return 3;
	if (x < 10000U) return 4;

	return 5;
}

template<>
constexpr unsigned int DigitCount(std::uint32_t x)
{
	if (x < 10U) return 1;
	if (x < 100U) return 2;
	if (x < 1000U) return 3;
	if (x < 10000U) return 4;
	if (x < 100000U) return 5;
	if (x < 1000000U) return 6;
	if (x < 10000000U) return 7;
	if (x < 100000000U) return 8;
	if (x < 1000000000U) return 9;

	return 10;
}

template<>
constexpr unsigned int DigitCount(std::uint64_t x)
{
	if (x < 10U) return 1;
	if (x < 100U) return 2;
	if (x < 1000U) return 3;
	if (x < 10000U) return 4;
	if (x < 100000U) return 5;
	if (x < 1000000U) return 6;
	if (x < 10000000U) return 7;
	if (x < 100000000U) return 8;
	if (x < 1000000000U) return 9;
	if (x < 10000000000U) return 10;
	if (x < 100000000000U) return 11;
	if (x < 1000000000000U) return 12;
	if (x < 10000000000000U) return 13;
	if (x < 100000000000000U) return 14;
	if (x < 1000000000000000U) return 15;
	if (x < 10000000000000000U) return 16;
	if (x < 100000000000000000U) return 17;
	if (x < 1000000000000000000U) return 18;
	if (x < 10000000000000000000U) return 19;

	return 20;
}

template<>
constexpr unsigned int DigitCount(std::int8_t x)
{
	return DigitCount(static_cast<std::uint8_t>((x < 0) ? -x : x));
}

template<>
constexpr unsigned int DigitCount(std::int16_t x)
{
	return DigitCount(static_cast<std::uint16_t>((x < 0) ? -x : x));
}

template<>
constexpr unsigned int DigitCount(std::int32_t x)
{
	return DigitCount(static_cast<std::uint32_t>((x < 0) ? -x : x));
}

template<>
constexpr unsigned int DigitCount(std::int64_t x)
{
	return DigitCount(static_cast<std::uint64_t>((x < 0) ? -x : x));
}
