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
#include "CryCommon/CryCore/MTPseudoRandom.h"

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
