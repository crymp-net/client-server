/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: #ifdef jail to whip a few platforms into the UNIX ideal.
 last mod: $Id: os_types.h,v 1.11 2002/07/19 08:25:51 msmith Exp $

 ********************************************************************/
#ifndef _OS_TYPES_H
#define _OS_TYPES_H

/* make it easy on the folks that want to compile the libs with a
   different malloc than stdlib */
#if 0

#include "../../../../../src/fmod_memory.h"

#define _ogg_malloc          FMOD_Memory_Alloc
#define _ogg_calloc(_x, _y)  FMOD_Memory_Calloc((_x) * (_y))
#define _ogg_realloc         FMOD_Memory_ReAlloc
#define _ogg_free            FMOD_Memory_Free
#else
#define _ogg_malloc          malloc
#define _ogg_calloc          calloc
#define _ogg_realloc         realloc
#define _ogg_free            free
#endif

#if defined(WIN32) || defined(_XENON)

#  ifndef __GNUC__
   /* MSVC/Borland */
   typedef __int64 ogg_int64_t;
   typedef __int32 ogg_int32_t;
   typedef unsigned __int32 ogg_uint32_t;
   typedef __int16 ogg_int16_t;
   
    #define OGG_CDECL _cdecl
    
#  else
   /* Cygwin */
   #include <_G_config.h>
   typedef _G_int64_t ogg_int64_t;
   typedef _G_int32_t ogg_int32_t;
   typedef _G_uint32_t ogg_uint32_t;
   typedef _G_int16_t ogg_int16_t;
   
   #define OGG_CDECL __cdecl
       
#  endif

#elif defined(__MACOSX__) || (defined(__MWERKS__) && defined(__MACH__)) /* MacOS X Framework build */

   #include <sys/types.h>
   #include <stdlib.h>
 
   typedef int16_t ogg_int16_t;
   typedef int32_t ogg_int32_t;
   typedef u_int32_t ogg_uint32_t;
   typedef int64_t ogg_int64_t;
   
   typedef int32_t __int32;
   
   #define OGG_CDECL

#elif defined(__MACOS__)

   #include <MacTypes.h>
   #include <alloca.h>

   typedef SInt16 ogg_int16_t;
   typedef SInt32 ogg_int32_t;
   typedef UInt32 ogg_uint32_t;
   typedef SInt64 ogg_int64_t;
 
   typedef SInt32 __int32;
   
   #define OGG_CDECL
   
#elif defined(__BEOS__)

   /* Be */
#  include <inttypes.h>
   typedef int16_t ogg_int16_t;
   typedef int32_t ogg_int32_t;
   typedef u_int32_t ogg_uint32_t;
   typedef int64_t ogg_int64_t;

#elif defined (__EMX__)

   /* OS/2 GCC */
   typedef short ogg_int16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;

#elif defined (R5900)

   /* PS2 */
   typedef short ogg_int16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long ogg_int64_t;

   #define OGG_CDECL

    #if defined(__MWERKS__)
        #define alloca __alloca
    #endif
    
#elif defined(SN_TARGET_PS3) || defined(_PS3)

    #include <alloca.h>

    typedef signed short          ogg_int16_t;
    typedef signed int            ogg_int32_t;
    typedef unsigned int          ogg_uint32_t;
    typedef signed long long      ogg_int64_t;

    #define OGG_CDECL

#elif defined (GEKKO)

    #if defined(PLATFORM_WII)
        #include <alloca.h>
    #else
        #if defined(__MWERKS__)
            #define alloca __alloca
        #endif
    #endif

   /* Gamecube */
   typedef short ogg_int16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;

    #define OGG_CDECL

#elif defined (__psp__)

   typedef short ogg_int16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;

   #define OGG_CDECL

#elif defined (__sun)

   typedef short ogg_int16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;

   #define OGG_CDECL

#else

#  include <sys/types.h>
#  include <ogg/config_types.h>

   #define OGG_CDECL

#endif

#endif  /* _OS_TYPES_H */
