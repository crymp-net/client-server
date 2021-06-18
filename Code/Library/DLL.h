#pragma once

#include <utility>

struct DLL
{
	// flags
	enum
	{
		NO_RELEASE = (1 << 0),  // do not unload the library
		NO_LOAD    = (1 << 1),  // do not load and unload the library, only obtain its handle
	};

private:
	void *m_handle = nullptr;
	int m_flags = 0;

	void release();

public:
	DLL() = default;

	DLL(const DLL &) = delete;

	DLL(DLL && other)
	{
		swap(other);
	}

	DLL & operator=(const DLL &) = delete;

	DLL & operator=(DLL && other)
	{
		if (this != &other)
		{
			unload();

			swap(other);
		}

		return *this;
	}

	~DLL()
	{
		unload();
	}

	void swap(DLL & other)
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_flags, other.m_flags);
	}

	bool load(const char *name, int flags = 0);

	void unload()
	{
		if (isLoaded() && !(m_flags & NO_LOAD) && !(m_flags & NO_RELEASE))
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

	template<class T>
	T getSymbol(const char *name) const
	{
		return reinterpret_cast<T>(getSymbolAddress(name));
	}
};
