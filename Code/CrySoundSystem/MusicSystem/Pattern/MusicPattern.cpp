#include "StdAfx.h"

#include <algorithm>
#include "CryCommon/CryCore/CrySizer.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "MusicPattern.h"
#include "../MusicSystem.h"
#include "../Decoder/PCMDecoder.h"
#include "../Decoder/ADPCMDecoder.h"
#include "../Decoder/FMODBankDecoder.h"
#include "../Decoder/PatternDecoder.h"

#if !defined(_XENON) && !defined(PS3)
#include "../Decoder/OGGDecoder.h"
#endif

#include "../MusicCVars.h"

//#define TEST

CMusicPattern::CMusicPattern(CMusicSystem *pMusicSystem, const char *pszName,const char *pszFilename)
{
	m_pMusicSystem	= pMusicSystem;
	m_pDecoder			= NULL;
	m_vecFadePoints.clear();
	m_fLayeringVolume = 1.0f;
	m_nPreFadeIn = 0;
	m_sName = pszName;
	m_sFilename = pszFilename;
	m_numPatternInstances = 0;
	m_nSamples = 0;
	
}

CMusicPattern::~CMusicPattern()
{
	Close(); 
}

static bool inline 
FindFile( ICryPak* pPak, const char* pszFilename )
{
	bool bRet( false );
	FILE* pFile( pPak->FOpen( pszFilename, "rb" ) ) ;
	if( 0 != pFile )
	{
		bRet = true;
		pPak->FClose( pFile );
	}
	return( bRet );
}

