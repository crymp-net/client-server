#pragma once

#include <array>
#include <bitset>
#include <cstring>

#ifdef _MSC_VER
#include <intrin.h>
#endif

struct CPUID
{
	struct Query
	{
		unsigned int eax = 0;
		unsigned int ebx = 0;
		unsigned int ecx = 0;
		unsigned int edx = 0;

		explicit Query(unsigned int leaf)
		{
#ifdef _MSC_VER
			int regs[4];
			__cpuid(regs, leaf);

			this->eax = regs[0];
			this->ebx = regs[1];
			this->ecx = regs[2];
			this->edx = regs[3];
#else
			__asm__
			(
				"cpuid"
				: "=a" (this->eax), "=b" (this->ebx), "=c" (this->ecx), "=d" (this->edx)
				: "a" (leaf), "c" (0)
			);
#endif
		}
	};

	static_assert(sizeof(Query) == 16);

	enum class Vendor
	{
		UNKNOWN = 0,
		AMD,
		INTEL,
	};

	Vendor vendor = Vendor::UNKNOWN;
	std::bitset<32> leaf_1_edx;
	std::bitset<32> leaf_80000001_edx;
	std::array<char, 48 + 1> brand_string = {};
	std::array<char, 12 + 1> vendor_string = {};

	CPUID()
	{
		Query query(0x0);
		const unsigned int maxBasicLeaf = query.eax;

		*reinterpret_cast<unsigned int*>(&this->vendor_string[0]) = query.ebx;
		*reinterpret_cast<unsigned int*>(&this->vendor_string[4]) = query.edx;
		*reinterpret_cast<unsigned int*>(&this->vendor_string[8]) = query.ecx;

		static constexpr struct { std::array<char, 12 + 1> string; Vendor id; } vendors[] = {
			{ "AuthenticAMD", Vendor::AMD },
			{ "GenuineIntel", Vendor::INTEL },
		};

		for (unsigned int i = 0; i < (sizeof vendors / sizeof vendors[0]); i++)
		{
			if (this->vendor_string == vendors[i].string)
			{
				this->vendor = vendors[i].id;
				break;
			}
		}

		if (maxBasicLeaf >= 0x1)
		{
			query = Query(0x1);
			this->leaf_1_edx = query.edx;
		}

		query = Query(0x80000000);
		const unsigned int maxExtendedLeaf = query.eax;

		if (maxExtendedLeaf >= 0x80000001)
		{
			query = Query(0x80000001);
			this->leaf_80000001_edx = query.edx;
		}

		if (maxExtendedLeaf >= 0x80000004)
		{
			query = Query(0x80000002);
			std::memcpy(&this->brand_string[0], &query, 16);
			query = Query(0x80000003);
			std::memcpy(&this->brand_string[16], &query, 16);
			query = Query(0x80000004);
			std::memcpy(&this->brand_string[32], &query, 16);
		}
	}

	bool HasMMX() const
	{
		return this->leaf_1_edx[23];
	}

	bool HasSSE() const
	{
		return this->leaf_1_edx[25];
	}

	bool HasSSE2() const
	{
		return this->leaf_1_edx[26];
	}

	bool Has3DNow() const
	{
		return this->vendor == Vendor::AMD && this->leaf_80000001_edx[31];
	}
};

extern const CPUID g_cpuid;
