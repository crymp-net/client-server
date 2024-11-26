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


#ifndef __DEBUGLOGGER_H__
#define __DEBUGLOGGER_H__


#pragma once


class CDebugLogger
{
public:

	CDebugLogger(const char* sFilename);
	~CDebugLogger();

	void	LogString(const char* pString);
	void	LogString(const char* pString, bool bNextLine);
	//void	LogCommand()

protected:

	string m_sFilename;

	FILE	*m_pFile;
};

#endif __DEBUGLOGGER_H__
