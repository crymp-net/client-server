#pragma once

struct CPUInfo;

namespace Patch
{
	// CryAction
	void AllowDX9ImmersiveMultiplayer(void* pCryAction);
	void DisableBreakLog(void* pCryAction);

	// CryNetwork
	void FixFileCheckCrash(void* pCryNetwork);
	void FixInternetConnect(void* pCryNetwork);
	void EnablePreordered(void* pCryNetwork);
	void AllowSameCDKeys(void* pCryNetwork);

	// CrySystem
	void RemoveSecuROM(void* pCrySystem);
	void MakeDX9Default(void* pCrySystem);
	void AllowDX9VeryHighSpec(void* pCrySystem);
	void AllowMultipleInstances(void* pCrySystem);
	void UnhandledExceptions(void* pCrySystem);
	void HookCPUDetect(void* pCrySystem, void (*handler)(CPUInfo* info));
	void DisableIOErrorLog(void* pCrySystem);

	// CryRenderD3D9
	void HookWindowNameD3D9(void* pCryRenderD3D9, const char* name);

	// CryRenderD3D10
	void HookWindowNameD3D10(void* pCryRenderD3D10, const char* name);
	void FixLowRefreshRateBug(void* pCryRenderD3D10);
}
