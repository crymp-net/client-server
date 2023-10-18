#include <cstdint>

#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "ScriptRMI.h"

ScriptRMI::ScriptRMI()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307c9870;
#else
	std::uintptr_t ctor = 0x306e4190;
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
		0x30961068
#else
		0x307d79a8
#endif
	);

	IConsole* pConsole = gEnv->pConsole;

	pCVars->pLogRMICVar = pConsole->RegisterInt("net_log_remote_methods", 0, VF_DUMPTODISK,
		"Log remote method invocations.");
	pCVars->pDisconnectOnRMIErrorCVar = pConsole->RegisterInt("net_disconnect_on_rmi_error", 0, VF_DUMPTODISK,
		"Disconnect remote connections on script exceptions during RMI calls.");
}
