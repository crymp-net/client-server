#include <cstdint>

#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "GameFrameworkCVars.h"

extern std::uintptr_t CRYACTION_BASE;

GameFrameworkCVars::GameFrameworkCVars()
{
#ifdef BUILD_64BIT
	*reinterpret_cast<GameFrameworkCVars**>(CRYACTION_BASE + 0x465268) = this;
#else
	*reinterpret_cast<GameFrameworkCVars**>(CRYACTION_BASE + 0x2e1c9c) = this;
#endif

	IConsole* pConsole = gEnv->pConsole;

	pConsole->Register("g_playerInteractorRadius", &this->playerInteractorRadius, 2.0f, VF_CHEAT,
		"Maximum radius at which player can interact with other entities");
	pConsole->Register("ai_LogSignals", &this->aiLogSignals, 0, VF_CHEAT, "");
	pConsole->Register("ai_MaxSignalDuration", &this->aiMaxSignalDuration, 3.0f, VF_CHEAT, "");
}

GameFrameworkCVars::~GameFrameworkCVars()
{
	IConsole* pConsole = gEnv->pConsole;

	pConsole->UnregisterVariable("g_playerInteractorRadius", true);
	pConsole->UnregisterVariable("ai_LogSignals", true);
	pConsole->UnregisterVariable("ai_MaxSignalDuration", true);
}
