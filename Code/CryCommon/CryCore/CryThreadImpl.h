/////////////////////////////////////////////////////////////////////////////
//
// Crytek Source File
// Copyright (C), Crytek Studios, 2001-2006.
//
// History:
// Jun 20, 2006: Created by Sascha Demetrio
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CryThreadImpl_h_
#define _CryThreadImpl_h_ 1

#include "CryThread.h"

#include "CryCommon/CrySystem/IThreadTask.h"

void CryThreadSetName( unsigned int dwThreadId,const char *sThreadName )
{
	if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIThreadTaskManager())
		gEnv->pSystem->GetIThreadTaskManager()->SetThreadName( dwThreadId,sThreadName );
}

const char* CryThreadGetName( unsigned int dwThreadId )
{
	if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIThreadTaskManager())
		return gEnv->pSystem->GetIThreadTaskManager()->GetThreadName( dwThreadId );
	return "";
}

#endif

// vim:ts=2

