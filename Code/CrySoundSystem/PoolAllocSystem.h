////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 1999-2010.
//
// -------------------------------------------------------------------------
//  File name:   PoolAllocSystem
//  Version:     v1.00
//  Created:     11/04/2010 by Tomas.
//  Description: This is a helper class to route every allocation by new
//               into the System Memory Pool in the SoundSystem
//
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __POOLALLOCSYSTEM_H__
#define __POOLALLOCSYSTEM_H__

class CPoolAllocSystem
{
public:

	// DISABLE NEW TO PREVENT CONSTRUCTOR OF BEING CALLED ON INVALID MEMORY ON PS3
	//////////////////////////////////////////////////////////////////////////
	// Custom new/delete for System Pool
	//////////////////////////////////////////////////////////////////////////
	//inline void* operator new( size_t nSize )
	//{
	//	void *ptr = g_MemoryPoolPrimary.Allocate<void*>(nSize);
	//	//if (ptr)
	//		//memset( ptr, 0, nSize ); // Clear objects memory.

	//	assert (ptr);

	//	return ptr;
	//}

	//////////////////////////////////////////////////////////////////////////
	inline void operator delete( void *ptr )
	{
		bool bFreed = false;

		if (ptr)
			bFreed = g_MemoryPoolPrimary.Free(ptr);

		assert(bFreed);
	}

};

#endif // __POOLALLOCSYSTEM_H__
