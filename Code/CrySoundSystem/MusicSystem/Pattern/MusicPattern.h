#ifndef CRYSOUND_MUSICSYSTEM_MUSICPATTERN_H
#define CRYSOUND_MUSICSYSTEM_MUSICPATTERN_H

#pragma once

#include <vector>
#include "MusicPatternInstance.h"

class CMusicSystem;
struct SMusicPatternFileInfo;
struct IMusicPatternDecoder;

typedef std::vector<int>			TMarkerVec;
typedef TMarkerVec::iterator	TMarkerVecIt;

class CMusicPattern
{
protected:
	CMusicSystem*					m_pMusicSystem;
	IMusicPatternDecoder*	m_pDecoder;
	string								m_sName;
	string								m_sFilename;
	TMarkerVec						m_vecFadePoints;
	int										m_nPreFadeIn;
	float									m_fLayeringVolume;
	int										m_numPatternInstances;
	int										m_nSamples; //!< Number of Samples in pattern.
	
	friend class CMusicPatternInstance;

private:
	IMusicPatternDecoderInstance* CreateDecoderInstance();
public:
	
	CMusicPattern(CMusicSystem *pMusicSystem, const char *pszName,const char *pszFilename);
	~CMusicPattern();

	bool	Open(const char *pszFilename);
	bool	Close();
	
	bool	AddFadePoint(const int nFadePos);
	void	ClearFadePoints();
	
	void	SetPreFadeIn(const int nPreFadeIn) { m_nPreFadeIn = nPreFadeIn; }
	int		GetPreFadeIn() { return m_nPreFadeIn; }
	
	void	SetLayeringVolume(const float fVol) { m_fLayeringVolume=fVol; }
	float GetLayeringVolume() { return m_fLayeringVolume; }
	
	CMusicPatternInstance* CreateInstance();
	void	ReleaseInstance( CMusicPatternInstance* pInstance );

	IMusicPatternDecoder* GetDecoder() { return m_pDecoder; }
	
	//////////////////////////////////////////////////////////////////////////
	//	Sets/Gets
	//////////////////////////////////////////////////////////////////////////
	void				SetFilename( const char *sFilename ) { m_sFilename = sFilename; };
	const char* GetFilename() { return m_sFilename.c_str(); }

	void				SetName(char const* pcName){m_sName = pcName;}
	const char* GetName() { return m_sName.c_str(); }

	void				SetNumSamples(int nSamples) { m_nSamples = nSamples; }
	uint32			GetNumSamples() { return m_nSamples; }

	bool				GetFileInfo(SMusicPatternFileInfo &FileInfo);
	void				GetMemoryUsage(class ICrySizer* pSizer);
	int					GetNumInstances() { return m_numPatternInstances; }
};

#endif