// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _SMART_PTR_H_
#define _SMART_PTR_H_

#include <atomic>
#include <cassert>

//////////////////////////////////////////////////////////////////
// SMART POINTER
//////////////////////////////////////////////////////////////////
template <class _I> class _smart_ptr 
{
private:
  _I* p;
public:
  _smart_ptr() : p(NULL) {}
#if defined(LINUX64)
	_smart_ptr(typeof(__null)) : p(NULL) {}
#endif
	_smart_ptr(int Null) : p(NULL) {}
  _smart_ptr(_I* p_)
	{
		p = p_;
		if (p)
			p->AddRef();
	}
	
  _smart_ptr(const _smart_ptr &p_)
	{
		p = p_.p;
		if (p)
			p->AddRef();
	}
	~_smart_ptr()
	{
		if (p)
			p->Release();
	}
  operator _I*() const { return p; } 
  operator const _I*() const { return p; }
  _I& operator*() const { return *p; }
  _I* operator->(void) const { return p; }
	_I* get() const { return p; }
  _smart_ptr&  operator=(_I* newp)
	{
		if (newp)
			newp->AddRef();
		if (p)
			p->Release();
		p = newp;
		return *this;
	}
	_smart_ptr&  operator=(const _smart_ptr &newp)
	{
		if (newp.p)
			newp.p->AddRef();
		if (p)
			p->Release();
		p = newp.p;
		return *this;
	}
  operator bool() const 
	{
		return p != NULL;
	};
	bool operator !() const 
	{
		return p == NULL;
	};
 	bool  operator ==(const _I* p2) const 
	{
		return p == p2;
	};
 	bool  operator ==(_I* p2) const 
	{
		return p == p2;
	};
	bool  operator ==(const _smart_ptr<_I>& rhs) const
	{
		return p == rhs.p;
	}
  bool  operator !=(const _I* p2) const 
	{
		return p != p2;
	};
	bool  operator !=(_I* p2) const 
	{
		return p != p2;
	};
	bool  operator !=(const _smart_ptr &p2) const 
	{
		return p != p2.p;
	};
  bool  operator <(const _I* p2) const 
	{
		return p < p2;
	};
  bool  operator >(const _I* p2) const 
	{
		return p > p2;
	};
	// set our contained pointer to null, but don't call Release()
	// useful for when we want to do that ourselves, or when we know that
	// the contained pointer is invalid
	friend _I* ReleaseOwnership( _smart_ptr<_I>& ptr )
	{
		_I * ret = ptr.p;
		ptr.p = 0;
		return ret;
	}
	void Swap( _smart_ptr<_I>& other )
	{
		std::swap(p, other.p);
	}
};

template <class _I>
inline bool operator ==(const _smart_ptr<_I> &p1, int null)	
{
	assert(!null);
	return !(bool)p1;	
}
template <class _I>
inline bool operator !=(const _smart_ptr<_I> &p1, int null)
{
	assert(!null);
	return (bool)p1;	
}
template <class _I>
inline bool operator ==(int null, const _smart_ptr<_I> &p1)
{
	assert(!null);
	return !(bool)p1;	
}
template <class _I>
inline bool operator !=(int null, const _smart_ptr<_I> &p1)
{
	assert(!null);
	return (bool)p1;	
}

#if defined(LINUX64)
template <class _I>
inline bool operator ==(const _smart_ptr<_I> &p1, typeof(__null))	
{
	return !(bool)p1;	
}
template <class _I>
inline bool operator !=(const _smart_ptr<_I> &p1, typeof(__null))
{
	return (bool)p1;	
}
template <class _I>
inline bool operator ==(typeof(__null), const _smart_ptr<_I> &p1)
{
	return !(bool)p1;	
}
template <class _I>
inline bool operator !=(typeof(__null), const _smart_ptr<_I> &p1)
{
	return (bool)p1;	
}
#endif //LINUX64

// reference target for smart pointer
// implements AddRef() and Release() strategy using reference counter of the specified type
template <typename Counter> class _reference_target
{
public:
	_reference_target():
		m_nRefCounter (0)
	{
	}

	virtual ~_reference_target()
	{
		//assert (!m_nRefCounter);
	}

	void AddRef()
	{
		++m_nRefCounter;
	}

	void Release()
	{
		if (--m_nRefCounter <= 0)
			delete this;
	}
	// Warning: use for debugging/statistics purposes only!
	Counter NumRefs()
	{
		return m_nRefCounter;
	}
protected:
	Counter m_nRefCounter;
};

// default implementation is int counter - for better alignment
typedef _reference_target<int> _reference_target_t;


// base class for interfaces implementing reference counting
// derive your interface from this class and the descendants won't have to implement
// the reference counting logic
template <typename Counter> class _i_reference_target
{
public:
	_i_reference_target():
		m_nRefCounter (0)
	{
	}

	virtual ~_i_reference_target()
	{
	}

	virtual void AddRef()
	{
		++m_nRefCounter;
	}

	virtual void Release()
	{
		if (--m_nRefCounter <= 0)
			delete this;
	}

	// Warning: use for debugging/statistics purposes only!
	Counter NumRefs()	const
	{
		return m_nRefCounter;
	}
protected:
	Counter m_nRefCounter;
};

typedef _i_reference_target<int> _i_reference_target_t;

class CMultiThreadRefCount
{
	std::atomic<int> m_count = 0;

public:
	CMultiThreadRefCount() = default;

	virtual ~CMultiThreadRefCount() = 0;

	void AddRef()
	{
		m_count.fetch_add(1);
	}

	void Release()
	{
		if ((m_count.fetch_sub(1) - 1) <= 0)
		{
			delete this;
		}
	}
};

inline CMultiThreadRefCount::~CMultiThreadRefCount() {}

// TYPEDEF_AUTOPTR macro, declares Class_AutoPtr, which is the smart pointer to the given class,
#define TYPEDEF_AUTOPTR(T) typedef _smart_ptr<T> T##_AutoPtr;

#endif //_SMART_PTR_H_
