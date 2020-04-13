#pragma once

#include <string>

struct DLL
{
	enum ELoadFlags
	{
		NO_RELEASE = (1 << 0),  //!< Do not unload the library.
		NO_LOAD    = (1 << 1)   //!< Do not load and unload the library. Only obtain handle to it.
	};

private:
	void *m_handle;
	int m_flags;

	void release();

public:
	DLL() = default;

	DLL(const DLL &) = delete;

	DLL(DLL && other)
	: m_handle(other.m_handle),
	  m_flags(other.m_flags)
	{
		other.m_handle = nullptr;
		other.m_flags = 0;
	}

	DLL & operator=(const DLL &) = delete;

	DLL & operator=(DLL && other)
	{
		if (this != &other)
		{
			m_handle = other.m_handle;
			m_flags = other.m_flags;

			other.m_handle = nullptr;
			other.m_flags = 0;
		}

		return *this;
	}

	~DLL()
	{
		unload();
	}

	bool load(const char *file, int flags = 0);

	bool load(const std::string & file, int flags = 0)
	{
		return load(file.c_str(), flags);
	}

	void unload()
	{
		if (isLoaded() && !(m_flags & NO_RELEASE) && !(m_flags & NO_LOAD))
		{
			release();
		}

		m_handle = nullptr;
		m_flags = 0;
	}

	bool isLoaded() const
	{
		return m_handle != nullptr;
	}

	explicit operator bool() const
	{
		return isLoaded();
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
