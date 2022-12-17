#include <cstring>

#include "Library/WinAPI.h"

#include "MemoryPatch.h"

static void* ByteOffset(void* base, std::size_t offset)
{
	return static_cast<unsigned char*>(base) + offset;
}

static void FillNop(void* base, std::size_t offset, std::size_t size)
{
	void* address = ByteOffset(base, offset);

	WinAPI::FillNOP(address, size);
}

static void FillMem(void* base, std::size_t offset, const void* data, std::size_t dataSize)
{
	void* address = ByteOffset(base, offset);

	WinAPI::FillMem(address, data, dataSize);
}

////////////////////////////////////////////////////////////////////////////////
// CryAction
////////////////////////////////////////////////////////////////////////////////

/**
 * Allows connecting to DX10 servers with game running in DX9 mode.
 */
void MemoryPatch::CryAction::AllowDX9ImmersiveMultiplayer(void* pCryAction)
{
#ifdef BUILD_64BIT
	FillNop(pCryAction, 0x2B394D, 0x1E);
	FillNop(pCryAction, 0x2B6860, 0x1A);
#else
	FillNop(pCryAction, 0x1D698A, 0x1A);
	FillNop(pCryAction, 0x1D89FC, 0x15);
#endif
}

/**
 * Disables useless "times out" log messages from break replicator.
 */
