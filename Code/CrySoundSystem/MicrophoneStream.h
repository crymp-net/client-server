////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   MicrophoneStream.h
//  Version:     v1.00
//  Created:     1/12/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: implementation of a MicrophoneStream class.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MICROPHONESTREAM_H__
#define __MICROPHONESTREAM_H__

#include "StdAfx.h"
//#include "ISound.h"

#pragma once

class CMicrophoneStream //: public IMicrophoneStream
{

public:
	CMicrophoneStream();
	~CMicrophoneStream();

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	void Release();

	// ask for butter to be read
	bool ReadDataBuffer(	const unsigned int nBitsPerSample,
												const unsigned int nSamplesPerSecond,	
												const unsigned int nNumSamples, 
												void* pData );
	
private:

};
#endif