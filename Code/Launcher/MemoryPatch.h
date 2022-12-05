#pragma once

struct CPUInfo;

namespace MemoryPatch
{
	namespace CryAction
	{
		void AllowDX9ImmersiveMultiplayer(void* pCryAction);
		void DisableBreakLog(void* pCryAction);
	}

	namespace CryNetwork
	{
		void AllowSameCDKeys(void* pCryNetwork);
		void EnablePreordered(void* pCryNetwork);
		void FixFileCheckCrash(void* pCryNetwork);
		void FixInternetConnect(void* pCryNetwork);
	}

	namespace CryRenderD3D9
	{
		void HookWindowNameD3D9(void* pCryRenderD3D9, const char* name);
	}

	namespace CryRenderD3D10
	{
		void FixLowRefreshRateBug(void* pCryRenderD3D10);
		void HookWindowNameD3D10(void* pCryRenderD3D10, const char* name);
	}

	namespace CrySystem
	{
		void AllowDX9VeryHighSpec(void* pCrySystem);
		void AllowMultipleInstances(void* pCrySystem);
		void DisableIOErrorLog(void* pCrySystem);
		void HookCPUDetect(void* pCrySystem, void (*handler)(CPUInfo* info));
		void MakeDX9Default(void* pCrySystem);
		void RemoveSecuROM(void* pCrySystem);
		void UnhandledExceptions(void* pCrySystem);
	}
}