void MemoryPatch::CryAction::DisableBreakLog(void* pCryAction)
{
#ifdef BUILD_64BIT
	FillNop(pCryAction, 0x2D183F, 0x5);
#else
	FillNop(pCryAction, 0x1E987D, 0x5);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CryNetwork
////////////////////////////////////////////////////////////////////////////////

/**
 * Prevents server from kicking players with the same CD-Key.
 *
 * This is a server-side patch.
 */
void MemoryPatch::CryNetwork::AllowSameCDKeys(void* pCryNetwork)
{
#ifdef BUILD_64BIT
	FillNop(pCryNetwork, 0xE0328, 0x47);
#else
	FillNop(pCryNetwork, 0x606A5, 0x4);
#endif
}

/**
 * Unlocks advantages of pre-ordered version for everyone.
 *
 * This is both server-side and client-side patch.
 */
void MemoryPatch::CryNetwork::EnablePreordered(void* pCryNetwork)
{
	const unsigned char code[] = {
#ifdef BUILD_64BIT
		0xC6, 0x83, 0x70, 0xFA, 0x00, 0x00, 0x01  // mov byte ptr ds:[rbx+0xFA70], 0x1
#else
		0xC6, 0x83, 0xC8, 0xF3, 0x00, 0x00, 0x01  // mov byte ptr ds:[ebx+0xF3C8], 0x1
#endif
	};

#ifdef BUILD_64BIT
	FillMem(pCryNetwork, 0x17C377, code, sizeof code);
#else
	FillMem(pCryNetwork, 0x43188, code, sizeof code);
#endif
}

/**
 * Fixes the sporadic crash when file check (sv_cheatProtection) is enabled.
 *
 * Both client and server are affected. Although server is much less prone to crashing. This patch fixes both.
 */
void MemoryPatch::CryNetwork::FixFileCheckCrash(void* pCryNetwork)
{
#ifdef BUILD_64BIT
	const unsigned char codeA[] = {
		0x48, 0x89, 0x0A,  // mov qword ptr ds:[rdx], rcx
		0x90               // nop
	};

	const unsigned char codeB[] = {
		0x48, 0x89, 0x4A, 0x08  // mov qword ptr ds:[rdx+0x8], rcx
	};
#else
	const unsigned char clientCode[] = {
		0x8B, 0x4D, 0xC0,  // mov ecx, dword ptr ss:[ebp-0x40]
		0xFF, 0x49, 0xF4,  // dec dword ptr ds:[ecx-0xC]
		0x8B, 0x4D, 0xBC,  // mov ecx, dword ptr ss:[ebp-0x44]
		0x89, 0x4D, 0xC0   // mov dword ptr ss:[ebp-0x40], ecx
	};

	const unsigned char serverCode[] = {
		0x90,              // nop
		0x90,              // nop
		0xEB, 0x02,        // jmp -------------------------------+
		0x33, 0xC0,        // xor eax, eax                       |
		0x8B, 0x4F, 0x04,  // mov ecx, dword ptr ds:[edi+0x4] <--+
		0xFF, 0x49, 0xF4,  // dec dword ptr ds:[ecx-0xC]
		0x8B, 0x0F,        // mov ecx, dword ptr ds:[edi]
		0x89, 0x4F, 0x04,  // mov dword ptr ds:[edi+0x4], ecx
		0x90,              // nop
		0x90,              // nop
		0x90               // nop
	};
#endif

#ifdef BUILD_64BIT
	// client
	FillMem(pCryNetwork, 0x14F5B1, codeA, sizeof codeA);
	FillMem(pCryNetwork, 0x14F5C9, codeB, sizeof codeB);
	// server
	FillMem(pCryNetwork, 0x14F8E1, codeA, sizeof codeA);
	FillMem(pCryNetwork, 0x14F8F9, codeB, sizeof codeB);
#else
	// client
	FillNop(pCryNetwork, 0x4A34F, 0xC);
	FillMem(pCryNetwork, 0x4A39E, clientCode, sizeof clientCode);
	// server
	FillNop(pCryNetwork, 0x49F68, 0xC);
	FillMem(pCryNetwork, 0x30E7B, serverCode, sizeof serverCode);
#endif
}

/**
 * Allows connecting to Internet servers without GameSpy account.
 */
void MemoryPatch::CryNetwork::FixInternetConnect(void* pCryNetwork)
{
#ifdef BUILD_64BIT
	FillNop(pCryNetwork, 0x189896, 0x18);
#else
	FillNop(pCryNetwork, 0x3FB7C, 0xD);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CryRenderD3D9
////////////////////////////////////////////////////////////////////////////////

static void HookWindowName(void* pCryRender, std::size_t offset, const char* name)
{
	using SetFunc = void (*)(char* buffer, const char* name);

	const SetFunc pSetFunc = [](char* buffer, const char* name)
	{
		constexpr std::size_t BUFFER_SIZE = 80;

		std::size_t length = std::strlen(name);
		if (length >= BUFFER_SIZE)
		{
			length = BUFFER_SIZE - 1;
		}

		std::memcpy(buffer, name, length + 1);
	};

#ifdef BUILD_64BIT
	unsigned char code[] = {
		0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rdx, 0x0
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xD0                                                   // call rax
	};

	std::memcpy(&code[2], &name, 8);
	std::memcpy(&code[12], &pSetFunc, 8);
#else
	unsigned char code[] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,        // mov eax, 0x0
		0x50,                                // push eax
		0x8D, 0x86, 0x84, 0x2C, 0x02, 0x00,  // lea eax, dword ptr ds:[esi+0x22C84]
		0x50,                                // push eax
		0xB8, 0x00, 0x00, 0x00, 0x00,        // mov eax, 0x0
		0xFF, 0xD0,                          // call eax
		0x33, 0xC0,                          // xor eax, eax
		0x50,                                // push eax
		0x50,                                // push eax
		0x90,                                // nop
		0x90,                                // nop
		0x90                                 // nop
	};

	std::memcpy(&code[1], &name, 4);
	std::memcpy(&code[14], &pSetFunc, 4);
#endif

	FillMem(pCryRender, offset, code, sizeof code);
}

/**
 * Sets a new string to be used as title during game window creation.
 *
 * Note that the name pointer must remain valid until the game window is created.
 */
void MemoryPatch::CryRenderD3D9::HookWindowNameD3D9(void* pCryRenderD3D9, const char* name)
{
#ifdef BUILD_64BIT
	HookWindowName(pCryRenderD3D9, 0xD0FF0, name);
#else
	HookWindowName(pCryRenderD3D9, 0x9A8AA, name);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CryRenderD3D10
////////////////////////////////////////////////////////////////////////////////

/**
 * Prevents DX10 renderer from switching to the lowest refresh rate available.
 *
 * Thanks to Guzz and Vladislav for this patch.
 */
void MemoryPatch::CryRenderD3D10::FixLowRefreshRateBug(void* pCryRenderD3D10)
{
#ifdef BUILD_64BIT
	FillNop(pCryRenderD3D10, 0x1C8F45, 0x4);
#else
	FillNop(pCryRenderD3D10, 0x16F3E0, 0x6);
#endif
}

/**
 * Sets a new string to be used as title during game window creation.
 *
 * Note that the name pointer must remain valid until the game window is created.
 */
void MemoryPatch::CryRenderD3D10::HookWindowNameD3D10(void* pCryRenderD3D10, const char* name)
{
#ifdef BUILD_64BIT
	HookWindowName(pCryRenderD3D10, 0xC95DD, name);
#else
	HookWindowName(pCryRenderD3D10, 0x99C57, name);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CrySystem
////////////////////////////////////////////////////////////////////////////////

/**
 * Allows Very High settings in DX9 mode.
 */
void MemoryPatch::CrySystem::AllowDX9VeryHighSpec(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNop(pCrySystem, 0x4674C, 0x54);
#else
	FillNop(pCrySystem, 0x59DA8, 0x4B);
#endif
}

/**
 * Allows running multiple instances of Crysis at once.
 *
 * Note that the first check if any instance is already running is normally done in launcher.
 */
void MemoryPatch::CrySystem::AllowMultipleInstances(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNop(pCrySystem, 0x42BFF, 0x68);
#else
	FillNop(pCrySystem, 0x5794F, 0x58);
#endif
}

/**
 * Disables useless "IO Error=" log messages from stream engine.
 */
void MemoryPatch::CrySystem::DisableIOErrorLog(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNop(pCrySystem, 0x7B23C, 0x5);
	FillNop(pCrySystem, 0x7B5E4, 0x5);
	FillNop(pCrySystem, 0x7B62C, 0x5);
	FillNop(pCrySystem, 0x7B65E, 0x5);
	FillNop(pCrySystem, 0x7B692, 0x5);
	FillNop(pCrySystem, 0x7B6DF, 0x5);
	FillNop(pCrySystem, 0x7B724, 0x5);
	FillNop(pCrySystem, 0x7B76A, 0x5);
	FillNop(pCrySystem, 0x7B982, 0x5);
	FillNop(pCrySystem, 0x7BAE1, 0x5);
	FillNop(pCrySystem, 0x7BB98, 0x5);
	FillNop(pCrySystem, 0x7BCDA, 0x5);
	FillNop(pCrySystem, 0x7C51D, 0x5);
#else
	FillNop(pCrySystem, 0x780CB, 0x5);
	FillNop(pCrySystem, 0x781AF, 0x5);
	FillNop(pCrySystem, 0x78490, 0x5);
	FillNop(pCrySystem, 0x784CF, 0x5);
	FillNop(pCrySystem, 0x7850D, 0x5);
	FillNop(pCrySystem, 0x78549, 0x5);
	FillNop(pCrySystem, 0x7859E, 0x5);
	FillNop(pCrySystem, 0x785D6, 0x5);
	FillNop(pCrySystem, 0x7861C, 0x5);
	FillNop(pCrySystem, 0x78744, 0x5);
#endif
}

/**
 * Hooks CryEngine CPU detection.
 */
void MemoryPatch::CrySystem::HookCPUDetect(void* pCrySystem, void (*handler)(CPUInfo* info))
{
#ifdef BUILD_64BIT
	unsigned char code[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xE0                                                   // jmp rax
	};

	std::memcpy(&code[2], &handler, 8);
#else
	unsigned char code[] = {
		0x51,                          // push ecx
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
		0xFF, 0xD0,                    // call eax
		0x83, 0xC4, 0x04,              // add esp, 0x4
		0xC3                           // ret
	};

	std::memcpy(&code[2], &handler, 4);
#endif

#ifdef BUILD_64BIT
	FillMem(pCrySystem, 0xA7E0, &code, sizeof code);
#else
	FillMem(pCrySystem, 0xA380, &code, sizeof code);
#endif
}

/**
 * Hooks CryEngine fatal error handler.
 */
void MemoryPatch::CrySystem::HookError(void* pCrySystem, void (*handler)(const char* format, va_list args))
{
	// convert thiscall into a normal function call
	// and call our handler
#ifdef BUILD_64BIT
	unsigned char code[] = {
		0x48, 0x89, 0x54, 0x24, 0x10,                                // mov qword ptr ss:[rsp+0x10], rdx
		0x4C, 0x89, 0x44, 0x24, 0x18,                                // mov qword ptr ss:[rsp+0x18], r8
		0x4C, 0x89, 0x4C, 0x24, 0x20,                                // mov qword ptr ss:[rsp+0x20], r9
		0x48, 0x83, 0xEC, 0x28,                                      // sub rsp, 0x28
		0x48, 0x8B, 0xCA,                                            // mov rcx, rdx
		0x48, 0x8D, 0x54, 0x24, 0x40,                                // lea rdx, qword ptr ss:[rsp+0x40]
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xD0,                                                  // call rax
		0x48, 0x83, 0xC4, 0x28,                                      // add rsp, 0x28
		0xC3                                                         // ret
	};

	std::memcpy(&code[29], &handler, 8);
#else
	unsigned char code[] = {
		0x8B, 0x4C, 0x24, 0x08,        // mov ecx, dword ptr ss:[esp+0x8]
		0x8D, 0x44, 0x24, 0x0C,        // lea eax, dword ptr ss:[esp+0xC]
		0x50,                          // push eax
		0x51,                          // push ecx
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
		0xFF, 0xD0,                    // call eax
		0x83, 0xC4, 0x08,              // add esp, 0x8
		0xC3                           // ret
	};

	std::memcpy(&code[11], &handler, 4);
#endif

#ifdef BUILD_64BIT
	FillMem(pCrySystem, 0x52D00, &code, sizeof code);
#else
	FillMem(pCrySystem, 0x63290, &code, sizeof code);
#endif
}

/**
 * Makes DX9 mode the default.
 *
 * It is still possible to use the "-dx10" command line parameter to run the game in DX10 mode.
 */
void MemoryPatch::CrySystem::MakeDX9Default(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNop(pCrySystem, 0x46719, 0x1F);
#else
	FillNop(pCrySystem, 0x59D86, 0x5);
#endif
}

/**
 * Disables the SecuROM crap in 64-bit CrySystem DLL.
 */
void MemoryPatch::CrySystem::RemoveSecuROM(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNop(pCrySystem, 0x470B9, 0x16);
#endif
}

/**
 * Prevents the engine from installing its own broken unhandled exceptions handler.
 */
void MemoryPatch::CrySystem::UnhandledExceptions(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNop(pCrySystem, 0x22946, 0x6);
	FillNop(pCrySystem, 0x22952, 0x7);
	FillNop(pCrySystem, 0x467A5, 0x16);
#else
	FillNop(pCrySystem, 0x17D67, 0x5);
	FillNop(pCrySystem, 0x17D72, 0xC);
	FillNop(pCrySystem, 0x59DF8, 0x13);
#endif
}
