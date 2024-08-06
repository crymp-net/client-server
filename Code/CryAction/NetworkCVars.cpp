#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "NetworkCVars.h"

NetworkCVars::NetworkCVars()
{
#ifdef BUILD_64BIT
	*reinterpret_cast<NetworkCVars**>(0x3095f6e8) = this;
#else
	*reinterpret_cast<NetworkCVars**>(0x307d6644) = this;
#endif

	IConsole* pConsole = gEnv->pConsole;

	pConsole->Register("g_breakagelog", &this->breakageLog, 0, VF_CHEAT, "Log break events");
	pConsole->Register("cl_voice_volume", &this->voiceVolume, 1.0f, 0, "Set VOIP playback volume: 0-1");
	pConsole->Register("net_phys_pingsmooth", &this->physPingSmooth, 0.1f, 0, "");
	pConsole->Register("net_phys_lagsmooth", &this->physLagSmooth, 0.1f, 0, "");
	pConsole->Register("net_phys_debug", &this->physDebug, 0, VF_CHEAT, "");
	pConsole->Register("g_breaktimeoutframes", &this->breakTimeoutFrames, 140, 0, "");
}

NetworkCVars::~NetworkCVars()
{
	IConsole* pConsole = gEnv->pConsole;

	pConsole->UnregisterVariable("g_breakagelog", true);
	pConsole->UnregisterVariable("cl_voice_volume", true);
	pConsole->UnregisterVariable("net_phys_pingsmooth", true);
	pConsole->UnregisterVariable("net_phys_lagsmooth", true);
	pConsole->UnregisterVariable("net_phys_debug", true);
	pConsole->UnregisterVariable("g_breaktimeoutframes", true);
}
