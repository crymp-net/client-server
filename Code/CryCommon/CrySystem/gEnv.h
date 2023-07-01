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
struct SSystemGlobalEnvironment
{
	struct ISystem*                   pSystem;

	struct IGame*                     pGame;
	struct INetwork*                  pNetwork;
	struct IRenderer*                 pRenderer;
	struct IInput*                    pInput;
	struct ITimer*                    pTimer;
	struct IConsole*                  pConsole;
	struct IScriptSystem*             pScriptSystem;
	struct I3DEngine*                 p3DEngine;
	struct ISoundSystem*              pSoundSystem;
	struct IMusicSystem*              pMusicSystem;
	struct IPhysicalWorld*            pPhysicalWorld;
	struct IMovieSystem*              pMovieSystem;
	struct IAISystem*                 pAISystem;
	struct IEntitySystem*             pEntitySystem;
	struct ICryFont*                  pCryFont;
	struct ICryPak*                   pCryPak;
	struct ILog*                      pLog;
	struct ICharacterManager*         pCharacterManager;
	struct IFrameProfileSystem*       pFrameProfileSystem;
	struct INameTable*                pNameTable;
	struct IFlowSystem*               pFlowSystem;
	struct IAnimationGraphSystem*     pAnimationGraphSystem;
	struct IDialogSystem*             pDialogSystem;
	struct IHardwareMouse*            pHardwareMouse;
	struct IMaterialEffects*          pMaterialEffects;

	//////////////////////////////////////////////////////////////////////////
	// Used to tell if this is a server/client/multiplayer instance
	bool bClient;
	bool bServer;
	bool bMultiplayer;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Used by frame profiler.
	bool bProfilerEnabled;
	FrameProfilerSectionCallback callbackStartSection;
	FrameProfilerSectionCallback callbackEndSection;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Indicate Editor status.
	//////////////////////////////////////////////////////////////////////////
	bool bEditor;          // Engine is running under editor.
	bool bEditorGameMode;  // Engine is in editor game mode.
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Used by CRY_ASSERT
	bool bIgnoreAllAsserts;
	//////////////////////////////////////////////////////////////////////////

	SPlatformInfo pi;
};

//////////////////////////////////////////////////////////////////////////
// Global environment variable.
//////////////////////////////////////////////////////////////////////////
inline SSystemGlobalEnvironment* gEnv;
