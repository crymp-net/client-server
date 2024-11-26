#ifndef __STLSOUNDALLOCATOR_H__
#define __STLSOUNDALLOCATOR_H__

template <class T>
class STLSoundAllocator
{
public:
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;

	template <class U> struct rebind
	{
		typedef STLSoundAllocator<U> other;
	};

	STLSoundAllocator() throw() { }
	STLSoundAllocator(const STLSoundAllocator&) throw() { }
	template <class U> STLSoundAllocator(const STLSoundAllocator<U>&) throw() { }

	pointer address(reference x) const
	{
		return &x;
	}

	const_pointer address(const_reference x) const
	{
		return &x;
	}

	pointer allocate(size_type n = 1, const void* hint = 0)
	{
		(void)hint;
		extern CSoundAllocator g_MemoryPoolPrimary;
		return static_cast<pointer>(g_MemoryPoolPrimary.AllocateRaw(n * sizeof(T), 16, "STL"));
	}

	void deallocate(pointer p, size_type n = 1)
	{
		(void)n;
		extern CSoundAllocator g_MemoryPoolPrimary;
		g_MemoryPoolPrimary.Free(p);
	}

	size_type max_size() const throw()
	{
		return INT_MAX;
	}

	void construct(pointer p, const T& val)
	{
		new(static_cast<void*>(p)) T(val);
	}

	void construct(pointer p)
	{
		new(static_cast<void*>(p)) T();
	}

	void destroy(pointer p)
	{
		p->~T();
	}

	pointer new_pointer()
	{
		return new(allocate()) T();
	}

	pointer new_pointer(const T& val)
	{
		return new(allocate()) T(val);
	}

	void delete_pointer(pointer p)
	{
		p->~T();
		deallocate(p);
	}

	bool operator==(const STLSoundAllocator&) {return true;}
	bool operator!=(const STLSoundAllocator&) {return false;}

	static void GetMemoryUsage( ICrySizer *pSizer ) { }
};

#endif
