#include <cstring>

#include "Library/WinAPI.h"

#include "Patch.h"

using WinAPI::RVA;
using WinAPI::FillNOP;
using WinAPI::FillMem;

///////////////
// CryAction //
///////////////

/**
 * @brief Allows connecting to DX10 servers with game running in DX9 mode.
 */
void Patch::AllowDX9ImmersiveMultiplayer(void* pCryAction)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCryAction, 0x2B394D), 0x1E);
	FillNOP(RVA(pCryAction, 0x2B6860), 0x1A);
#else
	FillNOP(RVA(pCryAction, 0x1D698A), 0x1A);
	FillNOP(RVA(pCryAction, 0x1D89FC), 0x15);
#endif
}

/**
 * @brief Disables useless "times out" log messages from the Break Replicator.
 */
void Patch::DisableBreakLog(void* pCryAction)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCryAction, 0x2D183F), 0x5);
#else
	FillNOP(RVA(pCryAction, 0x1E987D), 0x5);
#endif
}

////////////////
// CryNetwork //
////////////////

/**
 * @brief Fixes the sporadic crash when file check AKA sv_cheatProtection is enabled.
 */
void Patch::FixFileCheckCrash(void* pCryNetwork)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCryNetwork, 0x14F5B1), 0x4);
	FillNOP(RVA(pCryNetwork, 0x14F5C9), 0x4);
	FillNOP(RVA(pCryNetwork, 0x1533E0), 0x4B);
	FillNOP(RVA(pCryNetwork, 0x14F8E1), 0x4);
	FillNOP(RVA(pCryNetwork, 0x14F8F9), 0x4);
	FillNOP(RVA(pCryNetwork, 0x153823), 0x4A);
#else
	const uint8_t clientCode[] = {
		0x8B, 0x4D, 0xC0,              // mov ecx, dword ptr ss:[ebp-0x40]
		0x83, 0xC1, 0xF4,              // add ecx, 0xFFFFFFF4
		0xFF, 0x09,                    // dec dword ptr ds:[ecx]
		0x8B, 0x4D, 0xBC,              // mov ecx, dword ptr ss:[ebp-0x44]
		0x83, 0xC1, 0xF4,              // add ecx, 0xFFFFFFF4
		0xFF, 0x09,                    // dec dword ptr ds:[ecx]
		0x57,                          // push edi
		0xE8, 0x4F, 0xFA, 0xFF, 0xFF,  // call <crynetwork.sub_39549E03>
		0x8B, 0xF8,                    // mov edi, eax
		0xE8, 0xCA, 0xF7, 0xFF, 0xFF   // call <crynetwork.sub_39549B85>
	};

	FillNOP(RVA(pCryNetwork, 0x4A34F), 0xC);
	FillNOP(RVA(pCryNetwork, 0x4A39E), 0x2F);
	FillMem(RVA(pCryNetwork, 0x4A39E), clientCode, sizeof clientCode);

	const uint8_t serverCode[] = {
		0x8B, 0x75, 0x08,              // mov esi, dword ptr ss:[ebp+0x8]
		0xE8, 0xF0, 0x06, 0xFD, 0xFF,  // call <crynetwork.sub_39501575>
		0x83, 0xC6, 0x04,              // add esi, 0x4
		0xE8, 0xE8, 0x06, 0xFD, 0xFF   // call <crynetwork.sub_39501575>
	};

	FillNOP(RVA(pCryNetwork, 0x30E4F), 0x6);
	FillNOP(RVA(pCryNetwork, 0x30E60), 0xC);
	FillMem(RVA(pCryNetwork, 0x30E7D), serverCode, sizeof serverCode);
#endif
}

/**
 * @brief Allows connecting to Internet servers without GameSpy account.
 */
void Patch::FixInternetConnect(void* pCryNetwork)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCryNetwork, 0x189896), 0x18);
#else
	FillNOP(RVA(pCryNetwork, 0x3FB7C), 0xD);
#endif
}

/**
 * @brief Unlocks advantages of pre-ordered version for everyone.
 * This is both server-side and client-side patch.
 */
void Patch::EnablePreordered(void* pCryNetwork)
{
	const uint8_t code[] = {
#ifdef BUILD_64BIT
		0xC6, 0x83, 0x70, 0xFA, 0x00, 0x00, 0x01  // mov byte ptr ds:[rbx+0xFA70], 0x1
#else
		0xC6, 0x83, 0xC8, 0xF3, 0x00, 0x00, 0x01  // mov byte ptr ds:[ebx+0xF3C8], 0x1
#endif
	};

#ifdef BUILD_64BIT
	FillMem(RVA(pCryNetwork, 0x17C377), code, sizeof code);
#else
	FillMem(RVA(pCryNetwork, 0x43188), code, sizeof code);
#endif
}

/**
 * @brief Prevents server from kicking players with the same CD key.
 * This is server-side patch.
 */
void Patch::AllowSameCDKeys(void* pCryNetwork)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCryNetwork, 0xE0328), 0x47);
#else
	FillNOP(RVA(pCryNetwork, 0x606A5), 0x4);
#endif
}

///////////////
// CrySystem //
///////////////

