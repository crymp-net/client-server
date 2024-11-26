/********************************************************************
	Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  File name:   DebugLogger.cpp
	$Id$
  Description: 
  
 -------------------------------------------------------------------------
  History:
  - 1:12:2005   Created by Tomas Neumann

*********************************************************************/

#include "StdAfx.h"
#include "DebugLogger.h"

//
//----------------------------------------------------------------------------------------------
CDebugLogger::CDebugLogger(const char* sFilename)
{
	m_pFile = NULL;
	m_sFilename = sFilename;
}

//
//----------------------------------------------------------------------------------------------
CDebugLogger::~CDebugLogger()
{
	if(m_pFile)
		fclose(m_pFile);
}

void CDebugLogger::LogString(const char* pString)
{
	if (!m_pFile)
	{
		m_pFile = fxopen(m_sFilename.c_str(),"wt");

	LogString("\n--------------------------------------------------------------------------------------------",true);
	//LogString("<SOUND ERROR LOG>",true);
	LogString("--------------------------------------------------------------------------------------------",true);
	}

	LogString(pString, true);
}
//
//----------------------------------------------------------------------------------------------
void CDebugLogger::LogString(const char* pString, bool bNextLine)
{
	if(!m_pFile)
		return;
	char bufferString[512];
	if (bNextLine)
		sprintf(bufferString,"%s \n", pString);
	else
		sprintf(bufferString,"%s", pString);

	fputs(bufferString, m_pFile);
	fflush(m_pFile);
}

//
//----------------------------------------------------------------------------------------------
