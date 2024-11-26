/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the XIPHOPHORUS Company http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: window functions
 last mod: $Id: window.h,v 1.10 2002/01/22 11:59:00 xiphmont Exp $

 ********************************************************************/

#ifndef _V_WINDOW_
#define _V_WINDOW_

#ifdef __cplusplus
extern "C" {
#endif

void _vorbis_window_init();
float *_vorbis_window_get(int type,int left);
void _vorbis_apply_window(float *d,float *window[2],ogg_int32_t *blocksizes, int lW,int W,int nW);

#ifdef __cplusplus
}
#endif

#endif