/**
 * @brief Disables the SecuROM crap in 64-bit CrySystem.
 * It does nothing in 32-bit build.
 */
void Patch::RemoveSecuROM(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCrySystem, 0x470B9), 0x16);
#endif
}

/**
 * @brief Makes DX9 mode the default.
 * It's still possible to use the "-dx10" command line parameter to run the game in DX10 mode.
 */
void Patch::MakeDX9Default(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCrySystem, 0x46719), 0x1F);
#else
	FillNOP(RVA(pCrySystem, 0x59D86), 0x5);
#endif
}

/**
 * @brief Allows Very High settings in DX9 mode.
 */
void Patch::AllowDX9VeryHighSpec(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCrySystem, 0x4674C), 0x54);
#else
	FillNOP(RVA(pCrySystem, 0x59DA8), 0x4B);
#endif
}

/**
 * @brief Allows running multiple instances of Crysis at once.
 * Note that the first check if any instance is already running is normally done in launcher.
 */
void Patch::AllowMultipleInstances(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCrySystem, 0x42BFF), 0x68);
#else
	FillNOP(RVA(pCrySystem, 0x5794F), 0x58);
#endif
}

/**
 * @brief Prevents the engine from installing its own broken unhandled exceptions handler.
 */
void Patch::UnhandledExceptions(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCrySystem, 0x22946), 0x6);
	FillNOP(RVA(pCrySystem, 0x22952), 0x7);
	FillNOP(RVA(pCrySystem, 0x467A5), 0x16);
#else
	FillNOP(RVA(pCrySystem, 0x17D67), 0x5);
	FillNOP(RVA(pCrySystem, 0x17D72), 0xC);
	FillNOP(RVA(pCrySystem, 0x59DF8), 0x13);
#endif
}

/**
 * Hooks CryEngine CPU detection.
 */
void Patch::HookCPUDetect(void* pCrySystem, void (*handler)(CPUInfo* info))
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
	FillMem(RVA(pCrySystem, 0xA7E0), &code, sizeof code);
#else
	FillMem(RVA(pCrySystem, 0xA380), &code, sizeof code);
#endif
}

/**
 * @brief Disables useless "IO Error=" log messages from the Stream Engine.
 */
void Patch::DisableIOErrorLog(void* pCrySystem)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCrySystem, 0x7B23C), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B5E4), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B62C), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B65E), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B692), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B6DF), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B724), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B76A), 0x5);
	FillNOP(RVA(pCrySystem, 0x7B982), 0x5);
	FillNOP(RVA(pCrySystem, 0x7BAE1), 0x5);
	FillNOP(RVA(pCrySystem, 0x7BB98), 0x5);
	FillNOP(RVA(pCrySystem, 0x7BCDA), 0x5);
	FillNOP(RVA(pCrySystem, 0x7C51D), 0x5);
#else
	FillNOP(RVA(pCrySystem, 0x780CB), 0x5);
	FillNOP(RVA(pCrySystem, 0x781AF), 0x5);
	FillNOP(RVA(pCrySystem, 0x78490), 0x5);
	FillNOP(RVA(pCrySystem, 0x784CF), 0x5);
	FillNOP(RVA(pCrySystem, 0x7850D), 0x5);
	FillNOP(RVA(pCrySystem, 0x78549), 0x5);
	FillNOP(RVA(pCrySystem, 0x7859E), 0x5);
	FillNOP(RVA(pCrySystem, 0x785D6), 0x5);
	FillNOP(RVA(pCrySystem, 0x7861C), 0x5);
	FillNOP(RVA(pCrySystem, 0x78744), 0x5);
#endif
}

///////////////////
// CryRenderD3D9 //
///////////////////

static void WindowNameSetter(char* buffer, const char* name)
{
	constexpr std::size_t BUFFER_SIZE = 80;

	std::size_t length = std::strlen(name);
	if (length >= BUFFER_SIZE)
	{
		length = BUFFER_SIZE - 1;
	}

	std::memcpy(buffer, name, length + 1);
}

void Patch::HookWindowNameD3D9(void* pCryRenderD3D9, const char* name)
{
	const void* pSetFunc = &WindowNameSetter;

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

#ifdef BUILD_64BIT
	FillMem(RVA(pCryRenderD3D9, 0xD0FF0), code, sizeof code);
#else
	FillMem(RVA(pCryRenderD3D9, 0x9A8AA), code, sizeof code);
#endif
}

////////////////////
// CryRenderD3D10 //
////////////////////

void Patch::HookWindowNameD3D10(void* pCryRenderD3D10, const char* name)
{
	const void* pSetFunc = &WindowNameSetter;

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

#ifdef BUILD_64BIT
	FillMem(RVA(pCryRenderD3D10, 0xC95DD), code, sizeof code);
#else
	FillMem(RVA(pCryRenderD3D10, 0x99C57), code, sizeof code);
#endif
}

void Patch::FixLowRefreshRateBug(void* pCryRenderD3D10)
{
#ifdef BUILD_64BIT
	FillNOP(RVA(pCryRenderD3D10, 0x1C8F45), 0x4);
#else
	FillNOP(RVA(pCryRenderD3D10, 0x16F3E0), 0x6);
#endif
}