bool CMusicPattern::Open( const char* sSoundName )
{
	// no valid filename
	if( 0 == sSoundName || strlen( sSoundName ) < 5 )
	{
		return( false );
	}

	if( 0 != m_pDecoder )
	{
		Close();
	}

	ICryPak* pPak( gEnv->pCryPak );
	assert( 0 != pPak );

	//string strFilename( pszFilename );
	CryFixedStringT<128> sFixSoundName (sSoundName);
	CryFixedStringT<128> sFixFileName (sSoundName);

	//if( 0 != stricmp( strFilename.c_str() + strFilename.size() - 4, ".ogg" ) )		
	//{
	//	strFilename = strFilename.substr( 0, strFilename.size() - 4 ) + ".ogg";
	//}

	bool bUseWaveFallback = false;

	switch (m_pMusicSystem->g_nMusicFormat)
	{
	case 0:
		{
			sFixFileName = sFixSoundName.substr( 0, sFixSoundName.size() - 4 ) + ".wav";
			bUseWaveFallback = true;
			break;
		}
	case 1:
		{
			sFixFileName = sFixSoundName.substr( 0, sFixSoundName.size() - 4 ) + ".ogg";

#if !defined(XENON) && !defined(PS3) && !defined(LINUX)
			CDebugAllowFileAccess dafa;
			if (FindFile( pPak, sFixFileName.c_str() ))
			{
				m_pDecoder = new COGGDecoder( (IMusicSystem*)m_pMusicSystem, this );	
				m_pMusicSystem->LogMsg(2, "<Music> Pattern: Initialized OGG Decoder for %s", sFixFileName.c_str() );
			}
			else
#endif
			{
				m_pMusicSystem->LogMsg(1, "[Error] <Music> Pattern: File %s not found... trying .wav", sFixFileName.c_str() );
				bUseWaveFallback = true;
			}

			break;
		}
//	case 2:
//		{
//			sFixFileName = sFixSoundName.substr( 0, sFixSoundName.size() - 4 ) + ".xma";
//
//			if (FindFile( pPak, sFixFileName.c_str() ))
//			{
//#if defined(_XENON)
//			m_pDecoder = new CXMADecoder( (IMusicSystem*)m_pMusicSystem );	
//			m_pMusicSystem->LogMsg(2, "<Music> Pattern: Initialized XMA Decoder for %s", sFixFileName.c_str() );
//#else
//			bUseWaveFallback = true;
//#endif
//			}
//			else
//			{
//				m_pMusicSystem->LogMsg(1, "[Error] <Music> Pattern: File %s not found... trying .wav", sFixFileName.c_str() );
//				bUseWaveFallback = true;
//			}
//
//			break;
//		}
	case 4:
		{
			//int nPosLastSlash = sFixFileName.rfind('/', sFixFileName.size());
			//int nPos2ndLastSlash = sFixFileName.rfind('/', nPosLastSlash-1);

			//sFixSoundName.assign( sFixFileName.substr(nPosLastSlash+1, sFixFileName.size() - nPosLastSlash-5) + ".wav");
			//sFixFileName.assign(  sFixFileName.substr(0, nPosLastSlash+1));
			//sFixFileName += sFixFileName.substr( nPos2ndLastSlash+1, nPosLastSlash - nPos2ndLastSlash-1);
			sFixFileName += ".fsb";

			if (FindFile( pPak, sFixFileName.c_str() ))
			{
				m_pDecoder = (IMusicPatternDecoder*) new CFMODBankDecoder( (IMusicSystem*)m_pMusicSystem, this );	
				m_pMusicSystem->LogMsg(2, "<Music> Pattern: Initialized FMOD Bank Decoder for %s", sFixFileName.c_str() );
			}
			else
			{
				m_pMusicSystem->LogMsg(1, "[Error] <Music> Pattern: File %s not found... trying .wav", sFixFileName.c_str() );
				sFixFileName.assign(sSoundName);
				bUseWaveFallback = true;
			}

			break;
		}
	default:
		{
			break;
		}
	}

	if (bUseWaveFallback)
	{
		
		sFixFileName = sFixFileName.substr( 0, sFixFileName.size() - 4 ) + ".wav";

		FILE* pFile( pPak->FOpen( sFixFileName.c_str(), "rbx" ) );

		if( 0 != pFile )
		{

			// check encoding and spawn PCM or ADPCM decoder accordingly
			SWaveHdr header;
			pPak->FRead( &header, 1, pFile );

			if( header.dwSize < ( 90 - 8 + 12 ) )
			{
				// not enough data, not even one sample
				return( false );
			}

			if( WAVE_FORMAT_ID_ADPCM == header.wOne_0 )
			{
				m_pDecoder = new CADPCMDecoder( (IMusicSystem*)m_pMusicSystem, this );	
				m_pDecoder->Open( sFixFileName.c_str(), sFixSoundName.c_str() );// returns always true, just use here to really open it
				// since we actually do not know in advance how many samples are contained, we need to create a temp instance
				IMusicPatternDecoderInstance* pTempInstance( m_pDecoder->CreateInstance() );
				pTempInstance->Release();
				m_pMusicSystem->LogMsg(2, "<Music> Pattern: Initialized ADPCM Decoder for %s", sFixFileName.c_str() );
			}
			else
			{
				if( WAVE_FORMAT_ID_PCM == header.wOne_0 )
				{
					m_pDecoder = new CPCMDecoder( (IMusicSystem*)m_pMusicSystem, this );	
					m_pMusicSystem->LogMsg(2, "<Music> Pattern: Initialized PCM Decoder for %s", sFixFileName.c_str() );
				}
				else
				{
					m_pMusicSystem->LogMsg(1, "[Warning] <Music> Pattern: AD(PCM) expected... failed to initialize %s", sFixFileName.c_str() );
					return( false );
				}
			}

			pPak->FClose( pFile );
		}
	}

	if ( m_pDecoder )
	{
		if ( !m_pDecoder->Open( sFixFileName.c_str(), sFixSoundName.c_str() ) )
		{
			m_pDecoder->Release();
			m_pDecoder = NULL;
			return( false );
		}
	}
	else
	{
		return( false );
	}

	SMusicPatternFileInfo fileInfo;
	if( false != m_pDecoder->GetFileInfo( fileInfo ) )
	{
		m_nSamples = fileInfo.nSamples;
#ifdef TEST
		signed long *pDat = new signed long[m_nSamples];
		signed long *pDataLength = new signed long[m_nSamples];
		signed long *pData = pDat;
		IMusicPatternDecoderInstance* pTempInstance = m_pDecoder->CreateInstance();
		unsigned int c = m_nSamples;
		pTempInstance->GetPCMData(pDataLength,m_nSamples);
		pTempInstance->Seek0();

		while(c >= 11025)
		{
			pTempInstance->GetPCMData(pData,11025);
			pData += 11025;
			c -= 11025;
		}
		if(c > 0)
		{
			pTempInstance->GetPCMData(pData,c);
		}
		SWaveHdr header;
		FILE *pOrig = fopen("MUSIC/Original.wav","rb");
		FILE *pTest = fopen("MUSIC/Test.wav","wb");
		fread(&header, sizeof(SWaveHdr), 1, pOrig);
		fwrite(&header, 1, sizeof(SWaveHdr), pTest);
		fwrite(pDat, 1, m_nSamples*4, pTest);
		fclose(pTest);
		fclose(pOrig);
		pTempInstance->Release();
		for(int i=0;i<m_nSamples;i++)
		{
			if(pDat[i] != pDataLength[i])
			{
				signed long sl0 = pDat[i];
				signed long sl1 = pDataLength[i];
				int ivo=0;
			}
		}
		delete [] pDat;
		delete [] pDataLength;
#endif
	}

	//m_sFilename = sFixFileName.c_str();

	return true;
}

