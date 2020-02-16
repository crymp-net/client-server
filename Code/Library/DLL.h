#pragma once

#include <cstddef>
#include <string>

class DLL
{
public:
	enum ELoadFlags
	{
		NO_LOAD = (1 << 0)  //!< Do not load and unload the library. Only obtain handle to already loaded library.
	};

private:
	void *m_handle;
	int m_flags;

	// no copies
	DLL(const DLL &);
	DLL & operator=(const DLL &);

public:
	DLL()
	: m_handle(NULL),
	  m_flags(0)
	{
	}

	~DLL()
	{
		release();
	}

	void release();

	bool load(const char *file, int flags = 0);

	bool load(const std::string & file, int flags = 0)
	{
		return load(file.c_str(), flags);
	}

	bool isLoaded() const
	{
		return m_handle != NULL;
	}

	void *getHandle() const
	{
		return m_handle;
	}

	int getFlags() const
	{
		return m_flags;
	}

	void *getSymbolAddress(const char *name) const;

	void *getSymbolAddress(const std::string & name) const
	{
		return getSymbolAddress(name.c_str());
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
