// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9793C644_C91F_4BA6_A176_44537782901A__INCLUDED_)
#define AFX_STDAFX_H__9793C644_C91F_4BA6_A176_44537782901A__INCLUDED_

#include <CryModuleDefs.h>
#define eCryModule eCryM_SoundSystem
#define RWI_NAME_TAG "RayWorldIntersection(Sound)"
#define PWI_NAME_TAG "PrimitiveWorldIntersection(Sound)"

#define CRYSOUNDSYSTEM_EXPORTS

#include "platform.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// temporary memory leak detection
//////////////////////////////////////////////////////////////////////////
//#include "VisualLeakDetector/vld.h"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Choose one of these SoundLibraries

// FMODEX 400 Win32/Xenon
//#define SOUNDSYSTEM_USE_FMODEX400
//#undef SOUNDSYSTEM_USE_FMODEX400
//
// XAudio Xenon only
//#define SOUNDSYSTEM_USE_XENON_XAUDIO
//#undef SOUNDSYSTEM_USE_XENON_XAUDIO

// Disable Prototype sound library choice

#if defined(_WIN32) && !defined(_WIN64) && !defined(XENON) && !defined(PS3)

	#		pragma message (">>> Win32:")
	#define SOUNDSYSTEM_USE_FMODEX400
	#undef SOUNDSYSTEM_USE_XENON_XAUDIO

	#if defined(CRYSOUNDSYSTEM_FMOD_RELEASE)
	#		pragma message (">>> include lib: fmodex/lib/fmodex_vc.lib")
	#		pragma comment(lib,"fmodex/lib/fmodex_vc.lib")
	#		pragma message (">>> include lib: fmodex/lib/fmod_event.lib")
	#		pragma comment(lib,"fmodex/lib/fmod_event.lib")
	#		pragma message (">>> include lib: fmodex/lib/fmod_event_net.lib")
	#		pragma comment(lib,"fmodex/lib/fmod_event_net.lib")
	#elif defined(CRYSOUNDSYSTEM_FMOD_DEBUG)
	#		pragma message (">>> include lib: fmodex/lib/fmodex_vc.lib")
	#		pragma comment(lib,"fmodex/lib/fmodex_vc.lib")
	#		pragma message (">>> include lib: fmodex/lib/fmod_event.lib")
	#		pragma comment(lib,"fmodex/lib/fmod_event.lib")
	#		pragma message (">>> include lib: fmodex/lib/fmod_event_net.lib")
	#		pragma comment(lib,"fmodex/lib/fmod_event_net.lib")
	#endif

	#if defined(CRYSOUNDSYSTEM_OGGVORBIS_RELEASE)
	#		pragma message (">>> include lib: oggvorbis/lib/ogg/ogg_static.lib")
	#		pragma comment(lib,"oggvorbis/lib/ogg/ogg_static.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbis_static.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbis_static.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbisfile_static.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbisfile_static.lib")
	#elif defined(CRYSOUNDSYSTEM_OGGVORBIS_DEBUG)
	#		pragma message (">>> include lib: oggvorbis/lib/ogg/ogg_static_d.lib")
	#		pragma comment(lib,"oggvorbis/lib/ogg/ogg_static_d.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbis_static_d.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbis_static_d.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbisfile_static_d.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbisfile_static_d.lib")
	#endif

#endif



#if defined(_WIN64) && !defined(XENON) && !defined(PS3)

	#		pragma message (">>> Win64:")
	#define SOUNDSYSTEM_USE_FMODEX400
	#undef SOUNDSYSTEM_USE_XENON_XAUDIO

	#if defined(CRYSOUNDSYSTEM_FMOD_RELEASE)
	#		pragma message (">>> include lib: fmodex/lib/x64/fmodex64_vc.lib")
	#		pragma comment(lib,"fmodex/lib/x64/fmodex64_vc.lib")
	#		pragma message (">>> include lib: fmodex/lib/x64/fmod_event64.lib")
	#		pragma comment(lib,"fmodex/lib/x64/fmod_event64.lib")
	#		pragma message (">>> include lib: fmodex/lib/x64/fmod_event_net64.lib")
	#		pragma comment(lib,"fmodex/lib/x64/fmod_event_net64.lib")
	#elif defined(CRYSOUNDSYSTEM_FMOD_DEBUG)
	#		pragma message (">>> include lib: fmodex/lib/x64/fmodex64_vc.lib")
	#		pragma comment(lib,"fmodex/lib/x64/fmodex64_vc.lib")
	#		pragma message (">>> include lib: fmodex/lib/x64/fmod_event64.lib")
	#		pragma comment(lib,"fmodex/lib/x64/fmod_event64.lib")
	#		pragma message (">>> include lib: fmodex/lib/x64/fmod_event_net64.lib")
	#		pragma comment(lib,"fmodex/lib/x64/fmod_event_net64.lib")
	#endif

	#if defined(CRYSOUNDSYSTEM_OGGVORBIS_RELEASE)
	#		pragma message (">>> include lib: oggvorbis/lib/ogg/ogg_static_x64.lib")
	#		pragma comment(lib,"oggvorbis/lib/ogg/ogg_static_x64.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbis_static_x64.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbis_static_x64.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbisfile_static_x64.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbisfile_static_x64.lib")
	#elif defined(CRYSOUNDSYSTEM_OGGVORBIS_DEBUG)
	#		pragma message (">>> include lib: oggvorbis/lib/ogg/ogg_static_x64_d.lib")
	#		pragma comment(lib,"oggvorbis/lib/ogg/ogg_static_x64_d.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbis_static_x64_d.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbis_static_x64_d.lib")
	#		pragma message (">>> include lib: oggvorbis/lib/vorbis/vorbisfile_static_x64_d.lib")
	#		pragma comment(lib,"oggvorbis/lib/vorbis/vorbisfile_static_x64_d.lib")
#endif

#endif








































































	#define PS3_MEM_FLAG 0


#include "ProjectDefines.h"


#include <memory>
#include <map>
#include <vector>
#include <set>
#include <algorithm>





#include <ILog.h>
#include <IConsole.h>
#include <ISound.h>
#include <ISystem.h>

#include <Cry_Math.h>
#include <ICryPak.h>
//#include <vector.h>


inline void __cdecl __CRYTEKDLL_TRACE(const char *sFormat, ... ) PRINTF_PARAMS(1, 2);
inline void __cdecl __CRYTEKDLL_TRACE(const char *sFormat, ... )
{
	va_list vl;
	char sTraceString[1024];
	
	va_start(vl, sFormat);
	vsprintf_s(sTraceString, sFormat, vl);
	va_end(vl);

	strcat(sTraceString, "\n");

	CryLog( "%s",sTraceString );
	//::OutputDebugString(sTraceString);	
}

#define TRACE __CRYTEKDLL_TRACE

#if /* defined(_DEBUG) && */ defined(WIN32)
//class CHeapGuardian
//{
//public: CHeapGuardian() {assert (_CrtCheckMemory());} ~CHeapGuardian() {assert (_CrtCheckMemory());}
//};


//#define GUARD_HEAP //CHeapGuardian __heap_guardian

//#pragma message (">>> ´Guard Heap enable")
#define GUARD_HEAP	//if (_HEAPOK != _heapchk()) __asm int 3

#else
//#pragma message (">>> ´Guard Heap disable")
#define GUARD_HEAP
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9793C644_C91F_4BA6_A176_44537782901A__INCLUDED_)