bool CMusicPattern::Close()
{
	if (!m_pDecoder)
		return false;

	m_pDecoder->Close();
	m_pDecoder->Release();
	m_pDecoder = NULL;
	return true;
}

bool CMusicPattern::AddFadePoint(const int nFadePos)
{
	/*
	SMusicPatternFileInfo FileInfo;
	if (nFadePos<=0)
	{
		if (!GetFileInfo(FileInfo))
			return false;
		nFadePos = FileInfo.nSamples+nFadePos;
	}
	*/
	TMarkerVecIt It=std::find(m_vecFadePoints.begin(), m_vecFadePoints.end(), nFadePos);
	if (It!=m_vecFadePoints.end())	// already in list ?
		return true;
	m_vecFadePoints.push_back(nFadePos);
	std::sort(m_vecFadePoints.begin(), m_vecFadePoints.end());
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CMusicPattern::ClearFadePoints()
{
	m_vecFadePoints.clear();
}

//////////////////////////////////////////////////////////////////////////
IMusicPatternDecoderInstance* CMusicPattern::CreateDecoderInstance()
{
	if (!m_pDecoder)
		return NULL;
	return m_pDecoder->CreateInstance();
}

CMusicPatternInstance* CMusicPattern::CreateInstance()
{
	if (!m_pDecoder)
	{
		// Make new decoder.
		if (!Open( m_sFilename.c_str() ))
		{
			m_pMusicSystem->LogMsg(0, "[Error] <Music> Pattern: Music file %s failed to load",m_sFilename.c_str() );
			return 0;
		}
	}
	
	if (!m_pDecoder)
		return 0;

	m_numPatternInstances++;

	return new CMusicPatternInstance(this);
}

void CMusicPattern::ReleaseInstance( CMusicPatternInstance* pInstance )
{
	//TODO check this one
	m_numPatternInstances--;
	if (m_numPatternInstances <= 0)
	{
		// Can release decoder.
		Close();
	}
}

bool CMusicPattern::GetFileInfo(SMusicPatternFileInfo &FileInfo)
{
	if (!m_pDecoder)
	{
		// Make new decoder.
		if (!Open( m_sFilename.c_str() ))
		{
			m_pMusicSystem->LogMsg(1, "[Error] <Music> Pattern: Music file %s failed to load",m_sFilename.c_str() );
			return false;
		}
		bool bRes = false;
		if (m_pDecoder)
			bRes = m_pDecoder->GetFileInfo(FileInfo);
		Close();
		return bRes;
	}
	else
		return m_pDecoder->GetFileInfo(FileInfo);
}

void CMusicPattern::GetMemoryUsage(class ICrySizer* pSizer)
{
	if (!pSizer->Add(*this))
		return;

	if (m_pDecoder)
		m_pDecoder->GetMemoryUsage(pSizer);
}

