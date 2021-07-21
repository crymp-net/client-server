////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2006.
// -------------------------------------------------------------------------
//  File name:   ItemString.h
//  Version:     v1.00
//  Created:     20/02/2007 by AlexL
//  Compilers:   Visual Studio.NET
//  Description: Special version of CCryName
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __ITEMSTRING_H__
#define __ITEMSTRING_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include <unordered_set>

namespace SharedString
{
	class CNameTable
	{
		std::unordered_set<std::string> m_table;

	public:
		CNameTable() = default;

		// Finds an existing name table entry, or creates a new one if not found.
		const char *GetEntry(const char *str, size_t & length)
		{
			auto [it, isNew] = m_table.emplace(str);

			length = it->length();
			return it->c_str();
		}

		void Dump()
		{
			CryLogAlways("NameTable: %zu entries", m_table.size());

			for (const std::string & entry : m_table)
			{
				CryLogAlways("'%s'", entry.c_str());
			}
		}
	};

	///////////////////////////////////////////////////////////////////////////////
	// Class CSharedString.
	//////////////////////////////////////////////////////////////////////////
	class CSharedString
	{
		const char *m_str = nullptr;
		size_t m_length = 0;

		static CNameTable & GetNameTable()
		{
			static CNameTable table;
			return table;
		}

	public:
		CSharedString() = default;

		CSharedString(const char *str)
		{
			if (str && *str)
			{
				m_str = GetNameTable().GetEntry(str, m_length);
			}
		}

		CSharedString(const CSharedString & other)
		{
			m_str = other.m_str;
			m_length = other.m_length;
		}

		CSharedString & operator=(const CSharedString & other)
		{
			if (m_str != other.m_str)
			{
				m_str = other.m_str;
				m_length = other.m_length;
			}

			return *this;
		}

		CSharedString & operator=(const char *str)
		{
			if (str && *str)
			{
				m_str = GetNameTable().GetEntry(str, m_length);
			}
			else
			{
				reset();
			}

			return *this;
		}

		//! cast to C string operator.
		operator const char*() const
		{
			return c_str();
		}

		explicit operator bool() const
		{
			return !empty();
		}


		bool operator==(const CSharedString & other) const
		{
			return m_str == other.m_str;
		}

		bool operator!=(const CSharedString & other) const
		{
			return m_str != other.m_str;
		}

		bool operator==(const char *str) const
		{
			return m_str && strcmp(m_str, str) == 0;
		}

		bool operator!=(const char *str) const
		{
			return !m_str || strcmp(m_str, str) != 0;
		}

		bool empty() const
		{
			return length() == 0;
		}

		void reset()
		{
			m_str = nullptr;
			m_length = 0;
		}

		void clear()
		{
			reset();
		}

		const char *c_str() const
		{
			return (m_str) ? m_str : "";
		}

		int length() const
		{
			return m_length;
		}

		static void DumpNameTable()
		{
			GetNameTable().Dump();
		}
	};
}

#define ITEM_USE_SHAREDSTRING

typedef SharedString::CSharedString ItemString;
#define CONST_TEMPITEM_STRING(a) a

#endif
