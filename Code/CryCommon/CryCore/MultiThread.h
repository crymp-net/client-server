////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2005.
// -------------------------------------------------------------------------
//  File name:   MultiThread.h
//  Version:     v1.00
//  Compilers:   Visual Studio.NET 2003
//  Description:
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef _MSC_VER
#include <intrin.h>
#endif

#define WRITE_LOCK_VAL (1<<16)

inline long CryInterlockedIncrement(int volatile *lpAddend)
{
	return _InterlockedIncrement((volatile long*)lpAddend);
}

inline long CryInterlockedDecrement(int volatile *lpAddend)
{
	return _InterlockedDecrement((volatile long*)lpAddend);
}

inline long CryInterlockedExchangeAdd(long volatile *lpAddend, long Value)
{
	return _InterlockedExchangeAdd(lpAddend, Value);
}

inline long CryInterlockedCompareExchange(long volatile* dst, long exchange, long comperand)
{
	return _InterlockedCompareExchange(dst, exchange, comperand);
}

inline void* CryInterlockedCompareExchangePointer(void* volatile* dst, void* exchange, void* comperand)
{
	return _InterlockedCompareExchangePointer(dst, exchange, comperand);
}

ILINE void CrySpinLock(volatile int *pLock,int checkVal,int setVal)
{
#if !defined(_WIN64)
# ifdef __GNUC__
	register int val;
	__asm__(
		"0:  mov %[checkVal], %%eax\n"
		"    lock cmpxchg %[setVal], (%[pLock])\n"
		"    jnz 0b"
		: "=m" (*pLock)
		: [pLock] "r" (pLock), "m" (*pLock),
		  [checkVal] "m" (checkVal),
		  [setVal] "r" (setVal)
		: "eax"
		);
# else
	__asm
	{
		mov edx, setVal
		mov ecx, pLock
Spin:
		// Trick from Intel Optimizations guide
		pause

		mov eax, checkVal
		lock cmpxchg [ecx], edx
		jnz Spin
	}
# endif
#else
	// NOTE: The code below will fail on 64bit architectures!
	while(_InterlockedCompareExchange((volatile long*)pLock,setVal,checkVal)!=checkVal);
#endif
}

//////////////////////////////////////////////////////////////////////////
ILINE void CryInterlockedAdd(volatile int *pVal, int iAdd)
{
#if !defined(_WIN64)
# ifdef __GNUC__
	__asm__(
		"lock add %[iAdd], (%[pVal])\n"
		: "=m" (*pVal)
		: [pVal] "r" (pVal), "m" (*pVal), [iAdd] "r" (iAdd)
		);
# else
	__asm
	{
		mov edx, pVal
		mov eax, iAdd
		lock add [edx], eax
	}
# endif
#else
	// NOTE: The code below will fail on 64bit architectures!
	_InterlockedExchangeAdd((volatile long*)pVal,iAdd);
#endif
}

//////////////////////////////////////////////////////////////////////////
struct WriteLock
{
	ILINE WriteLock(volatile int &rw) { CrySpinLock(&rw,0,WRITE_LOCK_VAL); prw=&rw; }
	~WriteLock() { CryInterlockedAdd(prw,-WRITE_LOCK_VAL); }
private:
	volatile int *prw;
};

//////////////////////////////////////////////////////////////////////////
struct WriteLockCond
{
	ILINE WriteLockCond(volatile int &rw,int bActive=1)
	{
		if (bActive)
			CrySpinLock(&rw,0,iActive=WRITE_LOCK_VAL);
		else
			iActive = 0;
		prw = &rw;
	}
	~WriteLockCond() { CryInterlockedAdd(prw,-iActive); }
	void SetActive(int bActive=1) { iActive = -bActive & WRITE_LOCK_VAL; }
	void Release() { CryInterlockedAdd(prw,-iActive); }
private:
	volatile int *prw;
	int iActive;
};
