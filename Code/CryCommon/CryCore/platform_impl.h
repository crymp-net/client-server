////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   platform_impl.h
//  Version:     v1.00
//  Created:     23/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: This file should only be included Once in DLL module.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "platform.h"
#include "CryCommon/CrySystem/ISystem.h"

//////////////////////////////////////////////////////////////////////////
CMTRand_int32 g_random_generator;
uint32 cry_rand32()
{
	return g_random_generator.Generate();
}
unsigned int cry_rand()
{
	return g_random_generator.Generate() & RAND_MAX;
}
float cry_frand()
{
	return g_random_generator.GenerateFloat();
}
//////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CryAssert_impl.h"

//////////////////////////////////////////////////////////////////////////
void CryDebugBreak()
{
	DebugBreak();
}

//////////////////////////////////////////////////////////////////////////
int CryGetCurrentDirectory( unsigned int nBufferLength,char *lpBuffer )
{
	return GetCurrentDirectory(nBufferLength,lpBuffer);
}

//////////////////////////////////////////////////////////////////////////
long  CryInterlockedIncrement( int volatile *lpAddend )
{
	return InterlockedIncrement((long*)lpAddend);
}

//////////////////////////////////////////////////////////////////////////
long  CryInterlockedDecrement( int volatile *lpAddend )
{
	return InterlockedDecrement((long*)lpAddend);
}

//////////////////////////////////////////////////////////////////////////
long	CryInterlockedExchangeAdd(long volatile * lpAddend, long Value)
{
	return InterlockedExchangeAdd(lpAddend, Value);
}

long	CryInterlockedCompareExchange(long volatile * dst, long exchange, long comperand)
{
	return InterlockedCompareExchange(dst, exchange, comperand);
}

void*	CryInterlockedCompareExchangePointer(void* volatile * dst, void* exchange, void* comperand)
{
	return InterlockedCompareExchangePointer(dst, exchange, comperand);
}
