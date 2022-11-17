#pragma once

struct CPUInfo;
struct DLL;

namespace Patch
{
	// CryAction
	void AllowDX9ImmersiveMultiplayer(const DLL & CryAction);
	void DisableBreakLog(const DLL & CryAction);

	// CryNetwork
	void FixFileCheckCrash(const DLL & CryNetwork);
	void FixInternetConnect(const DLL & CryNetwork);
	void EnablePreordered(const DLL & CryNetwork);
	void AllowSameCDKeys(const DLL & CryNetwork);

	// CrySystem
	void RemoveSecuROM(const DLL & CrySystem);
	void MakeDX9Default(const DLL & CrySystem);
	void AllowDX9VeryHighSpec(const DLL & CrySystem);
	void AllowMultipleInstances(const DLL & CrySystem);
	void UnhandledExceptions(const DLL & CrySystem);
	void HookCPUDetect(const DLL & CrySystem, void (*handler)(CPUInfo* info));
	void DisableIOErrorLog(const DLL & CrySystem);

	// CryRenderD3D10
	void FixLowRefreshRateBug(const DLL & CryRenderD3D10);
}
