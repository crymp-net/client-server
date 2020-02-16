#pragma once

#include <stddef.h>
#include <string>

class DLL
{
	void *m_handle;

	void *getSymbolAddress(const char *name) const;
	void unload();

	// no copies
	DLL(const DLL &);
	DLL & operator=(const DLL &);

public:
	DLL()
	: m_handle(NULL)
	{
	}

	~DLL()
	{
		release();
	}

	void release()
	{
		if (m_handle)
		{
			unload();
		}
	}

	bool load(const char *file);

	bool load(const std::string & file)
	{
		return load(file.c_str());
	}

	bool isLoaded() const
	{
		return m_handle != NULL;
	}

	void *getHandle() const
	{
		return m_handle;
	}

	template<class T>
	T getSymbol(const char *name) const
	{
		return reinterpret_cast<T>(getSymbolAddress(name));
	}

	template<class T>
	T getSymbol(const std::string & name) const
	{
		return reinterpret_cast<T>(getSymbolAddress(name.c_str()));
	}
};
