#ifndef CRYSOUND_MUSICSYSTEM_MUSICPATTERNINSTANCE_H
#define CRYSOUND_MUSICSYSTEM_MUSICPATTERNINSTANCE_H

#pragma once

#include <smartptr.h>

class CMusicPattern;
struct IMusicPatternDecoderInstance;
struct SMusicMood;

class CMusicPatternInstance
{
protected:
	int m_nRefs;
	CMusicPattern *m_pPattern;
	IMusicPatternDecoderInstance *m_pDecoderInstance;
	SMusicMood*						m_pMood;

public:
	CMusicPatternInstance(CMusicPattern *pPattern);
	virtual ~CMusicPatternInstance();
	void AddRef()
	{
		m_nRefs++;
	}
	void Release()
	{
		m_nRefs--;
		if (m_nRefs<=0)
			delete this;
	}
	CMusicPattern*	GetPattern() { return m_pPattern; }
	
	void				SetMood(SMusicMood* pMood)	{ m_pMood = pMood; }
	SMusicMood*	GetMood()										{ return m_pMood; }

	//! Seek to beginning of pattern (if nDelay is set it will wait nDelay-samples before starting playback).
	bool	Seek0(const int nDelayInSamples=0);
	bool	SeekPos(const int nSamplePos);
	int		GetPos();
	bool	GetPCMData(signed long *pDataOut, int nSamples, bool bLoop=true);
	int		GetSamplesToNextFadePoint();
	int		GetSamplesToLastFadePoint();
	int		GetSamplesToEnd();

	bool m_bPlayFromStart;
	bool m_bPlaySingle;
	bool m_bPlayAtOnce;

	int GetRefCount() { return m_nRefs; }

	IMusicPatternDecoderInstance* GetDecoderInstance() { return m_pDecoderInstance; }

		//! Retrieve mem-info.
	void GetMemoryUsage(class ICrySizer* pSizer);
};

#endif