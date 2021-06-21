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

	void Release();

public:
	DLL() = default;

	DLL(const DLL &) = delete;

	DLL(DLL && other)
	{
		Swap(other);
	}

	DLL & operator=(const DLL &) = delete;

	DLL & operator=(DLL && other)
	{
		if (this != &other)
		{
			Unload();

			Swap(other);
		}

		return *this;
	}

	~DLL()
	{
		Unload();
	}

	void Swap(DLL & other)
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_flags, other.m_flags);
	}

	bool Load(const char *name, int flags = 0);

	void Unload()
	{
		if (IsLoaded() && !(m_flags & NO_LOAD) && !(m_flags & NO_RELEASE))
		{
			Release();
		}

		m_handle = nullptr;
		m_flags = 0;
	}

	bool IsLoaded() const
	{
		return m_handle != nullptr;
	}

	explicit operator bool() const
	{
		return IsLoaded();
	}

	void *GetHandle() const
	{
		return m_handle;
	}

	int GetFlags() const
	{
		return m_flags;
	}

	void *GetSymbolAddress(const char *name) const;

	template<class T>
	T GetSymbol(const char *name) const
	{
		return reinterpret_cast<T>(GetSymbolAddress(name));
	}
};
