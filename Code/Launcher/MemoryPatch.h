#pragma once

#include <cstdarg>
#include <cstddef>

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
		void FixLanServerBrowser(void* pCryNetwork);
	}

	namespace CryRenderD3D9
	{
		struct AdapterInfo
		{
			// D3DADAPTER_IDENTIFIER9
			char driver[512];
			char description[512];
			char device_name[32];
			unsigned long driver_version_lo;
			unsigned long driver_version_hi;
			unsigned long vendor_id;
			unsigned long device_id;
			unsigned long sub_sys_id;
			unsigned long revision;
			// ...
		};

		void HookWindowNameD3D9(void* pCryRenderD3D9, const char* name);
		void HookAdapterInfo(void* pCryRenderD3D9, void (*handler)(AdapterInfo* info));
	}

	namespace CryRenderD3D10
	{
		struct AdapterInfo
		{
			void* reserved;
			// DXGI_ADAPTER_DESC
			wchar_t description[128];
			unsigned int vendor_id;
			unsigned int device_id;
			unsigned int sub_sys_id;
			unsigned int revision;
			std::size_t dedicated_video_memory;
			std::size_t dedicated_system_memory;
			std::size_t shared_system_memory;
			// ...
		};

		void FixLowRefreshRateBug(void* pCryRenderD3D10);
		void HookWindowNameD3D10(void* pCryRenderD3D10, const char* name);
		void HookAdapterInfo(void* pCryRenderD3D10, void (*handler)(AdapterInfo* info));
	}

	namespace CrySystem
	{
		void AllowDX9VeryHighSpec(void* pCrySystem);
		void AllowMultipleInstances(void* pCrySystem);
		void DisableIOErrorLog(void* pCrySystem);
		void FixCPUInfoOverflow(void* pCrySystem);
		void HookCPUDetect(void* pCrySystem, void (*handler)(CPUInfo* info));
		void HookError(void* pCrySystem, void (*handler)(const char* format, va_list args));
		void MakeDX9Default(void* pCrySystem);
		void RemoveSecuROM(void* pCrySystem);
		void UnhandledExceptions(void* pCrySystem);
	}
}
