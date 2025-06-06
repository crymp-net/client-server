#include <cstdint>

#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "GameObject.h"

extern std::uintptr_t CRYACTION_BASE;

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
		CRYACTION_BASE + 0x441ad8
#else
		CRYACTION_BASE + 0x2b9028
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

void GameObject::UpdateSchedulingProfiles()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1AD1E0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x12A480;
#endif

	reinterpret_cast<void(*)()>(func)();
}
