#include "StdAfx.h"

#ifndef SOUNDSYSTEM_USE_XENON_XAUDIO

#include "MusicPatternInstance.h"
#include "MusicPattern.h"
#include "../Decoder/PatternDecoder.h"
#include <CrySizer.h>

CMusicPatternInstance::CMusicPatternInstance(CMusicPattern *pPattern)
{
	m_nRefs						= 0;
	m_pPattern				= pPattern;
	m_pMood						= NULL;
	m_bPlayFromStart	= true;
	m_bPlaySingle			= false;
	m_bPlayAtOnce			= true;

	if (m_pPattern)
		m_pDecoderInstance = m_pPattern->CreateDecoderInstance();
	else
		m_pDecoderInstance = NULL;
}

CMusicPatternInstance::~CMusicPatternInstance()
{
	if (m_pDecoderInstance)
		m_pDecoderInstance->Release();

	m_pPattern->ReleaseInstance( this );

	m_pPattern = 0;
	m_pDecoderInstance = 0;
}

bool CMusicPatternInstance::Seek0(const int nDelayInSamples)
{
	if (!m_pDecoderInstance)
		return false;

	return m_pDecoderInstance->Seek0(nDelayInSamples);
}

bool CMusicPatternInstance::SeekPos(const int nSamplePos)
{
	if (!m_pDecoderInstance)
		return false;

	return m_pDecoderInstance->SeekPos(nSamplePos);
}

int CMusicPatternInstance::GetPos()
{
	if (!m_pDecoderInstance)
		return -1;

	return m_pDecoderInstance->GetPos();
}

bool CMusicPatternInstance::GetPCMData(signed long *pDataOut, int nSamples, bool bLoop)
{
	if (!m_pDecoderInstance)
		return false;

	return m_pDecoderInstance->GetPCMData(pDataOut, nSamples, bLoop);
}

int CMusicPatternInstance::GetSamplesToNextFadePoint()
{
	if (!m_pDecoderInstance)
		return -1;

	int nPos = m_pDecoderInstance->GetPos();

	if (nPos < 0)
		return -1;

	for (TMarkerVecIt It=m_pPattern->m_vecFadePoints.begin();It!=m_pPattern->m_vecFadePoints.end();++It)
	{
		int nFadePos=(*It);
		if (nFadePos <= 0)
			nFadePos = m_pPattern->m_nSamples + nFadePos;

		if (nFadePos>nPos)
			return nFadePos-nPos;
	}
	return GetSamplesToEnd();
}

int CMusicPatternInstance::GetSamplesToLastFadePoint()
{
	if (!m_pDecoderInstance)
		return false;

	int nPos = m_pDecoderInstance->GetPos();
	if (m_pPattern->m_vecFadePoints.empty())
		return GetSamplesToEnd();
	
	int nFadePos = (*(m_pPattern->m_vecFadePoints.end()-1));
	if (nFadePos <= 0)
		nFadePos = m_pPattern->m_nSamples + nFadePos;

	return nFadePos - nPos;
}

int CMusicPatternInstance::GetSamplesToEnd()
{
	if (m_pDecoderInstance != NULL && m_pPattern != NULL && m_pDecoderInstance->Ready())
		return m_pPattern->m_nSamples - m_pDecoderInstance->GetPos();

	return -1;
}

void CMusicPatternInstance::GetMemoryUsage(class ICrySizer* pSizer)
{
	if (!pSizer->Add(*this))
		return;

	if (m_pDecoderInstance)
		m_pDecoderInstance->GetMemoryUsage(pSizer);

}
#endif