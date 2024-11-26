////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   MicrophoneStream.cpp
//  Version:     v1.00
//  Created:     1/12/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: implementation of a MicrophoneStream class.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "MicrophoneStream.h"


//////////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////////

CMicrophoneStream::CMicrophoneStream()
{
}

CMicrophoneStream::~CMicrophoneStream()
{
}


void CMicrophoneStream::Release()
{
	//delete this;
}

// ask for butter to be read
bool CMicrophoneStream::ReadDataBuffer(	const unsigned int nBitsPerSample,
																				const unsigned int nSamplesPerSecond,	
																				const unsigned int nNumSamples, 
																				void* pData )
{
	return true;
}

