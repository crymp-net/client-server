////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   CryName.h
//  Version:     v1.00
//  Created:     6/10/2004 by Timur.
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CryCommon/CrySystem/ISystem.h"

struct INameTable
{
	// Name entry header, immediately after this header in memory starts actual string data.
	struct SNameEntry
	{
		// Reference count of this string.
		int nRefCount;
		// Current length of string.
		int nLength;
		// Size of memory allocated at the end of this class.
		int nAllocSize;
		// Here in memory starts character buffer of size nAllocSize.
		// char data[nAllocSize]

		char *GetStr()
		{
			return (char *)(this + 1);
		}

		void AddRef()
		{
			nRefCount++; /*InterlockedIncrement(&_header()->nRefCount);*/
		}

		int Release()
		{
			return --nRefCount;
		}

		int GetMemoryUsage()
		{
			return sizeof(SNameEntry) + strlen(GetStr());
		}
	};

	// Finds an existing name table entry, or creates a new one if not found.
	virtual SNameEntry *GetEntry(const char *str) = 0;
	// Only finds an existing name table entry, return 0 if not found.
	virtual SNameEntry *FindEntry(const char *str) = 0;
	// Release existing name table entry.
	virtual void Release(SNameEntry *pEntry) = 0;
	virtual int GetMemoryUsage() = 0;
};

class CCryName
{
	using SNameEntry = INameTable::SNameEntry;

	const char *m_str = nullptr;

	static INameTable *GetNameTable()
	{
		return gEnv->pNameTable;
	}

	SNameEntry *_entry(const char *pBuffer) const
	{
		assert(pBuffer);
		return ((SNameEntry *)pBuffer) - 1;
	}

	void _release(const char *pBuffer)
	{
		if (pBuffer && _entry(pBuffer)->Release() <= 0)
		{
			GetNameTable()->Release(_entry(pBuffer));
		}
	}

	int _length() const
	{
		return (m_str) ? _entry(m_str)->nLength : 0;
	}

	void _addref(const char *pBuffer)
	{
		if (pBuffer)
		{
			_entry(pBuffer)->AddRef();
		}
	}

public:
	CCryName() = default;

	explicit CCryName(const char *s)
	{
		*this = s;
	}

	CCryName(const CCryName& n)
	{
		_addref(n.m_str);
		m_str = n.m_str;
	}

	CCryName& operator=(const CCryName& n)
	{
		if (m_str != n.m_str)
		{
			_release(m_str);
			m_str = n.m_str;
			_addref(m_str);
		}

		return *this;
	}

	CCryName& operator=(const char *s)
	{
		const char *pBuf = (s && *s) ? GetNameTable()->GetEntry(s)->GetStr() : nullptr;

		if (m_str != pBuf)
		{
			_release(m_str);
			m_str = pBuf;
			_addref(m_str);
		}

		return *this;
	}

	~CCryName()
	{
		_release(m_str);
	}

	bool operator==(const CCryName& n) const
	{
		return m_str == n.m_str;
	}

	bool operator!=(const CCryName& n) const
	{
		return !(*this == n);
	}

	bool operator==(const char *s) const
	{
		return m_str && _stricmp(m_str, s) == 0;
	}

	bool operator!=(const char *s) const
	{
		return !(*this == s);
	}

	bool operator<(const CCryName& n) const
	{
		// FIXME: this is broken, probably on purpose
		return m_str < n.m_str;
	}

	bool empty() const
	{
		return length() == 0;
	}

	void reset()
	{
		_release(m_str);
		m_str = 0;
	}

	const char *c_str() const
	{
		return (m_str) ? m_str : "";
	}

	int length() const
	{
		return _length();
	}

	static int GetMemoryUsage()
	{
		return GetNameTable()->GetMemoryUsage();
	}
};
