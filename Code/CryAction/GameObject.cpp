#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "GameObject.h"

void GameObject::RegisterCVars()
{
	struct {
		int reserved;
		int showUpdateState;
		ICVar* pForceFastUpdateCVar;
		ICVar* pVisibilityTimeoutCVar;
		ICVar* pVisibilityTimeoutTimeCVar;
	} *pCVars = reinterpret_cast<decltype(pCVars)>(
#ifdef BUILD_64BIT
		0x30941ad8
#else
		0x307b9028
#endif
	);

	IConsole* pConsole = gEnv->pConsole;

	pCVars->pForceFastUpdateCVar = pConsole->RegisterInt("g_goForceFastUpdate", 0, VF_CHEAT,
		"GameObjects IsProbablyVisible->TRUE && IsProbablyDistant()->FALSE");
	pCVars->pVisibilityTimeoutCVar = pConsole->RegisterInt("g_VisibilityTimeout", 0, VF_CHEAT,
		"Add visibility timeout to IsProbablyVisible() calculations.");
	pCVars->pVisibilityTimeoutTimeCVar = pConsole->RegisterFloat("g_VisibilityTimeoutTime", 30.0f, VF_CHEAT,
		"Visibility timeout time used by IsProbablyVisible() calculations.");
	pConsole->Register("g_showUpdateState", &pCVars->showUpdateState, 0, VF_CHEAT,
		"Show the game object update state of any activated entities; 3-4 -- AI objects only.");
}
