#pragma once

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

		explicit Query(unsigned int leaf, unsigned int subleaf = 0)
		{
#ifdef _MSC_VER
			int regs[4];
			__cpuidex(regs, leaf, subleaf);

			this->eax = regs[0];
			this->ebx = regs[1];
			this->ecx = regs[2];
			this->edx = regs[3];
#else
			__asm__
			(
				"cpuid"
				: "=a" (this->eax), "=b" (this->ebx), "=c" (this->ecx), "=d" (this->edx)
				: "a" (leaf), "c" (subleaf)
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
	std::bitset<32> leaf_1_edx = {};
	std::bitset<32> leaf_80000001_edx = {};
	char brand_string[48 + 1] = {};
	char vendor_string[12 + 1] = {};

	CPUID()
	{
		Query query(0x0);
		const unsigned int maxBasicLeaf = query.eax;

		*reinterpret_cast<unsigned int*>(this->vendor_string + 0) = query.ebx;
		*reinterpret_cast<unsigned int*>(this->vendor_string + 4) = query.edx;
		*reinterpret_cast<unsigned int*>(this->vendor_string + 8) = query.ecx;

		static constexpr struct { char string[12 + 1]; Vendor id; } vendors[] = {
			{ "AuthenticAMD", Vendor::AMD },
			{ "GenuineIntel", Vendor::INTEL },
		};

		for (const auto& v : vendors)
		{
			if (std::memcmp(this->vendor_string, v.string, 12) == 0)
			{
				this->vendor = v.id;
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
			std::memcpy(this->brand_string + 0, &query, 16);
			query = Query(0x80000003);
			std::memcpy(this->brand_string + 16, &query, 16);
			query = Query(0x80000004);
			std::memcpy(this->brand_string + 32, &query, 16);

			TrimSpaces(this->brand_string);
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

private:
	static void TrimSpaces(char* s)
	{
		char* begin = s;
		char* end = s;

		while (*s == ' ')
		{
			s++;
		}

		while (*s)
		{
			if (*s != ' ')
			{
				end = begin + 1;
			}

			*begin++ = *s++;
		}

		*end = '\0';
	}
};

extern const CPUID g_cpuid;
