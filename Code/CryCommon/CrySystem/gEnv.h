#pragma once

typedef void (*FrameProfilerSectionCallback)(class CFrameProfilerSection* pSection);

struct SPlatformInfo
{
	unsigned int numCoresAvailableToProcess;

#if defined(WIN32) || defined(WIN64)
	enum EWinVersion
	{
		WinUndetected,
		Win2000,
		WinXP,
		WinSrv2003,
		WinVista
	};

	EWinVersion winVer;
	bool win64Bit;
	bool vistaKB940105Required;
#endif
};

//////////////////////////////////////////////////////////////////////////
// Global environment.
// Contain pointers to all global often needed interfaces.
// This is a faster way to get interface pointer then calling ISystem interface to retrieve one.
// Some pointers can be NULL, use with care.
//////////////////////////////////////////////////////////////////////////
struct SSystemGlobalEnvironment                                   // 32-bit | 64-bit
{                                                                 // ------ | ------
	struct ISystem*                   pSystem;                // 0x0    | 0x0
	struct IGame*                     pGame;                  // 0x4    | 0x8
	struct INetwork*                  pNetwork;               // 0x8    | 0x10
	struct IRenderer*                 pRenderer;              // 0xc    | 0x18
	struct IInput*                    pInput;                 // 0x10   | 0x20
	struct ITimer*                    pTimer;                 // 0x14   | 0x28
	struct IConsole*                  pConsole;               // 0x18   | 0x30
	struct IScriptSystem*             pScriptSystem;          // 0x1c   | 0x38
	struct I3DEngine*                 p3DEngine;              // 0x20   | 0x40
	struct ISoundSystem*              pSoundSystem;           // 0x24   | 0x48
	struct IMusicSystem*              pMusicSystem;           // 0x28   | 0x50
	struct IPhysicalWorld*            pPhysicalWorld;         // 0x2c   | 0x58
	struct IMovieSystem*              pMovieSystem;           // 0x30   | 0x60
	struct IAISystem*                 pAISystem;              // 0x34   | 0x68
	struct IEntitySystem*             pEntitySystem;          // 0x38   | 0x70
	struct ICryFont*                  pCryFont;               // 0x3c   | 0x78
	struct ICryPak*                   pCryPak;                // 0x40   | 0x80
	struct ILog*                      pLog;                   // 0x44   | 0x88
	struct ICharacterManager*         pCharacterManager;      // 0x48   | 0x90
	struct IFrameProfileSystem*       pFrameProfileSystem;    // 0x4c   | 0x98
	struct INameTable*                pNameTable;             // 0x50   | 0xa0
	struct IFlowSystem*               pFlowSystem;            // 0x54   | 0xa8
	struct IAnimationGraphSystem*     pAnimationGraphSystem;  // 0x58   | 0xb0
	struct IDialogSystem*             pDialogSystem;          // 0x5c   | 0xb8
	struct IHardwareMouse*            pHardwareMouse;         // 0x60   | 0xc0
	struct IMaterialEffects*          pMaterialEffects;       // 0x64   | 0xc8

	//////////////////////////////////////////////////////////////////////////
	// Used to tell if this is a server/client/multiplayer instance
	bool bClient;                                             // 0x68   | 0xd0
	bool bServer;                                             // 0x69   | 0xd1
	bool bMultiplayer;                                        // 0x6a   | 0xd2
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Used by frame profiler.
	bool bProfilerEnabled;                                    // 0x6b   | 0xd3
	FrameProfilerSectionCallback callbackStartSection;        // 0x6c   | 0xd8
	FrameProfilerSectionCallback callbackEndSection;          // 0x70   | 0xe0
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Indicate Editor status.
	//////////////////////////////////////////////////////////////////////////
	bool bEditor;          // Engine is running under editor. // 0x74   | 0xe8
	bool bEditorGameMode;  // Engine is in editor game mode.  // 0x75   | 0xe9
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Used by CRY_ASSERT
	bool bIgnoreAllAsserts;                                   // 0x76   | 0xea
	//////////////////////////////////////////////////////////////////////////

	SPlatformInfo pi;
};

//////////////////////////////////////////////////////////////////////////
// Global environment variable.
//////////////////////////////////////////////////////////////////////////
inline SSystemGlobalEnvironment* gEnv;
