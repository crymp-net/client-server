#pragma once

#include <cstddef>
#include <iterator>

#include "gEnv.h"
#include "ICryPak.h"

// EXAMPLE:
//
// for (auto& entry : CryFind("Scripts/Entities/Items/XML/*"))
// {
//     // no need to skip "." and ".." because they are not there anymore with our CryPak
//
//     if (entry.IsDirectory())
//     {
//         CryLogAlways("Found directory: %s", entry.name);
//     }
//     else
//     {
//         CryLogAlways("Found file: %s", entry.name);
//     }
// }

class CryFindEntry : public _finddata_t
{
public:
	bool IsDirectory() const
	{
		return (this->attrib & _A_SUBDIR);
	}

	bool IsInPak() const
	{
		return (this->attrib & _A_IN_CRYPAK);
	}
};

class CryFind
{
	CryFindEntry m_entry = {};
	intptr_t m_handle = -1;

public:
	explicit CryFind(const char* wildcard) : m_entry(), m_handle(gEnv->pCryPak->FindFirst(wildcard, &m_entry))
	{
	}

	CryFind(CryFind&) = delete;
	CryFind(CryFind&& other) = delete;

	CryFind& operator=(CryFind&) = delete;
	CryFind& operator=(CryFind&& other) = delete;

	~CryFind()
	{
		if (m_handle >= 0)
		{
			gEnv->pCryPak->FindClose(m_handle);
		}
	}

	bool HasEntry() const
	{
		return m_handle >= 0;
	}

	explicit operator bool() const
	{
		return this->HasEntry();
	}

	const CryFindEntry& GetEntry() const
	{
		return m_entry;
	}

	bool Next()
	{
		return gEnv->pCryPak->FindNext(m_handle, &m_entry) >= 0;
	}
};

class CryFindIterator
{
public:
	using difference_type = std::ptrdiff_t;
	using value_type = CryFindEntry;

private:
	CryFind* m_find = nullptr;

public:
	CryFindIterator() = default;

	explicit CryFindIterator(CryFind& find) : m_find(find.HasEntry() ? &find : nullptr)
	{
	}

	const value_type& operator*() const
	{
		return m_find->GetEntry();
	}

	const value_type* operator->() const
	{
		return &m_find->GetEntry();
	}

	CryFindIterator& operator++()
	{
		if (!m_find->Next())
		{
			m_find = nullptr;
		}

		return *this;
	}

	void operator++(int)
	{
		m_find->Next();
	}

	friend bool operator==(const CryFindIterator& a, const CryFindIterator& b)
	{
		return a.m_find == b.m_find;
	}
};

inline CryFindIterator begin(CryFind& find)
{
	return CryFindIterator(find);
}

inline CryFindIterator end(CryFind&)
{
	return {};
}

static_assert(std::input_iterator<CryFindIterator>);
