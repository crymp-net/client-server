#include <cstdint>

#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "ScriptRMI.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptRMI::ScriptRMI()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2c9870;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1e4190;
#endif

	(this->*reinterpret_cast<void(ScriptRMI::*&)()>(ctor))();
}

void ScriptRMI::RegisterCVars()
{
	struct {
		ICVar* pLogRMICVar;
		ICVar* pDisconnectOnRMIErrorCVar;
	} *pCVars = reinterpret_cast<decltype(pCVars)>(
#ifdef BUILD_64BIT
		CRYACTION_BASE + 0x461068
#else
		CRYACTION_BASE + 0x2d79a8
#endif
	);

	IConsole* pConsole = gEnv->pConsole;

	pCVars->pLogRMICVar = pConsole->RegisterInt("net_log_remote_methods", 0, VF_DUMPTODISK,
		"Log remote method invocations.");
	pCVars->pDisconnectOnRMIErrorCVar = pConsole->RegisterInt("net_disconnect_on_rmi_error", 0, VF_DUMPTODISK,
		"Disconnect remote connections on script exceptions during RMI calls.");
}
