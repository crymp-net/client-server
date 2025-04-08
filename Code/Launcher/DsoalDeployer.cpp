#include <cstddef>

#include <windows.h>
#include <dsound.h>

#include "Library/WinAPI.h"

#include "DsoalDeployer.h"

extern "C" HRESULT WINAPI DSOAL_DirectSoundCreate8(const GUID* deviceId, IDirectSound8** ds, IUnknown* outer);
extern "C" HRESULT WINAPI DSOAL_DirectSoundEnumerateA(LPDSENUMCALLBACKA callback, void* userPtr);
extern "C" HRESULT WINAPI DSOAL_DirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA callback, void* userPtr);
extern "C" HRESULT WINAPI DSOAL_DirectSoundCaptureCreate(const GUID* deviceId, IDirectSoundCapture** ds, IUnknown* outer);

struct FmodContext
{
	unsigned char reserved1[0x328];
	unsigned int someFlags;
	void* dsound;
	void* dsound3d;
	void* pDirectSoundCreate8;
	void* pDirectSoundEnumerateA;
	void* reserved2;
	void* reserved3;
	void* pDirectSoundCaptureEnumerateA;
	void* pDirectSoundCaptureCreate;
};

#ifdef BUILD_64BIT
static_assert(offsetof(FmodContext, someFlags) == 0x328);
static_assert(offsetof(FmodContext, dsound) == 0x330);
static_assert(offsetof(FmodContext, dsound3d) == 0x338);
static_assert(offsetof(FmodContext, pDirectSoundCreate8) == 0x340);
static_assert(offsetof(FmodContext, pDirectSoundEnumerateA) == 0x348);
static_assert(offsetof(FmodContext, pDirectSoundCaptureEnumerateA) == 0x360);
static_assert(offsetof(FmodContext, pDirectSoundCaptureCreate) == 0x368);
#else
// TODO: 32-bit
#endif

static void OnDirectSoundInit(FmodContext* context)
{
	// no dsound.dll
	context->dsound = nullptr;
	context->dsound3d = nullptr;

	context->pDirectSoundCreate8 = &DSOAL_DirectSoundCreate8;
	context->pDirectSoundEnumerateA = &DSOAL_DirectSoundEnumerateA;
	context->pDirectSoundCaptureEnumerateA = &DSOAL_DirectSoundCaptureEnumerateA;
	context->pDirectSoundCaptureCreate = &DSOAL_DirectSoundCaptureCreate;

	context->someFlags = 0x9;
}

void DsoalDeployer::Init(void* pFmodEx)
{
	void* pHook = &OnDirectSoundInit;

#ifdef CLIENT_LAUNCHER
#ifdef BUILD_64BIT
	const std::size_t codeOffset = 0x2FE82;
	const std::size_t codeMaxSize = 0x18F;

	unsigned char code[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xD0,                                                  // call rax
	};

	std::memcpy(&code[2], &pHook, 8);
#else
	// TODO: 32-bit
#endif

	static_assert(sizeof(code) <= codeMaxSize);

	WinAPI::FillMem(WinAPI::RVA(pFmodEx, codeOffset), code, sizeof(code));
	WinAPI::FillNOP(WinAPI::RVA(pFmodEx, codeOffset + sizeof(code)), codeMaxSize - sizeof(code));
#endif
}
