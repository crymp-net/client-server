////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   MoodManager.h
//  Version:     v1.00
//  Created:     25/10/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: MoodManager is responsible for organising categories into 
//							 moods and blending these together
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MoodManager.h"
#include "Sound.h"
#include "CryCommon/CryRenderer/IRenderer.h"
#include "ISoundBuffer.h"
#include "ISoundAssetManager.h"
#include "IAudioDevice.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "SoundSystem.h"
#include "CryCommon/CryNetwork/ISerialize.h"

#if defined(SOUNDSYSTEM_USE_FMODEX400)
//#include "FmodEx/inc/fmod_dsp.h"
#include "FmodEx/inc/fmod_errors.h"
#define IS_FMODERROR (m_ExResult != FMOD_OK )
#endif

//inline float round_up(float f, float r)
//{
//	float m = fmod(f, r);
//	if (m > 0.f)
//		f += r-m;
//	return f;
//}
//
inline float round_near( float f, int precision )
{
	// TODO replace current float values in SoundMoodManager with int representation or other solution
	// using hexadecimal pow to reduce float division artefacts
	float r = powf(8.f, static_cast<float>(precision));
	float n = (float)(int)ceilf( f * r );
	return (n / r);
}


//////////////////////////////////////////////////////////////////////////
// CCategory Definition
//////////////////////////////////////////////////////////////////////////

CCategory::CCategory(CMoodManager* pGroupManager, ICategory* pParent, CMood* pMood)
{
	assert (pGroupManager);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	m_ExResult = FMOD_OK;
	m_pDSPLowPass = 0;
	m_pDSPHighPass = 0;	
	m_pDSPCompressor = 0;
	m_pDSPParamEQ1 = 0;
	m_pDSPParamEQ2 = 0;
	m_pPlatformCategory = NULL;
	m_fMaxCategoryVolume = -1.0f;
#endif

	m_pMoodManager			= pGroupManager;
	m_pMood							= pMood;
	m_nFlags						= 0;
	m_ParentCategory		= NULL;


	// Attributes
	m_nAttributes = 0;
	m_fVolume			= 1.0f;
	m_fPitch			= 0.0f;
	// Lowpass
	m_fLowPassCutoff	= 22000.0f;
	// Highpass
	m_fHighPassCutoff	= 10.0f;
	m_fHighPassResonance= 1.0f;
  // Compressor
	m_fCompressorThreshold = 0.0f;
	m_fCompressorAttack = 50.0f;
	m_fCompressorRelease = 50.0f;
	m_fCompressorGainMakeup = 0.0f;
	// ParamEQ1
	m_fParamEQ1Gain = 1.0f;
	m_fParamEQ1Center = 8000.0f;
	m_fParamEQ1Bandwidth = 1.0f;
	// ParamEQ2
	m_fParamEQ2Gain = 1.0f;
	m_fParamEQ2Center = 8000.0f;
	m_fParamEQ2Bandwidth = 1.0f;
}

CCategory::~CCategory()
{
//	delete m_pElementData;

	//m_pMoodManager->RemoveNameFromManager(m_Handle);

	//tvecHandlesIter IterEnd = m_Children.end();
	//for (tvecHandlesIter Iter = m_Children.begin(); Iter != IterEnd; ++Iter)
	//{
	//	CategoryHandle nHandle((*Iter));
	//	RemoveChild(nHandle);
	//}
	//m_Children.clear();

	//AddRef();
	//AddRef();

	//if (m_Handle.IsCategoryIDValid())
	//{
	//	m_pMoodManager->m_CategoryArray[m_Handle.m_CategoryID] = NULL;
	//	//tvecElementsIter IterEnd = m_pMoodManager->m_ChildrenArray.end();
	//	//for (tvecElementsIter Iter = m_pMoodManager->m_ChildrenArray.begin(); Iter!=IterEnd; ++Iter)
	//	//	if (m_Handle.GetIndex() == (*Iter)->GetHandle().GetIndex())
	//	//		m_pMoodManager->m_ChildrenArray.erase(Iter);
	//}
	//else 
	//	if (m_Handle.IsMoodIDValid())
	//	{
	//		m_pMoodManager->m_MoodArray[m_Handle.m_MoodID] = NULL;

	//		//tvecElementsIter IterEnd = m_pMoodManager->m_GroupsArray.end();
	//		//for (tvecElementsIter Iter = m_pMoodManager->m_GroupsArray.begin(); Iter!=IterEnd; ++Iter)
	//		//	if (m_Handle.GetIndex() == (*Iter)->GetHandle().GetIndex())
	//		//		m_pMoodManager->m_GroupsArray.erase(Iter);
	//	}
		//Release();
		//Release();

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		// remove DSP
		{
			FMOD::System		*pFMODSystem = (FMOD::System*) gEnv->pSoundSystem->GetIAudioDevice();

			if (pFMODSystem)
			{
				if (m_pDSPLowPass)
				{
					m_ExResult = m_pDSPLowPass->remove();
					m_ExResult = m_pDSPLowPass->release();
					m_pDSPLowPass = 0;
					SetDSPForAll(FLAG_SOUNDMOOD_LOWPASS, 0);
				}

				if (m_pDSPHighPass)
				{
					m_ExResult = m_pDSPHighPass->remove();
					m_ExResult = m_pDSPHighPass->release();
					m_pDSPHighPass = 0;
					SetDSPForAll(FLAG_SOUNDMOOD_HIGHPASS, 0);
				}

				if (m_pDSPCompressor)
				{
					m_ExResult = m_pDSPCompressor->remove();
					m_ExResult = m_pDSPCompressor->release();
					m_pDSPCompressor = 0;
					SetDSPForAll(FLAG_SOUNDMOOD_COMPRESSOR, 0);
				}

				if (m_pDSPParamEQ1)
				{
					m_ExResult = m_pDSPParamEQ1->remove();
					m_ExResult = m_pDSPParamEQ1->release();
					m_pDSPParamEQ1 = 0;
					SetDSPForAll(FLAG_SOUNDMOOD_PARAMEQ1, 0);
				}

				if (m_pDSPParamEQ2)
				{
					m_ExResult = m_pDSPParamEQ2->remove();
					m_ExResult = m_pDSPParamEQ2->release();
					m_pDSPParamEQ2 = 0;
					SetDSPForAll(FLAG_SOUNDMOOD_PARAMEQ2, 0);
				}

			}
		}
#endif

		//m_Props.clear();
		//CategoryHandle Invalid;
		//m_Handle.SetHandle(Invalid.GetIndex());
}

#if defined(SOUNDSYSTEM_USE_FMODEX400)
void CCategory::FmodErrorOutput(const char * sDescription, ESoundLogType LogType)
{
	gEnv->pSoundSystem->Log(LogType, "<Sound> MoodManager: %s (%d) %s\n", sDescription, m_ExResult, FMOD_ErrorString(m_ExResult));
}
#endif

const char* CCategory::GetName() const
{ 
	return m_sName.c_str();
	//return (m_pMoodManager->GetCategoryName(m_Handle));
}

const char* CCategory::GetMoodName() const
{ 
	return m_pMood->GetName();
}

void* CCategory::GetPlatformCategory() const 
{ 
	#if defined(SOUNDSYSTEM_USE_FMODEX400)
	return m_pPlatformCategory; 
	#endif

	return NULL;
}

void	CCategory::SetPlatformCategory(void *pPlatformCategory) 
{ 
	#if defined(SOUNDSYSTEM_USE_FMODEX400)
	m_pPlatformCategory = (FMOD::EventCategory*)pPlatformCategory; 

	if (m_pPlatformCategory)
	{
		if (m_fMaxCategoryVolume < 0)
			m_ExResult = m_pPlatformCategory->getVolume(&m_fMaxCategoryVolume);


	}
	#endif
}



//f32 CCategory::GetVolume() const
//{
//	f32 fVolume = 0.0;
//	ptParam *pParam = GetParamPtr(gspFVOLUME);
//	
//	if (pParam)
//		pParam->GetValue(fVolume);
//
//	return m_fVolume;
//}

void CCategory::SetMuted(bool bMuted)
{
	if (bMuted)
		m_nFlags |= FLAG_SOUNDMOOD_MUTED;
	else
		m_nFlags &= ~FLAG_SOUNDMOOD_MUTED;
}

uint32 CCategory::GetChildCount()
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD_RESULT Result;
	int nNum = 0;

	if (m_pPlatformCategory)
		Result = m_pPlatformCategory->getNumCategories(&nNum);
	(void)Result;

	return nNum;
#else
	return 0;
#endif
}

ICategory* CCategory::GetCategoryByIndex(uint32 nCategoryCount) const
{
	ICategory* pElement = NULL;

	//if (!m_Children.empty() && nCategoryCount < m_Children.size())
	//{
	//	CategoryHandle nHandle(m_Children[nCategoryCount]);
	//	pElement = m_pMoodManager->m_CategoryArray[nHandle.m_CategoryID];
	//}
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD::EventCategory* pFMODCat = NULL;
	FMOD_RESULT Result;

	if (m_pPlatformCategory)
		Result = m_pPlatformCategory->getCategoryByIndex(nCategoryCount, &pFMODCat);

	if (pFMODCat && Result == FMOD_OK)
	{
		pElement = GetCategoryByPtr((void*)pFMODCat);
	}
#endif

	return pElement;
}

ICategory*	CCategory::GetCategory(const char *sCategoryName) const
{
	ICategory *pElement = NULL;

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	if (m_pPlatformCategory)
	{
		FMOD::EventCategory* pSearch = NULL;
		FMOD_RESULT Result;
		Result = m_pPlatformCategory->getCategory(sCategoryName, &pSearch);
		if (pSearch && Result==FMOD_OK)
		{
			pElement = m_pMood->GetCategoryByPtr((void*)pSearch);
		}
	}
#endif

	return pElement;

	//tvecHandlesIterConst IterEnd = m_Children.end();
	//for (tvecHandlesIterConst Iter = m_Children.begin(); Iter != IterEnd; ++Iter)
	//{
	//	CategoryHandle nHandle((*Iter));
	//	//int nSize = m_pMoodManager->m_CategoryArray.size();
	//	//int nIndex = nHandle.GetIndex();
	//	ICategory *pIter = m_pMoodManager->m_CategoryArray[nHandle.m_CategoryID];
	//	if (pIter)
	//	{
	//		if (strcmp(sCategoryName, pIter->GetName()) == 0)
	//		{
	//			pElement = pIter;
	//			break;
	//		}
	//	}
	//}

	//return pElement;
}

ICategory* CCategory::GetCategoryByPtr(void *pPlatformCategory) const
{
	ICategory *pElement = NULL;

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	pElement = m_pMood->GetCategoryByPtr(pPlatformCategory);
#endif

	return pElement;
}


ICategory* CCategory::AddCategory(const char *sCategoryName)
{
	ICategory* pChild = m_pMood->AddCategory(sCategoryName);
	
	if (pChild)
		pChild->SetParentCategory(this);
	
	return pChild;
	
	//ICategory *pNewChild = GetCategory(sCategoryName);

	//if (!pNewChild)
	//{
	//	bool bNewEntry = true;
	//	//CategoryHandle nHandle(m_Handle), nHandleFailed;
	//	//nHandle.m_CategoryID = m_pMoodManager->m_CategoryArray.size();

	//	// fill up holes in the array first
	//	uint32 i = 0;
	//	for (i=0; i<m_pMoodManager->m_CategoryArray.size(); ++i)
	//	{
	//		if ( m_pMoodManager->m_CategoryArray[i] == NULL)
	//		{
	//			//nHandle.m_CategoryID = i;
	//			bNewEntry = false;
	//			break;
	//		}
	//	}

	//	//if (nHandle.IsCategoryIDValid())
	//	pNewChild = new CCategory(m_pMoodManager, m_Handle, m_pMood);

	//	if (!pNewChild)
	//		return nHandleFailed;

	//	//pNewChild->SetHandle(nHandle);
	//	((CCategory*)pNewChild)->m_sName = sCategoryName;

	//	if (bNewEntry)
	//	{
	//		m_pMoodManager->m_CategoryArray.push_back(pNewChild);

	//		bool bFound = false;
	//		tvecCategoryIter ItEnd = m_pMood->m_vecCategories.end();
	//		for (tvecCategoryIter It = m_pMood->m_vecCategories.begin(); It != ItEnd; ++It)
	//		{
	//			ICategory* pCat = (*It);
	//			if (strcmp(sCategoryName, pCat->GetName()) == 0)
	//			{
	//				bFound = true;
	//				break;
	//			}
	//		}

	//		if (!bFound)
	//			m_pMood->m_vecCategories.push_back(pNewChild);
	//	}
	//	else
	//	{
	//		m_pMood->m_vecCategories.push_back(pNewChild);
	//		//m_pMoodManager->m_CategoryArray[i] = pNewChild;
	//	}

	//	const char *sName = sCategoryName;
	//	//m_pMoodManager->AddNameToManager(nHandle, sName);
	//	m_Children.push_back(nHandle.GetIndex());
	//	m_pMoodManager->UpdateMemUsage(sizeof(CCategory));
	//}

	//	return pNewChild->GetHandle();
}


bool CCategory::RemoveCategory(const char *sCategoryName)
{
	bool bResult = false;

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	if (m_pPlatformCategory)
	{
		FMOD::EventCategory *pSearch = NULL;
		m_pPlatformCategory->getCategory(sCategoryName, &pSearch);

		if (pSearch)
			bResult = m_pMood->RemoveCategory(sCategoryName);
	}
#endif

	return bResult;
}

bool CCategory::GetParam(enumGroupParamSemantics eSemantics, ptParam* pParam) const
{
	bool bResult = true;

	/*gspNONE,
	gspFLAGS,
	gspVOLUMEDB,
	gspTIMEOUTMS*/

	switch (eSemantics)
	{
	case gspNONE:
		break;
	case gspFLAGS:
		{
			int32 nTemp = 0;

			if (!(pParam->GetValue(nTemp))) 
				return (false);

			nTemp = m_nFlags;
			pParam->SetValue(nTemp);
			break;
		}
	case gspFVOLUME:
		{
			f32 fTemp = 1.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (!(m_nAttributes & FLAG_SOUNDMOOD_VOLUME))
			{
				pParam->SetValue(1.0f);
				return false;
			}

			pParam->SetValue(m_fVolume);

			// return false on default
			if (m_fVolume == 1.0f)
				return false;

			break;
		}
	case gspFPITCH:
		{
			f32 fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (!(m_nAttributes & FLAG_SOUNDMOOD_PITCH))
			{
				pParam->SetValue(0.0f);
				return false;
			}

			pParam->SetValue(m_fPitch);

			// return false on default
			if (m_fPitch == 0.0f)
				return false;


			break;
		}
	case gspFLOWPASSCUTOFF:
		{
			f32 fTemp = 22000.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (!(m_nAttributes & FLAG_SOUNDMOOD_LOWPASS))
			{
				pParam->SetValue(22000.0f);
				return false;
			}

			pParam->SetValue(m_fLowPassCutoff);

			// return false on default
			if (m_fLowPassCutoff == 22000.0f)
				return false;

			break;
		}
	case gspFHIGHPASSCUTOFF:
		{
			f32 fTemp = 10.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (!(m_nAttributes & FLAG_SOUNDMOOD_HIGHPASS))
			{
				pParam->SetValue(10.0f);
				return false;
			}

			pParam->SetValue(m_fHighPassCutoff);

			// return false on default
			if (m_fHighPassCutoff == 10.0f)
				return false;

			break;
		}
	case gspFHIGHPASSRESONANCE:
		{
			f32 fTemp = 1.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_HIGHPASS))
			//{
			//	pParam->SetValue(1.0f);
			//	return false;
			//}

			pParam->SetValue(m_fHighPassResonance);

			// return false on default
			if (m_fHighPassResonance == 1.0f)
				return false;

			break;
		}
	case gspFCOMPRESSORTHRESHOLD:
		{
			f32 fTemp = 0.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (!(m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR))
			{
				pParam->SetValue(0.0f);
				return false;
			}

			pParam->SetValue(m_fCompressorThreshold);

			// return false on default
			if (m_fCompressorThreshold == 0.0f)
				return false;

			break;
		}
	case gspFCOMPRESSORATTACK:
		{
			f32 fTemp = 50.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR))
			//{
			//	pParam->SetValue(50.0f);
			//	return false;
			//}

			pParam->SetValue(m_fCompressorAttack);

			// return false on default
			if (m_fCompressorAttack == 50.0f)
				return false;

			break;
		}
	case gspFCOMPRESSORRELEASE:
		{
			f32 fTemp = 50.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR))
			//{
			//	pParam->SetValue(50.0f);
			//	return false;
			//}

			pParam->SetValue(m_fCompressorRelease);

			// return false on default
			if (m_fCompressorRelease == 50.0f)
				return false;

			break;
		}
	case gspFCOMPRESSORGAINMAKEUP:
		{
			f32 fTemp = 0.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR))
			//{
			//	pParam->SetValue(0.0f);
			//	return false;
			//}

			pParam->SetValue(m_fCompressorGainMakeup);

			// return false on default
			if (m_fCompressorGainMakeup == 0.0f)
				return false;

			break;
		}
	case gspFPARAMEQ1GAIN:
		{
			f32 fTemp = 1.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (!(m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1))
			{
				pParam->SetValue(1.0f);
				return false;
			}

			pParam->SetValue(m_fParamEQ1Gain);

			// return false on default
			if (m_fParamEQ1Gain == 1.0f)
				return false;


			break;
		}
	case gspFPARAMEQ1CENTER:
		{
			f32 fTemp = 8000.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1))
			//{
			//	pParam->SetValue(8000.0f);
			//	return false;
			//}

			pParam->SetValue(m_fParamEQ1Center);

			// return false on default
			if (m_fParamEQ1Center == 8000.0f)
				return false;

			break;
		}
	case gspFPARAMEQ1BANDWIDTH:
		{
			f32 fTemp = 1.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1))
			//{
			//	pParam->SetValue(1.0f);
			//	return false;
			//}

			pParam->SetValue(m_fParamEQ1Bandwidth);

			// return false on default
			if (m_fParamEQ1Bandwidth == 1.0f)
				return false;

			break;
		}
	case gspFPARAMEQ2GAIN:
		{
			f32 fTemp = 1.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (!(m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2))
			{
				pParam->SetValue(1.0f);
				return false;
			}

			pParam->SetValue(m_fParamEQ2Gain);

			// return false on default
			if (m_fParamEQ2Gain == 1.0f)
				return false;

			break;
		}
	case gspFPARAMEQ2CENTER:
		{
			f32 fTemp = 8000.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2))
			//{
			//	pParam->SetValue(8000.0f);
			//	return false;
			//}

			pParam->SetValue(m_fParamEQ2Center);

			// return false on default
			if (m_fParamEQ2Center == 8000.0f)
				return false;

			break;
		}
	case gspFPARAMEQ2BANDWIDTH:
		{
			f32 fTemp = 1.0f;

			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (!(m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2))
			//{
			//	pParam->SetValue(1.0f);
			//	return false;
			//}

			pParam->SetValue(m_fParamEQ2Bandwidth);

			// return false on default
			if (m_fParamEQ2Bandwidth == 1.0f)
				return false;

			break;
		}
	case gspMUTED:
		{
			bool bTemp = false;
			if (!(pParam->GetValue(bTemp))) 
				return (false);

			bTemp = (m_nFlags & FLAG_SOUNDMOOD_MUTED) != 0;
			pParam->SetValue(bTemp);
			break;
		}

	default:
		break;
	}


	return bResult;

}

bool CCategory::SetParam(enumGroupParamSemantics eSemantics, ptParam* pParam)
{
	bool bResult = true;

	/*gspNONE,
	gspFLAGS,
	gspVOLUMEDB,
	gspTIMEOUTMS*/

	switch (eSemantics)
	{
	case gspNONE:
		break;
	
	case gspFLAGS:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			m_nFlags = nTemp;
			break;
		}
		break;

	case gspMUTED:
		{
			bool bTemp = false;
			if (!(pParam->GetValue(bTemp))) 
				return (false);

			if (bTemp)
				m_nFlags |= FLAG_SOUNDMOOD_MUTED;
			else
				m_nFlags &= ~FLAG_SOUNDMOOD_MUTED;

			break;
		}

	case gspFVOLUME:
		{
			f32 fTemp = 1.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fVolume = clamp(fTemp, 0.0f, 1.0f);
			
			if (m_fVolume < 1.0f)
				m_nAttributes |= FLAG_SOUNDMOOD_VOLUME;
			else
				m_nAttributes &= ~FLAG_SOUNDMOOD_VOLUME;

			break;
		}

	case gspFPITCH:
		{
			f32 fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fPitch = fTemp;

			if (m_fPitch != 0.0f)
				m_nAttributes |= FLAG_SOUNDMOOD_PITCH;
			else
				m_nAttributes &= ~FLAG_SOUNDMOOD_PITCH;

			break;
		}

	case gspFLOWPASSCUTOFF:
		{
			f32 fTemp = 22000.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fLowPassCutoff = clamp(fTemp, 10.0f, 22000.0f);
			
			if (m_fLowPassCutoff < 22000.0f)
				m_nAttributes |= FLAG_SOUNDMOOD_LOWPASS;
			else
				m_nAttributes &= ~FLAG_SOUNDMOOD_LOWPASS;

			break;
		}
	case gspFHIGHPASSCUTOFF:
		{
			f32 fTemp = 10.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fHighPassCutoff = clamp(fTemp, 10.0f, 22000.0f);

			if (m_fHighPassCutoff > 10.0f)
				m_nAttributes |= FLAG_SOUNDMOOD_HIGHPASS;
			else
				m_nAttributes &= ~FLAG_SOUNDMOOD_HIGHPASS;

			break;
		}
	case gspFHIGHPASSRESONANCE:
		{
			f32 fTemp = 1.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fHighPassResonance = clamp(fTemp, 1.0f, 10.0f);

			break;
		}
	case gspFCOMPRESSORTHRESHOLD:
		{
			f32 fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fCompressorThreshold = clamp(fTemp, -60.0f, 0.0f);

			if (m_fCompressorThreshold != 0.0f)
				m_nAttributes |= FLAG_SOUNDMOOD_COMPRESSOR;
			else
				m_nAttributes &= ~FLAG_SOUNDMOOD_COMPRESSOR;

			break;
		}
	case gspFCOMPRESSORATTACK:
		{
			f32 fTemp = 50.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fCompressorAttack = clamp(fTemp, 10.0f, 200.0f);

			break;
		}
	case gspFCOMPRESSORRELEASE:
		{
			f32 fTemp = 50.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fCompressorRelease = clamp(fTemp, 20.0f, 1000.0f);

			break;
		}
	case gspFCOMPRESSORGAINMAKEUP:
		{
			f32 fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fCompressorGainMakeup = clamp(fTemp, 0.0f, 30.0f);

			break;
		}
	case gspFPARAMEQ1GAIN:
		{
			f32 fTemp = 1.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fParamEQ1Gain = clamp(fTemp, 0.05f, 3.0f);

			if (m_fParamEQ1Gain != 1.0f)
				m_nAttributes |= FLAG_SOUNDMOOD_PARAMEQ1;
			else
				m_nAttributes &= ~FLAG_SOUNDMOOD_PARAMEQ1;

			break;
		}
	case gspFPARAMEQ1CENTER:
		{
			f32 fTemp = 8000.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fParamEQ1Center = clamp(fTemp, 20.0f, 22000.0f);

			break;
		}
	case gspFPARAMEQ1BANDWIDTH:
		{
			f32 fTemp = 1.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fParamEQ1Bandwidth = clamp(fTemp, 0.2f, 5.0f);

			break;
		}
	case gspFPARAMEQ2GAIN:
		{
			f32 fTemp = 1.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fParamEQ2Gain = clamp(fTemp, 0.05f, 3.0f);

			if (m_fParamEQ2Gain != 1.0f)
				m_nAttributes |= FLAG_SOUNDMOOD_PARAMEQ2;
			else
				m_nAttributes &= ~FLAG_SOUNDMOOD_PARAMEQ2;

			break;
		}
	case gspFPARAMEQ2CENTER:
		{
			f32 fTemp = 8000.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fParamEQ2Center = clamp(fTemp, 20.0f, 22000.0f);

			break;
		}
	case gspFPARAMEQ2BANDWIDTH:
		{
			f32 fTemp = 1.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_fParamEQ2Bandwidth = clamp(fTemp, 0.2f, 5.0f);

			break;
		}
	default:
		break;
	}


	return bResult;
}


bool CCategory::ImportProps(XmlNodeRef &xmlNode)
{
	bool bResult = true;
	f32  fTemp = 0.0f;
	ptParamF32 Param(fTemp);

	//	if (xmlNode->getAttr("Muted", bTemp))
	//	{
	//	ptParamBOOL Param(bTemp);
	//	if (!SetParam(gspMUTED, &Param))
	//		bResult = false;
	//}

	//	if (xmlNode->getAttr("Solo", bTemp))
	// {
	//	ptParamBOOL Param(bTemp);
	//	if (!SetParam(gspSOLO, &Param))
	//		bResult = false;
	//}

	// Volume
	if (xmlNode->getAttr("Volume", m_fVolume))
		m_nAttributes |= FLAG_SOUNDMOOD_VOLUME;

	// Pitch
	if (xmlNode->getAttr("Pitch", m_fPitch))
		m_nAttributes |= FLAG_SOUNDMOOD_PITCH;
		
	// Lowpass
	if (xmlNode->getAttr("LowPassCutoff", m_fLowPassCutoff))
		m_nAttributes |= FLAG_SOUNDMOOD_LOWPASS;

	// Highpass
	if (xmlNode->getAttr("HighPassCutoff", m_fHighPassCutoff))
		m_nAttributes |= FLAG_SOUNDMOOD_HIGHPASS;

	xmlNode->getAttr("HighPassResonance", m_fHighPassResonance);

	// Compressor
	if (xmlNode->getAttr("CompressorThreshold", m_fCompressorThreshold))
	{
		m_nAttributes |= FLAG_SOUNDMOOD_COMPRESSOR;

		xmlNode->getAttr("CompressorAttack", m_fCompressorAttack);
		xmlNode->getAttr("CompressorRelease", m_fCompressorRelease);
		xmlNode->getAttr("CompressorGainMakeup", m_fCompressorGainMakeup);
	}

	// ParamEQ1
	if (xmlNode->getAttr("ParamEQ1Gain", m_fParamEQ1Gain))
	{
		m_nAttributes |= FLAG_SOUNDMOOD_PARAMEQ1;

		xmlNode->getAttr("ParamEQ1Center", m_fParamEQ1Center);
		xmlNode->getAttr("ParamEQ1Bandwidth", m_fParamEQ1Bandwidth);
	}

	// ParamEQ2
	if (xmlNode->getAttr("ParamEQ2Gain", m_fParamEQ2Gain))
	{
		m_nAttributes |= FLAG_SOUNDMOOD_PARAMEQ2;

		xmlNode->getAttr("ParamEQ2Center", m_fParamEQ2Center);
		xmlNode->getAttr("ParamEQ2Bandwidth", m_fParamEQ2Bandwidth);
	}

	return bResult;
}


bool CCategory::ExportProps(XmlNodeRef &xmlNode)
{
	bool bResult = true;

	// Save the Flags

	// save other Properties
	if (m_nAttributes & FLAG_SOUNDMOOD_VOLUME && m_fVolume < 1.0f)
	{
		xmlNode->setAttr("Volume", m_fVolume);
	}

	if (m_nAttributes & FLAG_SOUNDMOOD_PITCH && m_fPitch != 0.0f)
	{
		xmlNode->setAttr("Pitch", m_fPitch);
	}

	if (m_nAttributes & FLAG_SOUNDMOOD_LOWPASS && m_fLowPassCutoff < 22000.0f)
	{
		xmlNode->setAttr("LowPassCutoff", m_fLowPassCutoff);
	}

	if (m_nAttributes & FLAG_SOUNDMOOD_HIGHPASS && m_fHighPassCutoff > 10.0f)
	{
		xmlNode->setAttr("HighPassCutoff", m_fHighPassCutoff);
		xmlNode->setAttr("HighPassResonance", m_fHighPassResonance);
	}

	if (m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR && m_fCompressorThreshold < 0.0f)
	{
		xmlNode->setAttr("CompressorThreshold", m_fCompressorThreshold);
		xmlNode->setAttr("CompressorAttack", m_fCompressorAttack);
		xmlNode->setAttr("CompressorRelease", m_fCompressorRelease);
		xmlNode->setAttr("CompressorGainMakeup", m_fCompressorGainMakeup);
	}

	if (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1 && m_fParamEQ1Gain != 1.0f)
	{
		xmlNode->setAttr("ParamEQ1Gain", m_fParamEQ1Gain);
		xmlNode->setAttr("ParamEQ1Center", m_fParamEQ1Center);
		xmlNode->setAttr("ParamEQ1Bandwidth", m_fParamEQ1Bandwidth);
	}

	if (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2 && m_fParamEQ2Gain != 1.0f)
	{
		xmlNode->setAttr("ParamEQ2Gain", m_fParamEQ2Gain);
		xmlNode->setAttr("ParamEQ2Center", m_fParamEQ2Center);
		xmlNode->setAttr("ParamEQ2Bandwidth", m_fParamEQ2Bandwidth);
	}


	return bResult;
}


bool CCategory::InterpolateCategories(ICategory *pCat1, float fFade1, ICategory *pCat2, float fFade2, bool bForceDefault)
{
	if (pCat1 && pCat2)
	{
		// PlatformCategory
		//SetPlatformCategory(pCat1->GetPlatformCategory());

		// both Cats
		bool bProp1 = false;
		bool bProp2 = false;
		float fValue1 = 0.0f;
		float fValue2 = 0.0f;
		ptParamF32 fTemp1(fValue1);
		ptParamF32 fTemp2(fValue2);

		// Volume
		bProp1 = pCat1->GetParam(gspFVOLUME, &fTemp1) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_VOLUME;
		bProp2 = pCat2->GetParam(gspFVOLUME, &fTemp2) || pCat2->GetAttributes() & FLAG_SOUNDMOOD_VOLUME;

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		float fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFVOLUME, &fTemp1);
		m_nAttributes |= (bForceDefault || (bProp1 || bProp2)) ? FLAG_SOUNDMOOD_VOLUME : 0;

		// Pitch
		bProp1 = pCat1->GetParam(gspFPITCH, &fTemp1) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_PITCH;
		bProp2 = pCat2->GetParam(gspFPITCH, &fTemp2) || pCat2->GetAttributes() & FLAG_SOUNDMOOD_PITCH;

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFPITCH, &fTemp1);
		m_nAttributes |= (bForceDefault || (bProp1 || bProp2)) ? FLAG_SOUNDMOOD_PITCH : 0;

		// Lowpass
		bProp1 = pCat1->GetParam(gspFLOWPASSCUTOFF, &fTemp1) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_LOWPASS;
		bProp2 = pCat2->GetParam(gspFLOWPASSCUTOFF, &fTemp2) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_LOWPASS;

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFLOWPASSCUTOFF, &fTemp1);
		m_nAttributes |= (bForceDefault || (bProp1 || bProp2)) ? FLAG_SOUNDMOOD_LOWPASS : 0;

		// Highpass
		bProp1 = pCat1->GetParam(gspFHIGHPASSCUTOFF, &fTemp1) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_HIGHPASS;
		bProp2 = pCat2->GetParam(gspFHIGHPASSCUTOFF, &fTemp2) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_HIGHPASS;

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFHIGHPASSCUTOFF, &fTemp1);
		m_nAttributes |= (bForceDefault || (bProp1 || bProp2)) ? FLAG_SOUNDMOOD_HIGHPASS : 0;

		bProp1 = pCat1->GetParam(gspFHIGHPASSRESONANCE, &fTemp1);
		bProp2 = pCat2->GetParam(gspFHIGHPASSRESONANCE, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFHIGHPASSRESONANCE, &fTemp1);

		// Compressor
		bProp1 = pCat1->GetParam(gspFCOMPRESSORTHRESHOLD, &fTemp1) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_COMPRESSOR;
		bProp2 = pCat2->GetParam(gspFCOMPRESSORTHRESHOLD, &fTemp2) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_COMPRESSOR;

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFCOMPRESSORTHRESHOLD, &fTemp1);
		m_nAttributes |= (bForceDefault || (bProp1 || bProp2)) ? FLAG_SOUNDMOOD_COMPRESSOR : 0;

		bProp1 = pCat1->GetParam(gspFCOMPRESSORATTACK, &fTemp1);
		bProp2 = pCat2->GetParam(gspFCOMPRESSORATTACK, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFCOMPRESSORATTACK, &fTemp1);

		bProp1 = pCat1->GetParam(gspFCOMPRESSORRELEASE, &fTemp1);
		bProp2 = pCat2->GetParam(gspFCOMPRESSORRELEASE, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFCOMPRESSORRELEASE, &fTemp1);

		bProp1 = pCat1->GetParam(gspFCOMPRESSORGAINMAKEUP, &fTemp1);
		bProp2 = pCat2->GetParam(gspFCOMPRESSORGAINMAKEUP, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFCOMPRESSORGAINMAKEUP, &fTemp1);

		// ParamEQ1
		bProp1 = pCat1->GetParam(gspFPARAMEQ1GAIN, &fTemp1) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_PARAMEQ1;
		bProp2 = pCat2->GetParam(gspFPARAMEQ1GAIN, &fTemp2) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_PARAMEQ1;

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFPARAMEQ1GAIN, &fTemp1);
		m_nAttributes |= (bForceDefault || (bProp1 || bProp2)) ? FLAG_SOUNDMOOD_PARAMEQ1 : 0;

		bProp1 = pCat1->GetParam(gspFPARAMEQ1CENTER, &fTemp1);
		bProp2 = pCat2->GetParam(gspFPARAMEQ1CENTER, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFPARAMEQ1CENTER, &fTemp1);

		bProp1 = pCat1->GetParam(gspFPARAMEQ1BANDWIDTH, &fTemp1);
		bProp2 = pCat2->GetParam(gspFPARAMEQ1BANDWIDTH, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFPARAMEQ1BANDWIDTH, &fTemp1);

		// ParamEQ2
		bProp1 = pCat1->GetParam(gspFPARAMEQ2GAIN, &fTemp1) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_PARAMEQ2;
		bProp2 = pCat2->GetParam(gspFPARAMEQ2GAIN, &fTemp2) || pCat1->GetAttributes() & FLAG_SOUNDMOOD_PARAMEQ2;

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFPARAMEQ2GAIN, &fTemp1);
		m_nAttributes |= (bForceDefault || (bProp1 || bProp2)) ? FLAG_SOUNDMOOD_PARAMEQ2 : 0;

		bProp1 = pCat1->GetParam(gspFPARAMEQ2CENTER, &fTemp1);
		bProp2 = pCat2->GetParam(gspFPARAMEQ2CENTER, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFPARAMEQ2CENTER, &fTemp1);

		bProp1 = pCat1->GetParam(gspFPARAMEQ2BANDWIDTH, &fTemp1);
		bProp2 = pCat2->GetParam(gspFPARAMEQ2BANDWIDTH, &fTemp2);

		fTemp1.GetValue(fValue1);
		fTemp2.GetValue(fValue2);
		fResult = (fValue1*fFade1 + fValue2*fFade2);
		fResult = round_near(fResult, 2);
		fTemp1.SetValue(fResult);
		SetParam(gspFPARAMEQ2BANDWIDTH, &fTemp1);
	}

	return true;

}

bool CCategory::Serialize(XmlNodeRef &node, bool bLoading)
{
	bool bResult = true;

	if (bLoading)
	{
		// import from node

		// quick test
		if (strcmp("Category", node->getTag())!= 0)
			return false;		// no known Tag

		// parse Properties
		if (!ImportProps(node))
			bResult = false;

		// go through Childs and parse them
		int nGroupChildCount = node->getChildCount();
		for (int i=0 ; i < nGroupChildCount; ++i)
		{
			XmlNodeRef GroupChildNode = node->getChild(i);

			// read the name of the child
			const char* sChildName = GroupChildNode->getAttr("Name");
			ICategory *pNewChild = AddCategory(sChildName);

			if (!pNewChild)
				return false; // something wrong !

			bool bChildResult = pNewChild->Serialize(GroupChildNode, true);

			if (!bChildResult)
			{
				RemoveCategory(sChildName);
				bResult = false;
			}
		} // done with all children

		//RefreshCategories
		if (m_pMoodManager)
			m_pMoodManager->m_bNeedUpdate = true;
	}
	else
	{
		node->setTag("Category");
		node->setAttr("Name", GetName());

		// parse Properties
		if (!ExportProps(node))
			bResult = false;

		// go through Childs and parse them
		//tvecCategoryIter IterEnd = m_vecChildren.end();
		//for (tvecCategoryIter Iter = m_vecChildren.begin(); Iter!=IterEnd; ++Iter)
		//{
		//	//CategoryHandle nHandle(*Iter);
		//	ICategory *pCat = (*Iter);
		//	if (pCat)
		//	{
		//		XmlNodeRef ChildNode = node->newChild("Export");
		//		bool bChildResult = pCat->Serialize(ChildNode, false);

		//		if (bChildResult)
		//		{
		//			//node->addChild(ChildNode);
		//		}
		//		else
		//		{
		//			node->removeChild(ChildNode);
		//			bResult = false;
		//		}
		//	} // done with all children
		//}

		// export to node
	}
	(void)bResult;
	return true;
}

bool CCategory::RefreshCategories(const bool bAddNew)
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	//if (m_pPlatformCategory)
	{
		FMOD::EventCategory *pCat = NULL;

		int i = 0;
		
		if (m_pPlatformCategory)
			m_ExResult = m_pPlatformCategory->getCategoryByIndex(i, &pCat);

		while (pCat)
		{
			int nIndex = 0;
			char *sName = 0; 

			m_ExResult = pCat->getInfo( &nIndex,&sName );
			if (m_ExResult == FMOD_OK)
			{
				ICategory *pChild = GetCategory(sName);
				if (!pChild && bAddNew)
				{
					pChild = AddCategory(sName);
				}

				if (pChild)
				{
					pChild->SetParentCategory(this);
					pChild->SetPlatformCategory(pCat);
					pChild->RefreshCategories(bAddNew);
				}
			}
			else
			{
				int a = 0; // very bad
				(void)a;
			}

			++i;
			// get next category
			pCat = NULL;
			m_ExResult = m_pPlatformCategory->getCategoryByIndex(i, &pCat);

		}
		return true;
	}
#endif

	return false;
}

bool CCategory::ApplyCategory(float fFade)
{

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	if (m_pPlatformCategory)
	{
		float fNewValue = 0.0f;
		float fOldValue = 0.0f;
		ptParamF32 fTemp(fNewValue);

		if (m_nAttributes & FLAG_SOUNDMOOD_VOLUME)
		{
			fNewValue = m_fVolume;
			m_pPlatformCategory->getVolume(&fOldValue);
			
			if (m_fMaxCategoryVolume > 0.0f)
				fOldValue = fOldValue/m_fMaxCategoryVolume;

			fNewValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade) * m_fMaxCategoryVolume;

			m_pPlatformCategory->setVolume(fNewValue);
		}

		if (m_nAttributes & FLAG_SOUNDMOOD_PITCH)
		{
			fNewValue = m_fPitch;
			m_ExResult = m_pPlatformCategory->getPitch(&fOldValue);
			m_ExResult = m_pPlatformCategory->setPitch(fOldValue*(1.0f-fFade) + fNewValue*fFade);
		}

		if (m_nAttributes & FLAG_SOUNDMOOD_LOWPASS)
		{
			// See if others have it
			if (!m_pDSPLowPass)
				GetDSPFromAll(FLAG_SOUNDMOOD_LOWPASS);

			// create DSP
			if (!m_pDSPLowPass && (m_nAttributes & FLAG_SOUNDMOOD_LOWPASS))
			{
				FMOD::System		*pFMODSystem = NULL;

				if (((CSoundSystem*)gEnv->pSoundSystem)->g_nSoundMoodsDSP)
					pFMODSystem = (FMOD::System*) gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary();

				if (pFMODSystem && (m_nAttributes & FLAG_SOUNDMOOD_LOWPASS))
				{
					m_ExResult = pFMODSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS_SIMPLE, &m_pDSPLowPass);
					if (m_ExResult == FMOD_OK)
					{
						FMOD::ChannelGroup* pCGroup;
						m_ExResult = m_pPlatformCategory->getChannelGroup(&pCGroup);
						if (m_ExResult == FMOD_OK)
						{
							m_ExResult = pCGroup->addDSP(m_pDSPLowPass, 0);
						}
					}
					SetDSPForAll(FLAG_SOUNDMOOD_LOWPASS, m_pDSPLowPass);
				}
			}

			fNewValue = 0;

			if (m_pDSPLowPass)
			{
				// Cutoff
				fNewValue = m_fLowPassCutoff;

				m_ExResult = m_pDSPLowPass->getParameter(FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPLowPass->setParameter(FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, fOldValue);

				if (fOldValue == 22000.0f)
					m_ExResult = m_pDSPLowPass->setBypass(true);
				else
					m_ExResult = m_pDSPLowPass->setBypass(false);
			}
		}


		if (m_nAttributes & FLAG_SOUNDMOOD_HIGHPASS)
		{
			// See if others have it
			if (!m_pDSPHighPass)
				GetDSPFromAll(FLAG_SOUNDMOOD_HIGHPASS);

			// create DSP
			if (!m_pDSPHighPass && (m_nAttributes & FLAG_SOUNDMOOD_HIGHPASS))
			{
				FMOD::System		*pFMODSystem = NULL;

				if (((CSoundSystem*)gEnv->pSoundSystem)->g_nSoundMoodsDSP)
					pFMODSystem = (FMOD::System*) gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary();

				if (pFMODSystem && (m_nAttributes & FLAG_SOUNDMOOD_HIGHPASS))
				{
					m_ExResult = pFMODSystem->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &m_pDSPHighPass);
					if (m_ExResult == FMOD_OK)
					{
						FMOD::ChannelGroup* pCGroup;
						m_ExResult = m_pPlatformCategory->getChannelGroup(&pCGroup);
						if (m_ExResult == FMOD_OK)
						{
							m_ExResult = pCGroup->addDSP(m_pDSPHighPass, 0);
						}
					}
					SetDSPForAll(FLAG_SOUNDMOOD_HIGHPASS, m_pDSPHighPass);
				}
			}

			fNewValue = 0;

			if (m_pDSPHighPass)
			{
				// Resonance
				fNewValue = m_fHighPassResonance;

				m_ExResult = m_pDSPHighPass->getParameter(FMOD_DSP_HIGHPASS_RESONANCE, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPHighPass->setParameter(FMOD_DSP_HIGHPASS_RESONANCE, fOldValue);				
				
				// Cutoff
				fNewValue = m_fHighPassCutoff;

				m_ExResult = m_pDSPHighPass->getParameter(FMOD_DSP_HIGHPASS_CUTOFF, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPHighPass->setParameter(FMOD_DSP_HIGHPASS_CUTOFF, fOldValue);

				if (fOldValue == 10.0f)
					m_ExResult = m_pDSPHighPass->setBypass(true);
				else
					m_ExResult = m_pDSPHighPass->setBypass(false);

			}
		}

		// Add ParamEQs to DSP Chains before adding Compressor
		if (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1)
		{
			// See if others have it
			if (!m_pDSPParamEQ1)
				GetDSPFromAll(FLAG_SOUNDMOOD_PARAMEQ1);

			// create DSP
			if (!m_pDSPParamEQ1 && (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1))
			{
				FMOD::System		*pFMODSystem = NULL;

				if (((CSoundSystem*)gEnv->pSoundSystem)->g_nSoundMoodsDSP)
					pFMODSystem = (FMOD::System*) gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary();

				if (pFMODSystem && (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1))
				{
					m_ExResult = pFMODSystem->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &m_pDSPParamEQ1);
					if (m_ExResult == FMOD_OK)
					{
						FMOD::ChannelGroup* pCGroup;
						m_ExResult = m_pPlatformCategory->getChannelGroup(&pCGroup);
						if (m_ExResult == FMOD_OK)
						{
							m_ExResult = pCGroup->addDSP(m_pDSPParamEQ1, 0);
						}
					}
					SetDSPForAll(FLAG_SOUNDMOOD_PARAMEQ1, m_pDSPParamEQ1);
				}
			}

			fNewValue = 0;

			if (m_pDSPParamEQ1)
			{
				// Center
				fNewValue = m_fParamEQ1Center;

				m_ExResult = m_pDSPParamEQ1->getParameter(FMOD_DSP_PARAMEQ_CENTER, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPParamEQ1->setParameter(FMOD_DSP_PARAMEQ_CENTER, fOldValue);

				// Release
				fNewValue = m_fParamEQ1Bandwidth;

				m_ExResult = m_pDSPParamEQ1->getParameter(FMOD_DSP_PARAMEQ_BANDWIDTH, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPParamEQ1->setParameter(FMOD_DSP_PARAMEQ_BANDWIDTH, fOldValue);

				// Gain
				fNewValue = m_fParamEQ1Gain;

				m_ExResult = m_pDSPParamEQ1->getParameter(FMOD_DSP_PARAMEQ_GAIN, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPParamEQ1->setParameter(FMOD_DSP_PARAMEQ_GAIN, fOldValue);

				if (fOldValue == 1.0f)
					m_ExResult = m_pDSPParamEQ1->setBypass(true);
				else
					m_ExResult = m_pDSPParamEQ1->setBypass(false);

			}
		}

		if (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2)
		{
			// See if others have it
			if (!m_pDSPParamEQ2)
				GetDSPFromAll(FLAG_SOUNDMOOD_PARAMEQ2);

			// create DSP
			if (!m_pDSPParamEQ2 && (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2))
			{
				FMOD::System		*pFMODSystem = NULL;

				if (((CSoundSystem*)gEnv->pSoundSystem)->g_nSoundMoodsDSP)
					pFMODSystem = (FMOD::System*) gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary();

				if (pFMODSystem && (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2))
				{
					m_ExResult = pFMODSystem->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &m_pDSPParamEQ2);
					if (m_ExResult == FMOD_OK)
					{
						FMOD::ChannelGroup* pCGroup;
						m_ExResult = m_pPlatformCategory->getChannelGroup(&pCGroup);
						if (m_ExResult == FMOD_OK)
						{
							m_ExResult = pCGroup->addDSP(m_pDSPParamEQ2, 0);
						}
					}
					SetDSPForAll(FLAG_SOUNDMOOD_PARAMEQ2, m_pDSPParamEQ2);
				}
			}

			fNewValue = 0;

			if (m_pDSPParamEQ2)
			{
				// Center
				fNewValue = m_fParamEQ2Center;

				m_ExResult = m_pDSPParamEQ2->getParameter(FMOD_DSP_PARAMEQ_CENTER, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPParamEQ2->setParameter(FMOD_DSP_PARAMEQ_CENTER, fOldValue);

				// Release
				fNewValue = m_fParamEQ2Bandwidth;

				m_ExResult = m_pDSPParamEQ2->getParameter(FMOD_DSP_PARAMEQ_BANDWIDTH, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPParamEQ2->setParameter(FMOD_DSP_PARAMEQ_BANDWIDTH, fOldValue);

				// Gain
				fNewValue = m_fParamEQ2Gain;

				m_ExResult = m_pDSPParamEQ2->getParameter(FMOD_DSP_PARAMEQ_GAIN, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPParamEQ2->setParameter(FMOD_DSP_PARAMEQ_GAIN, fOldValue);

				if (fOldValue == 1.0f)
					m_ExResult = m_pDSPParamEQ2->setBypass(true);
				else
					m_ExResult = m_pDSPParamEQ2->setBypass(false);

			}
		}

		if (m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR)
		{
			// See if others have it
			if (!m_pDSPCompressor)
				GetDSPFromAll(FLAG_SOUNDMOOD_COMPRESSOR);

			// create DSP
			if (!m_pDSPCompressor && (m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR))
			{
				FMOD::System		*pFMODSystem = NULL;

				if (((CSoundSystem*)gEnv->pSoundSystem)->g_nSoundMoodsDSP)
					pFMODSystem = (FMOD::System*) gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary();

				if (pFMODSystem && (m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR))
				{
					m_ExResult = pFMODSystem->createDSPByType(FMOD_DSP_TYPE_COMPRESSOR, &m_pDSPCompressor);
					if (m_ExResult == FMOD_OK)
					{
						FMOD::ChannelGroup* pCGroup;
						m_ExResult = m_pPlatformCategory->getChannelGroup(&pCGroup);
						if (m_ExResult == FMOD_OK)
						{
							m_ExResult = pCGroup->addDSP(m_pDSPCompressor, 0);
						}
					}
					SetDSPForAll(FLAG_SOUNDMOOD_COMPRESSOR, m_pDSPCompressor);
				}
			}

			fNewValue = 0;

			if (m_pDSPCompressor)
			{
				// Attack
				fNewValue = m_fCompressorAttack;

				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_ATTACK, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPCompressor->setParameter(FMOD_DSP_COMPRESSOR_ATTACK, fOldValue);

				// Release
				fNewValue = m_fCompressorRelease;

				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_RELEASE, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPCompressor->setParameter(FMOD_DSP_COMPRESSOR_RELEASE, fOldValue);

				// GainMakeup
				fNewValue = m_fCompressorGainMakeup;

				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_GAINMAKEUP, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPCompressor->setParameter(FMOD_DSP_COMPRESSOR_GAINMAKEUP, fOldValue);

				// Threshold
				fNewValue = m_fCompressorThreshold;

				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_THRESHOLD, &fOldValue, 0, 0);
				fOldValue = (fOldValue*(1.0f-fFade) + fNewValue*fFade);
				m_ExResult = m_pDSPCompressor->setParameter(FMOD_DSP_COMPRESSOR_THRESHOLD, fOldValue);

				if (fOldValue == 0.0f)
					m_ExResult = m_pDSPCompressor->setBypass(true);
				else
					m_ExResult = m_pDSPCompressor->setBypass(false);

			}
		}

	}
#endif

	return true;
}

// writes output to screen in debug
void CCategory::DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, bool bAppliedValues)
{
	assert (pRenderer);

	float fColor[4]			=	{1.0f, 1.0f, 1.0f, 0.7f};
	float fColorRed[4]	=	{1.0f, 0.0f, 0.0f, 0.7f};
	float *pColor = fColor;

	char sText[512];

	sprintf(sText,"%s",GetName());

	if (m_nAttributes & FLAG_SOUNDMOOD_VOLUME || bAppliedValues)
	{
		float fVolume = m_fVolume;
		size_t idLen = strlen(sText);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		if (bAppliedValues)
		{
			if (m_pPlatformCategory)
				m_ExResult = m_pPlatformCategory->getVolume(&fVolume);
			
			if (IS_FMODERROR)
			{
				FmodErrorOutput("category get volume failed! ", eSLT_Message);
			}
		}
#endif
	
		sprintf(sText+idLen,", Vol=%1.2f", fVolume);
		
		if (m_fVolume != fVolume)
			pColor = fColorRed;
	}

	if (m_nAttributes & FLAG_SOUNDMOOD_PITCH || bAppliedValues)
	{
		float fPitch = m_fPitch;
		size_t idLen = strlen(sText);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		if (bAppliedValues)
		{
			if (m_pPlatformCategory)
				m_ExResult = m_pPlatformCategory->getPitch(&fPitch);

			if (IS_FMODERROR)
			{
				FmodErrorOutput("category get pitch failed! ", eSLT_Message);
			}
		}
#endif

		sprintf(sText+idLen,", Pitch=%1.2f", fPitch);
		
		if (m_fPitch != fPitch)
			pColor = fColorRed;
	}

	// LOWPASS
	if (m_nAttributes & FLAG_SOUNDMOOD_LOWPASS || bAppliedValues)
	{
		float fLowPassCutoff = m_fLowPassCutoff;
		size_t idLen = strlen(sText);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		if (bAppliedValues)
		{
			if (m_pDSPLowPass)
				m_ExResult = m_pDSPLowPass->getParameter(FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, &fLowPassCutoff, 0, 0);

			if (IS_FMODERROR)
			{
				FmodErrorOutput("category get lowpass cutoff failed! ", eSLT_Message);
			}
		}
#endif

		sprintf(sText+idLen,", LowPass: Cutoff=%1.2f", fLowPassCutoff);

		if (m_fLowPassCutoff != fLowPassCutoff)
			pColor = fColorRed;
	}

	// HIGHPASS
	if (m_nAttributes & FLAG_SOUNDMOOD_HIGHPASS || bAppliedValues)
	{
		float fHighPassCutoff = m_fHighPassCutoff;
		float fHighPassResonance = m_fHighPassResonance;
		size_t idLen = strlen(sText);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		if (bAppliedValues)
		{
			if (m_pDSPHighPass)
				m_ExResult = m_pDSPHighPass->getParameter(FMOD_DSP_HIGHPASS_CUTOFF, &fHighPassCutoff, 0, 0);

			if (IS_FMODERROR)
			{
				FmodErrorOutput("category get highpass cutoff failed! ", eSLT_Message);
			}
			
			if (m_pDSPHighPass)
				m_ExResult = m_pDSPHighPass->getParameter(FMOD_DSP_HIGHPASS_RESONANCE, &fHighPassResonance, 0, 0);

			if (IS_FMODERROR)
			{
				FmodErrorOutput("category get highpass resonance failed! ", eSLT_Message);
			}
		}
#endif

		sprintf(sText+idLen,", HighPass: Cut=%1.2f Res.=%1.2f", fHighPassCutoff, fHighPassResonance);

		if (m_fHighPassCutoff != fHighPassCutoff)
			pColor = fColorRed;
		
		if (m_fHighPassResonance != fHighPassResonance)
			pColor = fColorRed;
	}

	// COMPRESSOR
	if (m_nAttributes & FLAG_SOUNDMOOD_COMPRESSOR || bAppliedValues)
	{
		float fCompressorThreshold = m_fCompressorThreshold;
		float fCompressorAttack = m_fCompressorAttack;
		float fCompressorRelease = m_fCompressorRelease;
		float fCompressorGainMakeup = m_fCompressorGainMakeup;
		
		size_t idLen = strlen(sText);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		if (bAppliedValues)
		{
			if (m_pDSPCompressor)
			{
				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_THRESHOLD, &fCompressorThreshold, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get compressor threshold failed! ", eSLT_Message);
				}

				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_ATTACK, &fCompressorAttack, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get compressor attack failed! ", eSLT_Message);
				}

				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_RELEASE, &fCompressorRelease, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get compressor release failed! ", eSLT_Message);
				}

				m_ExResult = m_pDSPCompressor->getParameter(FMOD_DSP_COMPRESSOR_GAINMAKEUP, &fCompressorGainMakeup, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get compressor gainmakeup failed! ", eSLT_Message);
				}
			}

		}
#endif

		sprintf(sText+idLen,", Compressor: Thres.=%1.2f Atk.=%1.2f Rel.=%1.2f GainMkp.=%1.2f", fCompressorThreshold, fCompressorAttack, fCompressorRelease, fCompressorGainMakeup);

		if (m_fCompressorThreshold != fCompressorThreshold)
			pColor = fColorRed;

		if (m_fCompressorAttack != fCompressorAttack)
			pColor = fColorRed;
		
		if (m_fCompressorRelease != fCompressorRelease)
			pColor = fColorRed;

		if (m_fCompressorGainMakeup != fCompressorGainMakeup)
			pColor = fColorRed;
	}

	// PARAMEQ1
	if (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ1 || bAppliedValues)
	{
		float fParamEQ1Center = m_fParamEQ1Center;
		float fParamEQ1Bandwidth = m_fParamEQ1Bandwidth;
		float fParamEQ1Gain = m_fParamEQ1Gain;

		size_t idLen = strlen(sText);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		if (bAppliedValues)
		{
			if (m_pDSPParamEQ1)
			{
				m_ExResult = m_pDSPParamEQ1->getParameter(FMOD_DSP_PARAMEQ_CENTER, &fParamEQ1Center, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get parameq center failed! ", eSLT_Message);
				}

				m_ExResult = m_pDSPParamEQ1->getParameter(FMOD_DSP_PARAMEQ_BANDWIDTH, &fParamEQ1Bandwidth, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get parameq bandwidth failed! ", eSLT_Message);
				}

				m_ExResult = m_pDSPParamEQ1->getParameter(FMOD_DSP_PARAMEQ_GAIN, &fParamEQ1Gain, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get parameq gain failed! ", eSLT_Message);
				}
			}

		}
#endif

		sprintf(sText+idLen,", ParamEQ1: Center=%1.2f Bandw.=%1.2f Gain=%1.2f", fParamEQ1Center, fParamEQ1Bandwidth, fParamEQ1Gain);

		if (m_fParamEQ1Center != fParamEQ1Center)
			pColor = fColorRed;

		if (m_fParamEQ1Bandwidth != fParamEQ1Bandwidth)
			pColor = fColorRed;

		if (m_fParamEQ1Gain != fParamEQ1Gain)
			pColor = fColorRed;

	}

	// PARAMEQ2
	if (m_nAttributes & FLAG_SOUNDMOOD_PARAMEQ2 || bAppliedValues)
	{
		float fParamEQ2Center = m_fParamEQ2Center;
		float fParamEQ2Bandwidth = m_fParamEQ2Bandwidth;
		float fParamEQ2Gain = m_fParamEQ2Gain;

		size_t idLen = strlen(sText);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
		if (bAppliedValues)
		{
			if (m_pDSPParamEQ2)
			{
				m_ExResult = m_pDSPParamEQ2->getParameter(FMOD_DSP_PARAMEQ_CENTER, &fParamEQ2Center, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get parameq center failed! ", eSLT_Message);
				}

				m_ExResult = m_pDSPParamEQ2->getParameter(FMOD_DSP_PARAMEQ_BANDWIDTH, &fParamEQ2Bandwidth, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get parameq bandwidth failed! ", eSLT_Message);
				}

				m_ExResult = m_pDSPParamEQ2->getParameter(FMOD_DSP_PARAMEQ_GAIN, &fParamEQ2Gain, 0, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("category get parameq gain failed! ", eSLT_Message);
				}
			}

		}
#endif

		sprintf(sText+idLen,", ParamEQ2: Center=%1.2f Bandw.=%1.2f Gain=%1.2f", fParamEQ2Center, fParamEQ2Bandwidth, fParamEQ2Gain);

		if (m_fParamEQ2Center != fParamEQ2Center)
			pColor = fColorRed;

		if (m_fParamEQ2Bandwidth != fParamEQ2Bandwidth)
			pColor = fColorRed;

		if (m_fParamEQ2Gain != fParamEQ2Gain)
			pColor = fColorRed;

	}


#if defined(SOUNDSYSTEM_USE_FMODEX400)
	if (!m_pPlatformCategory)
		pColor = fColorRed;
#endif

	pRenderer->Draw2dLabel(xpos, ypos, 1, pColor, false, "%s", sText);

	ypos += 10;
	xpos += 10;

	// go through Children
#if defined(SOUNDSYSTEM_USE_FMODEX400)
		FMOD::EventCategory *pCat = NULL;

		int i = 0;

		if (m_pPlatformCategory)
		{
			while (m_pPlatformCategory->getCategoryByIndex(i, &pCat) == FMOD_OK)
			{
				{
					ICategory *pChild = GetCategoryByPtr((void*)pCat);

					if (pChild)
						pChild->DrawInformation(pRenderer, xpos, ypos, bAppliedValues);
				}

				++i;
				pCat = NULL;
			}
		}
#endif
		xpos -= 10;
}

void* CCategory::GetDSP(const int nDSPType)
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	switch(nDSPType)
	{
	case FLAG_SOUNDMOOD_LOWPASS:
		return m_pDSPLowPass;
		break;
	case FLAG_SOUNDMOOD_HIGHPASS:
		return m_pDSPHighPass;
	    break;
	case FLAG_SOUNDMOOD_COMPRESSOR:
		return m_pDSPCompressor;
		break;
	case FLAG_SOUNDMOOD_PARAMEQ1:
		return m_pDSPParamEQ1;
		break;
	case FLAG_SOUNDMOOD_PARAMEQ2:
		return m_pDSPParamEQ2;
		break;
	default:
	    break;
	}
#endif

	return NULL;

}

void CCategory::SetDSP(const int nDSPType, void* pDSP)
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	switch(nDSPType)
	{
	case FLAG_SOUNDMOOD_LOWPASS:
		m_pDSPLowPass = (FMOD::DSP*)pDSP;
			break;
	case FLAG_SOUNDMOOD_HIGHPASS:
		m_pDSPHighPass = (FMOD::DSP*)pDSP;
			break;
	case FLAG_SOUNDMOOD_COMPRESSOR:
		m_pDSPCompressor = (FMOD::DSP*)pDSP;
		break;
	case FLAG_SOUNDMOOD_PARAMEQ1:
		m_pDSPParamEQ1 = (FMOD::DSP*)pDSP;
		break;
	case FLAG_SOUNDMOOD_PARAMEQ2:
		m_pDSPParamEQ2 = (FMOD::DSP*)pDSP;
		break;
	default:
		break;
	}
#endif

}

void CCategory::GetDSPFromAll(const int nDSPType)
{
	int nCount = 0;
	IMood* pMood = m_pMoodManager->GetMoodPtr(nCount);
	void* pDSP = NULL;
	
	while (pMood)
	{
		CCategory* pCat = (CCategory*)pMood->GetCategory(GetName());
		
		if (pCat)
			pDSP = pCat->GetDSP(nDSPType);

		if (pDSP)
		{
			SetDSP(nDSPType, pDSP);
			break;
		}

		++nCount;
		pMood = m_pMoodManager->GetMoodPtr(nCount);
	}

}

void CCategory::SetDSPForAll(const int nDSPType, void* pDSP)
{
	int nCount = 0;
	IMood* pMood = m_pMoodManager->GetMoodPtr(nCount);

	while (pMood)
	{
		CCategory* pCat = (CCategory*)pMood->GetCategory(GetName());

		if (pCat)
			pCat->SetDSP(nDSPType, pDSP);

		++nCount;
		pMood = m_pMoodManager->GetMoodPtr(nCount);
	}


}

void CCategory::GetMemoryUsage( ICrySizer *pSizer ) const
{
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_sName);	
}

//////////////////////////////////////////////////////////////////////////
// CMood Definition
//////////////////////////////////////////////////////////////////////////

CMood::CMood(CMoodManager *pMoodManager)
{
	m_pMoodManager = pMoodManager;
	m_vecCategories.clear();
	m_fPriority = 1.0f;
	m_fMusicVolume = 0.8f;
	m_bIsMixMood = false;

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	m_ExResult = FMOD_OK;
#endif
}

CMood::~CMood()
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	// a bit sub-optimal way to clean up FMOD DSPs before simply clearing out all categories 
	// because there was a crash in the Category destructor by iterating through all categories again
	FMOD::DSP* pDSP = 0;
	for (uint32 i=0; i<m_vecCategories.size(); ++i)
	{
		ICategory* pICat = m_vecCategories[i];
		CCategory* pCat = (CCategory*)pICat;

		pDSP = (FMOD::DSP*)pCat->GetDSP(FLAG_SOUNDMOOD_LOWPASS);
		
		if (pDSP)
		{
			m_ExResult = pDSP->remove();
			m_ExResult = pDSP->release();
			pCat->SetDSP(FLAG_SOUNDMOOD_LOWPASS, 0);
			pCat->SetDSPForAll(FLAG_SOUNDMOOD_LOWPASS, 0);
		}

		pDSP = (FMOD::DSP*)pCat->GetDSP(FLAG_SOUNDMOOD_HIGHPASS);

		if (pDSP)
		{
			m_ExResult = pDSP->remove();
			m_ExResult = pDSP->release();
			pCat->SetDSP(FLAG_SOUNDMOOD_HIGHPASS, 0);
			pCat->SetDSPForAll(FLAG_SOUNDMOOD_HIGHPASS, 0);
		}

		pDSP = (FMOD::DSP*)pCat->GetDSP(FLAG_SOUNDMOOD_COMPRESSOR);

		if (pDSP)
		{
			m_ExResult = pDSP->remove();
			m_ExResult = pDSP->release();
			pCat->SetDSP(FLAG_SOUNDMOOD_COMPRESSOR, 0);
			pCat->SetDSPForAll(FLAG_SOUNDMOOD_COMPRESSOR, 0);
		}

		pDSP = (FMOD::DSP*)pCat->GetDSP(FLAG_SOUNDMOOD_PARAMEQ1);

		if (pDSP)
		{
			m_ExResult = pDSP->remove();
			m_ExResult = pDSP->release();
			pCat->SetDSP(FLAG_SOUNDMOOD_PARAMEQ1, 0);
			pCat->SetDSPForAll(FLAG_SOUNDMOOD_PARAMEQ1, 0);
		}

		pDSP = (FMOD::DSP*)pCat->GetDSP(FLAG_SOUNDMOOD_PARAMEQ2);

		if (pDSP)
		{
			m_ExResult = pDSP->remove();
			m_ExResult = pDSP->release();
			pCat->SetDSP(FLAG_SOUNDMOOD_PARAMEQ2, 0);
			pCat->SetDSPForAll(FLAG_SOUNDMOOD_PARAMEQ2, 0);
		}
	}
#endif//SOUNDSYSTEM_USE_FMODEX400
	m_vecCategories.clear();
}

const char* CMood::GetName() const
{ 
	return m_sMoodName.c_str();
}

ICategory* CMood::GetMasterCategory() const
{
	ICategory *pCat = GetCategory("master");

	return pCat;
}

ICategory* CMood::AddCategory(const char *sCategoryName)
{
	ICategory *pNewChild = GetCategory(sCategoryName);

	if (!pNewChild)
	{
		bool bNewEntry = true;

		// fill up holes in the array first
		uint32 i = 0;
		for (i=0; i<m_vecCategories.size(); ++i)
			if (m_vecCategories[i] == NULL)
			{
				bNewEntry = false;
				break;
			}

			pNewChild = (ICategory*) new CCategory(m_pMoodManager, NULL, this);

			if (!pNewChild)
			{
				//delete pNewChild;
				return 0;
			}

			((CCategory*)pNewChild)->SetName(sCategoryName);

			if (bNewEntry)
				m_vecCategories.push_back(pNewChild);
			else
				m_vecCategories[i] = pNewChild;

			m_pMoodManager->UpdateMemUsage(sizeof(CCategory));
	}

	return pNewChild;
}


bool CMood::RemoveCategory(const char *sCategoryName)
{
	bool bResult = false;

		uint32 i = 0;
		for (i=0; i<m_vecCategories.size(); ++i)
		{
			ICategory *pCat = m_vecCategories[i];
			if (pCat)
			{
				if (strcmp(sCategoryName, pCat->GetName()) == 0)
				{
					m_vecCategories[i] = NULL;
					bResult = true;
				}
			}
		}

		return bResult;
}

uint32 CMood::GetCategoryCount() const
{
	int nNum = m_vecCategories.size();

	return nNum;
}

ICategory*	CMood::GetCategoryByIndex(uint32 nCategoryCount) const
{
	ICategory* pElement = NULL;

	if (m_vecCategories.size() > nCategoryCount)
		pElement = m_vecCategories[nCategoryCount];

	return pElement;
}

ICategory*	CMood::GetCategory(const char *sCategoryName) const
{
	ICategory *pElement = NULL;

	tvecCategoryIterConst IterEnd = m_vecCategories.end();
	for (tvecCategoryIterConst Iter = m_vecCategories.begin(); Iter != IterEnd; ++Iter)
	{
		ICategory *pCat = (*Iter);
		if (pCat)
		{
			if (strcmp(sCategoryName, pCat->GetName()) == 0)
			{
				pElement = pCat;
				break;
			}
		}
	}

	return pElement;
}

ICategory* CMood::GetCategoryByPtr(void* pPlatformCategory) const
{
	ICategory *pElement = NULL;

	if (pPlatformCategory)
	{
		tvecCategoryIterConst ItEnd = m_vecCategories.end();
		for (tvecCategoryIterConst It = m_vecCategories.begin(); It != ItEnd; ++It)
		{
			ICategory *pCat = (*It);
			if (pCat->GetPlatformCategory() == pPlatformCategory)
			{
				pElement = pCat;
				break;
			}
		}
	}

	return pElement;

}

bool CMood::Serialize(XmlNodeRef &node, bool bLoading)
{
	bool bResult = true;

	if (bLoading)
	{
		// import from node
		if (strcmp("Mood", node->getTag()) != 0)
			return false;		// no known Tag

		node->getAttr("Priority", m_fPriority);
		node->getAttr("MusicVolume", m_fMusicVolume);

		// go through Childs and parse them
		int nGroupChildCount = node->getChildCount();
		for (int i=0 ; i < nGroupChildCount; ++i)
		{
			XmlNodeRef GroupChildNode = node->getChild(i);

			// read the name of the child
			const char *sChildName = GroupChildNode->getAttr("Name");
			ICategory *pNewChild = AddCategory(sChildName);

			if (!pNewChild)
				return false; // something wrong !

			bool bChildResult = pNewChild->Serialize(GroupChildNode, true);

			if (!bChildResult)
			{
				RemoveCategory(sChildName);
				bResult = false;
			}
		} // done with all children
	}
	else
	{

		node->setTag("Mood");
		node->setAttr("Name", GetName());
		node->setAttr("Priority", m_fPriority);
		node->setAttr("MusicVolume", m_fMusicVolume);

		// sorting Category names
		std::vector<string> CategoryNames;

		tvecCategoryIter IterEnd = m_vecCategories.end();
		for (tvecCategoryIter Iter = m_vecCategories.begin(); Iter!=IterEnd; ++Iter)
		{
			ICategory *pCat = (*Iter);
			if (pCat)
				CategoryNames.push_back(pCat->GetName());
		}

		//CategoryNames.sort();
		std::sort(CategoryNames.begin(), CategoryNames.end());

		int nSize = CategoryNames.size();
		for (int i=0; i<nSize; ++i)
		{
			ICategory *pCat = GetCategory(CategoryNames[i].c_str());
			if (pCat)
			{
				XmlNodeRef ChildNode = node->newChild("Export");
				bool bChildResult = pCat->Serialize(ChildNode, false);

				if (bChildResult)
				{
					//node->addChild(ChildNode);
				}
				else
				{
					node->removeChild(ChildNode);
					bResult = false;
				}
			} // done with all children
		}


		// export to node
	}

	return bResult;
}

bool CMood::InterpolateMoods(const IMood *pMood1, float fFade1, const IMood *pMood2, float fFade2, bool bForceDefault)
{

	// priority
	float fPriorityRatio = pMood1->GetPriority()*fFade1 + pMood2->GetPriority()*fFade2;
	float fNewFade1 = (pMood1->GetPriority() * fFade1) / fPriorityRatio;
	float fNewFade2 = 1.0f - fNewFade1;

	m_fPriority = (fPriorityRatio / 2.0f);

	int nNum1 = pMood1->GetCategoryCount();
	int nNum2 = pMood1->GetCategoryCount();
	
	assert (nNum1 == nNum2);

	int nNum3 = GetCategoryCount();

	if (nNum3 < nNum1)
	{
		// insert all categories from Mood1 without hierarchy
		for (uint32 i=0; i < pMood1->GetCategoryCount(); ++i)
		{
			ICategory* pCat1 = pMood1->GetCategoryByIndex(i);
			ICategory* pNewCat = NULL;

			if (pCat1)
			{
				pNewCat = GetCategoryByPtr( (void*)pCat1->GetPlatformCategory() );

				if (!pNewCat)
					pNewCat = AddCategory( pCat1->GetName() );

				if (pNewCat && !pNewCat->GetPlatformCategory())
					pNewCat->SetPlatformCategory(pCat1->GetPlatformCategory());
			}
		}
	}

	// interpolate MusicVolume
	float fMusic1 = pMood1->GetMusicVolume();
	float fMusic2 = pMood2->GetMusicVolume();
	float fNewMusic = fMusic1*fNewFade1 + fMusic2*fNewFade2;
	m_fMusicVolume = fNewMusic;
	
	// check if both Moods have the same num of categories
	assert (pMood1->GetCategoryCount() == pMood2->GetCategoryCount());
	if (CSound::m_pSSys && CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL && pMood1->GetCategoryCount() != pMood2->GetCategoryCount())
	{
		CSound::m_pSSys->Log(eSLT_Warning, "<Sound> incompatible SoundMoods  %s and %s\n", pMood1->GetName(), pMood2->GetName());		
	}

	nNum3 = GetCategoryCount();

	assert (nNum1 == nNum3);

	// go through all Categories
	for (uint32 i=0; i < GetCategoryCount(); ++i)
	{
		ICategory* pCat = GetCategoryByIndex(i);
		ICategory* pCat1 = pMood1->GetCategoryByIndex(i);
		ICategory* pCat2 = pMood2->GetCategoryByIndex(i);
		pCat->InterpolateCategories(pCat1, fNewFade1, pCat2, fNewFade2, bForceDefault);
	}

	return true;
}

bool CMood::ApplyMood(float fFade)
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD::EventSystem		*pEventSystem = (FMOD::EventSystem*) gEnv->pSoundSystem->GetIAudioDevice()->GetEventSystem();

	if (!pEventSystem)
		return false;

	for (uint32 i=0; i < GetCategoryCount(); ++i)
	{
		ICategory* pCat = GetCategoryByIndex(i);

		if (pCat)
			pCat->ApplyCategory(fFade);
		else
		{
			int verybad = 1;	(void)verybad;
		}
	}

#endif

	gEnv->pSoundSystem->GetInterfaceExtended()->SetMusicEffectsVolume(m_fMusicVolume);

	return true;
}

// writes output to screen in debug
void CMood::DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, bool bAppliedValues)
{
	assert (pRenderer);

	float fColor[4] = {1.0f, 1.0f, 1.0f, 0.7f};

	pRenderer->Draw2dLabel(xpos, ypos, 1, fColor, false, "<%s>", GetName());
	ypos += 10;
	xpos += 10;

	ICategory *pCategory = GetCategory("master");

	if (pCategory)
		pCategory->DrawInformation(pRenderer, xpos, ypos, bAppliedValues);

	xpos -= 10;
}

void CMood::GetMemoryUsage( ICrySizer *pSizer ) const
{
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_sMoodName);
	pSizer->AddObject(m_vecCategories);
	pSizer->AddObject(m_vecCategories);
}
//////////////////////////////////////////////////////////////////////////
// CMoodManager Definition
//////////////////////////////////////////////////////////////////////////

CMoodManager::CMoodManager()
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	m_ExResult = FMOD_OK;
#endif

	m_nTotalSoundsListed			= 0;
	m_nTotalMemUsedByManager	= sizeof(CMoodManager);

	m_tLastUpdate = gEnv->pTimer->GetFrameStartTime();

	m_MoodArray.clear();
	m_pFinalMix = NULL;
	//m_mapHandleNames.clear();

	Load(SOUNDMOOD_FILENAME);
	//RefreshCategories();
	m_bNeedUpdate = true;
	m_bForceFinalMix = false;
	
	// make sure to create the "default" mood
	Reset();
}

CMoodManager::~CMoodManager()
{
	//TODO free all that memory
	m_MoodArray.clear();
	//m_mapHandleNames.clear();
}

bool CMoodManager::RefreshCategories(IMood *pMood)
{
	if (!pMood)
		return false;

	// make sure we have a "default" mood anytime wihch is not the Mix-Mood
	IMood *pDefMood = AddMood("default");
	if (pDefMood == m_pFinalMix)
	{
		// rename Mix-Mood
		if (m_pFinalMix)
			m_pFinalMix->SetName("Mix-Mood");

		// try again
		pDefMood = AddMood("default");
		RefreshCategories(pDefMood);
	}

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD::EventSystem		*pEventSystem = (FMOD::EventSystem*) gEnv->pSoundSystem->GetIAudioDevice()->GetEventSystem();

	// reset all Categories of that mood
	int nNum = pMood->GetCategoryCount();
	for (int i=0; i < nNum; ++i)
	{
		ICategory* pCat = pMood->GetCategoryByIndex(i);
		if (pCat)
			pCat->SetPlatformCategory(NULL);
	}	
	
	if (!pEventSystem)
		return false;
	
	FMOD::EventCategory *pPlatformCategory;
	m_ExResult = pEventSystem->getCategory("master", &pPlatformCategory);

	ICategory *pCategory = pMood->AddCategory("master");
	pCategory->SetPlatformCategory(pPlatformCategory?pPlatformCategory:NULL);
	pCategory->RefreshCategories(true);

#endif

	return true;
}

bool CMoodManager::RefreshPlatformCategories()
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD::EventSystem		*pEventSystem = (FMOD::EventSystem*) gEnv->pSoundSystem->GetIAudioDevice()->GetEventSystem();

	if (!pEventSystem)
		return false;

	FMOD::EventCategory *pPlatformCategory;

	// update platform category handle
	tvecActiveMoodsIter IterEnd = m_ActiveMoods.end();
	for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
	{
		IMood *pMood = GetMoodPtr((*It).sFixMoodName.c_str());
		if (pMood)
		{
			{
				ICategory *pCategory = pMood->GetMasterCategory();
				if (pCategory)
				{
					m_ExResult = pEventSystem->getCategory(pCategory->GetName(), &pPlatformCategory);

					if (m_ExResult != FMOD_OK)
						break;

					pCategory->SetPlatformCategory(pPlatformCategory);
					pCategory->RefreshCategories(false);
				}
				else
				{
					int a = 0;
					(void)a;
				}
			}
		}
	}
#endif

	return true;
}

void CMoodManager::Release()
{
	delete this;
}

void CMoodManager::Reset()
{
	if (!m_pFinalMix)
	{
		m_pFinalMix = AddMood("final");
		m_pFinalMix->SetIsMixMood(true);
	}

	assert (m_pFinalMix);

	// set the mixing mood back to default values
	m_pFinalMix->SetName("final");
	IMood* pDefault = GetMoodPtr("default");
	
	if (pDefault)
	{
		m_pFinalMix->InterpolateMoods(pDefault, 1.0f, pDefault, 1.0f, true);
		m_pFinalMix->ApplyMood(1.0f);
		m_pFinalMix->InterpolateMoods(pDefault, 1.0f, pDefault, 1.0f, false);
	}
	
	// remove all active moods
	m_ActiveMoods.clear();

	// re-enter the default mood
	RegisterSoundMood("default");
	UpdateSoundMood("default", 1.0f, 0);
}

//////////////////////////////////////////////////////////////////////////
// Memory Usage
//////////////////////////////////////////////////////////////////////////

//memUsage

//////////////////////////////////////////////////////////////////////////
// Information
//////////////////////////////////////////////////////////////////////////
unsigned int GetNumberSettingsLoaded(void);
unsigned int GetTotalNumberMultiGroupsLoaded(void);
unsigned int GetTotalNumberGroupsLoaded(void);

// writes output to screen in debug
//void DrawInformation(IRenderer* pRenderer, float xpos, float ypos); 

//////////////////////////////////////////////////////////////////////////
// Management
//////////////////////////////////////////////////////////////////////////

IMood* CMoodManager::AddMood(const char *sMoodName)
{
	IMood *pNewMood = GetMoodPtr(sMoodName);

	// add a new Mood if there is not already one with that name
	if (!pNewMood)
	{
		bool bNewEntry = true;

		// fill up holes in the array first
		uint32 i = 0;
		for (i=0; i<m_MoodArray.size(); ++i)
		{
			if ( m_MoodArray[i] == NULL)
			{
				bNewEntry = false;
				break;
			}
		}

		pNewMood = (IMood*) new CMood(this);

		// problem with new()
		if (!pNewMood)
			return (IMood*)pNewMood;

		pNewMood->SetName(sMoodName);
		
		if (bNewEntry)
			m_MoodArray.push_back(pNewMood);
		else
			m_MoodArray[i] = pNewMood;

		UpdateMemUsage(sizeof(CCategory));
	}

	return pNewMood;
}


bool CMoodManager::RemoveMood(const char *sMoodName)
{
	bool bFound = false;

	tvecMood::const_iterator ItEnd = m_MoodArray.end();
	for (tvecMood::iterator It=m_MoodArray.begin(); It!=ItEnd; ++It)
	{
		IMood *pMood = (*It);

		if (strcmp(pMood->GetName(), sMoodName) == 0 && pMood != m_pFinalMix) // dont remove the mix-mood
		{
			// copy last element and pop back
			if (It != ItEnd-1)
			{
				(*It) = *(ItEnd-1);
			}
			m_MoodArray.pop_back();

			bFound = true;
			break;
		}
	}

	return (bFound);
}


//////////////////////////////////////////////////////////////////////////
IMood*	CMoodManager::GetMoodPtr(const uint32 nGroupCount) const
{
	IMood *pMood = NULL;

	if (nGroupCount < m_MoodArray.size())
	{
		pMood = m_MoodArray[nGroupCount];
		assert(pMood);
	}

	return pMood;
}

//////////////////////////////////////////////////////////////////////////
IMood*	CMoodManager::GetMoodPtr(const char *sMoodName) const
{
	IMood *pMood = NULL;

	int i = 0;
	pMood = GetMoodPtr(i);

	while (pMood)
	{
		if (strcmp(pMood->GetName(), sMoodName) == 0)
		{
			break;
		}
		++i;
		pMood = GetMoodPtr(i);

	}
	return pMood;
}


//////////////////////////////////////////////////////////////////////////
// Import/Export
//////////////////////////////////////////////////////////////////////////

unsigned int CMoodManager::GetNumberSoundsLoaded() const
{
	return m_nTotalSoundsListed;
}

//////////////////////////////////////////////////////////////////////////
// Real Time Manipulation
//////////////////////////////////////////////////////////////////////////

// needs to be updated regularly
bool CMoodManager::Update()
{
	CSoundSystem* pSoundSystem = (CSoundSystem*)gEnv->pSoundSystem;

	if (!pSoundSystem->g_nSoundMoods)
		return false;

	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	// update timer
	CTimeValue tTimeDif = gEnv->pTimer->GetFrameStartTime() - m_tLastUpdate;
	float fMS = abs(tTimeDif.GetMilliSeconds());

	if (fMS == 0.0f)
		return false; // cannot do anything if last update was same frame;

	// update time based fading
	tvecActiveMoodsIter IterEnd = m_ActiveMoods.end();
	for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
	{

		if ((*It).fFadeTarget != (*It).fFadeCurrent && (*It).nFadeTimeInMS>0 )
		{
			float fFadeDiff = (((*It).fFadeTarget - (*It).fFadeCurrent) * min(fMS,(float)(*It).nFadeTimeInMS)) / (float)(*It).nFadeTimeInMS;
			(*It).fFadeCurrent += fFadeDiff;
			(*It).fFadeCurrent = ((*It).fFadeCurrent > 0.00001f)?(*It).fFadeCurrent:0.0f;
			fFadeDiff = (fFadeDiff > 0.00001f)?fFadeDiff:0.0f;

			(*It).fFadeCurrent = max(0.0f, (*It).fFadeCurrent);
			(*It).fFadeCurrent = min(1.0f, (*It).fFadeCurrent);
			(*It).nFadeTimeInMS -= (int)fMS;
			(*It).nFadeTimeInMS = max(0, (*It).nFadeTimeInMS);
			m_bNeedUpdate = true;

			if ((*It).fFadeCurrent==0 && (*It).bUnregisterOnFadeOut)
			{
				(*It).bReadyToRemove = true;
				m_ToBeRemoved.push_back(It);		
			}
		}			
		else
		{
			if (((*It).fFadeCurrent==0 || (*It).fFadeTarget==0) && (*It).bUnregisterOnFadeOut)
				m_ToBeRemoved.push_back(It);		

		}
	}

	if (!m_bNeedUpdate)
	{
		m_tLastUpdate = gEnv->pTimer->GetFrameStartTime();
		return false;
	}

	if (!m_pFinalMix)
	{
		m_pFinalMix = AddMood("final");
		m_pFinalMix->SetIsMixMood(true);
	}
	assert (m_pFinalMix);

	m_bNeedUpdate = false;

	float SumOfWeight = 0;

	// if there is only one Mood just interpolate directly with its weight
	if (m_ActiveMoods.size()==1)
	{
		tvecActiveMoodsIter It=m_ActiveMoods.begin();

		IMood* pSingleMood = GetMoodPtr((*It).sFixMoodName.c_str());

		if (pSingleMood)
		{
			m_pFinalMix->InterpolateMoods(pSingleMood, 1.0f, pSingleMood, 1.0f, false);

			m_pFinalMix->SetName((*It).sFixMoodName.c_str());
		}

		SumOfWeight = (*It).fFadeTarget;
		(void)SumOfWeight;
	}
	else
	{
		float fMixFade = 0.0f;
		int i = 0;

		tvecActiveMoodsIter ItEnd = m_ActiveMoods.end();
		for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=ItEnd; ++It)
		{
			// Copy first element to the temp preset
			if (It==m_ActiveMoods.begin())
			{
				IMood* pFirstMood = GetMoodPtr((*It).sFixMoodName.c_str());
				fMixFade = (*It).fFadeTarget;

				if (pFirstMood)
				{
					m_pFinalMix->InterpolateMoods(pFirstMood, 1.0f, pFirstMood, 1.0f, false);

					m_pFinalMix->SetName((*It).sFixMoodName.c_str());
				}

				++i;
			}
			else // start interpolating at the 2nd element
			{
				IMood* pNextMood = GetMoodPtr((*It).sFixMoodName.c_str());

				if (!pNextMood)
					continue;

				TFixedSoundMoodName sResultMood;
				sResultMood = m_pFinalMix->GetName();
				sResultMood += "-";
				sResultMood += pNextMood->GetName();
				m_pFinalMix->SetName(sResultMood.c_str());

				if (i==2)
				{
					int j = 0;	(void)j;
				}
				++i;

				if ((*It).bReadyToRemove)
					m_bForceFinalMix = true;
				else
					m_bForceFinalMix = false;
				

				m_pFinalMix->InterpolateMoods(m_pFinalMix, fMixFade, pNextMood, (*It).fFadeCurrent, m_bForceFinalMix);

			}
		}
	}



	assert (m_pFinalMix);

	m_pFinalMix->ApplyMood(1.0f);

	// Unregister Moods with current fade == 0;
	tvecRemoveMoodIterIterConstReverse IEnd = m_ToBeRemoved.rend();
	for (tvecRemoveMoodIterIterConstReverse rIt=m_ToBeRemoved.rbegin(); rIt!=IEnd; ++rIt)
	{
		m_ActiveMoods.erase((*rIt));
		m_bNeedUpdate = true;
	}

	m_ToBeRemoved.resize(0);

#ifdef SOUNDSYSTEM_USE_FMODEX400

#endif

	// update timer
	m_tLastUpdate = gEnv->pTimer->GetFrameStartTime();

	return true;
}

bool CMoodManager::InterpolateMoods(const IMood* pMood1, const float fFade1, const IMood* pMood2, const float fFade2, IMood* pResultMood, float fResultFade)
{
	TFixedSoundMoodName sResultMood; 
	sResultMood = pMood1->GetName();
	sResultMood += pMood2->GetName();

	if (!m_pFinalMix)
	{
		m_pFinalMix = AddMood(sResultMood.c_str());
	}
	else
	{
		m_pFinalMix->SetName(sResultMood.c_str());
	}

	if (m_pFinalMix)
		m_pFinalMix->InterpolateMoods(pMood1, fFade1, pMood2, fFade2, false);

	return true;
}

// registers a Mood to be actively processes (starts with 0 fade value)
bool CMoodManager::RegisterSoundMood(const char *sMoodName)
{	
	CSoundSystem* pSoundSystem = (CSoundSystem*)gEnv->pSoundSystem;

	if (!pSoundSystem->g_nSoundMoods)
		return false;

	assert( sMoodName[0] );
	if (sMoodName[0])
	{
		// verify if this Mood is available in the library
		IMood *pMood = GetMoodPtr(sMoodName);
		
		if (!pMood)
			return false;


		// add this Preset to the vector of active Presets or update it if its already in there
		tvecActiveMoodsIterConst IterEnd = m_ActiveMoods.end();
		for (tvecActiveMoodsIterConst It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
		{
			if ((*It).sFixMoodName == sMoodName)
			{
				// cant register a 2nd time
				return false;
				break;
			}
		}

		SSoundMoodProp MP;
		MP.sFixMoodName	= sMoodName;
		MP.fFadeCurrent = 0.0f;
		MP.fFadeTarget  = 0.0f;
		MP.nFadeTimeInMS = 0;
		MP.bUnregisterOnFadeOut = false;
		MP.bForceUpdate = false;
		MP.bReadyToRemove = false;

		m_ActiveMoods.push_back(MP);
		m_bNeedUpdate = true;

		return true;
	}

	return false;
}


// updates the fade value of a registered mood
bool CMoodManager::UpdateSoundMood(const char *sMoodName, float fFade, uint32 nFadeTimeInMS, bool bUnregistedOnFadeOut)
{
	assert( sMoodName[0] );

	// update this Mood to the vector of active Moodss or add if its not in there yet
	bool bMoodFound = false;

	tvecActiveMoodsIter IterEnd = m_ActiveMoods.end();
	for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
	{
		if ((*It).sFixMoodName == sMoodName)
		{
			if (fFade < 0)
			{
				bool bStillUpdating = ((*It).fFadeTarget != (*It).fFadeCurrent);
 				return bStillUpdating;
			}

			assert(fFade >= 0 && fFade <= 1);

			if (nFadeTimeInMS == 0)
				(*It).fFadeCurrent = fFade;
			
			float fTargetDiff = abs(fFade - (*It).fFadeCurrent);
			(*It).nFadeTimeInMS = (int) (nFadeTimeInMS * fTargetDiff); // reduce new FadeTime depending on current Fade value
			(*It).fFadeTarget = fFade;
			(*It).bUnregisterOnFadeOut = bUnregistedOnFadeOut;

			bMoodFound = true;
			break;
		}
	}

	if (!bMoodFound)
		return false;

	m_bNeedUpdate = true;
	return true;
}

// get current fade value of a registered mood
float	CMoodManager::GetSoundMoodFade(const char *sMoodName)
{
	tvecActiveMoodsIter IterEnd = m_ActiveMoods.end();
	for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
	{
		if ((*It).sFixMoodName == sMoodName)
		{
				return (*It).fFadeCurrent;
		}
	}
	return -1;

}

// unregisters a Mood and removes it from the active ones
bool CMoodManager::UnregisterSoundMood(const char *sMoodName)
{
	assert( sMoodName[0] );
	bool bMoodFound = false;

	tvecActiveMoodsIter IterEnd = m_ActiveMoods.end();
	for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
	{
		if ((*It).sFixMoodName == sMoodName)
		{
			// set it to 0 to disable all set effects first
			(*It).fFadeCurrent = -0.1f;
			(*It).fFadeTarget = 0.0f;
			(*It).nFadeTimeInMS = 1; // must be positive to trigger correct removal
			(*It).bUnregisterOnFadeOut = true;
			m_bNeedUpdate = true;
			Update();
			bMoodFound = true;
			break;
		}
	}

	if (!bMoodFound)
		return false;

	m_bNeedUpdate = true;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Sound Management
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Group Management
//////////////////////////////////////////////////////////////////////////


// writes output to screen in debug
void CMoodManager::DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, int nSoundInfo)
{
	assert (pRenderer);

	float fColor[4] = {1.0f, 1.0f, 1.0f, 0.7f};

	pRenderer->Draw2dLabel(xpos, ypos, 1, fColor, false, "Sound Moods: %d", m_ActiveMoods.size());
	ypos += 10;

	int nActiveMoodsNum = m_ActiveMoods.size();

	tvecActiveMoodsIter IterEnd = m_ActiveMoods.end();
	for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
	{
		float fActiveColor[4] = {0.2f, (*It).fFadeCurrent, 0.2f, 0.7f};
		IMood * pMood = GetMoodPtr((*It).sFixMoodName.c_str());

		if (pMood)
		{
			pRenderer->Draw2dLabel(xpos, ypos, 1, fActiveColor, false, "%s : Fade %f, Time %d, Cats %d", (*It).sFixMoodName.c_str(), (*It).fFadeCurrent, (*It).nFadeTimeInMS, pMood->GetCategoryCount());
			ypos += 10;
		}

	}

	pRenderer->Draw2dLabel(xpos, ypos, 1, fColor, false, "Sound Moods Mix: %d", m_ActiveMoods.size());
	ypos += 10;

	if (nSoundInfo == 9)
	{
		if (m_pFinalMix)
			m_pFinalMix->DrawInformation(pRenderer, xpos, ypos, false);
	}

	if (nSoundInfo == 10)
	{
		if (m_pFinalMix)
		{
			m_pFinalMix->DrawInformation(pRenderer, xpos, ypos, true);
		}

	}


}

//// Imports a group definition stored in an xml-file
//bool CMoodManager::ImportXMLFile(const string sFileName)
//{
//	CategoryHandle nHandleFailed;
//
//	if (sFileName.empty())
//		return false;
//
//	if (GetMoodPtr(sFileName))
//		return false;
//
//	int32 nPos = sFileName.rfind('/');
//	string sSingleGroupName = sFileName.substr(nPos+1);
//	string sFullFileName = sFileName + "/" + sSingleGroupName + ".xml";
//
//	XmlNodeRef root = GetISystem()->LoadXmlFile( sFullFileName.c_str());
//
//	// couldn't load or find file
//	if (!root)
//		return false; 
//
//	string sTag = root->getTag();
//	// quick test for correct tag
//	//if (root->getTag() != "SoundGroup")
//	if (sTag != "SoundGroup")
//		return false;
//
//	//PathUtil::r
//	IMood *pMood = AddMood(sFileName);
//	//ICategory *pNewGroup = GetElementPtr(nGroupHandle);
//
//	//if (pNewGroup)
//	//{
//	//	pNewGroup->Serialize(root, true);
//	//}
//	//else
//	//{
//	//	RemoveGroup(nGroupHandle);
//	//	return nHandleFailed;
//	//}
//
//	return true;
//}


bool CMoodManager::Serialize(XmlNodeRef &node, bool bLoading)
{
	bool bResult = true;

	if (bLoading)
	{
		// couldn't load or find file
		if (!node)
			return false; 

		// quick test for correct tag
		if (strcmp("SoundMoods", node->getTag()) != 0)
			return false;

		// go through Childs and parse them
		int nGroupChildCount = node->getChildCount();
		for (int i=0 ; i < nGroupChildCount; ++i)
		{
			XmlNodeRef GroupChildNode = node->getChild(i);

			// read the name of the child
			const char *sChildName = GroupChildNode->getAttr("Name");
			IMood *pNewMood = AddMood(sChildName);

			if (!pNewMood)
				return false; // something wrong !

			bool bChildResult = pNewMood->Serialize(GroupChildNode, true);

			if (!bChildResult)
			{
				RemoveMood(sChildName);
				bResult = false;
			}
		} // done with all children
	}
	else // saving
	{
		// setup a new root
		//XmlNodeRef root = GetISystem()->CreateXmlNode("SoundMood");
		
		node->setTag("SoundMoods");
		//node->setAttr("Name", GetName());

		int i = 0;
		IMood *pMood = GetMoodPtr(i);
		while (pMood)
		{
			if (!pMood->GetIsMixMood())
			{
				XmlNodeRef ChildNode = node->newChild("Export");

				bResult = pMood->Serialize(ChildNode, false);

				if (!bResult)
					node->removeChild(ChildNode);
			}

			++i;
			pMood = GetMoodPtr(i);
		}
	}

	m_bNeedUpdate = true;
	return bResult;

}

bool CMoodManager::Load(const char* sFilename)
{
	XmlNodeRef root = GetISystem()->LoadXmlFile( sFilename );
	Serialize(root, true);

	return true;
}

bool CMoodManager::RefreshCategories()
{
	IMood *pMood = NULL;
	bool bResult = false;

	for (uint32 i=0; i<m_MoodArray.size(); ++i)
	{
		if (m_MoodArray[i] != NULL)
		{
			pMood = m_MoodArray[i];
			bResult |= !RefreshCategories(pMood);
		}
	}
	return bResult;
}

void CMoodManager::SerializeState( TSerialize ser )
{
	ser.Value("NeedUpdate",m_bNeedUpdate);
	ser.Value("ForceFinalMix",m_bForceFinalMix);

	if (ser.IsWriting())
	{
		// active moods
		int nActiveMoodsNum = m_ActiveMoods.size();
		ser.Value("MoodCount",nActiveMoodsNum);

		tvecActiveMoodsIter IterEnd = m_ActiveMoods.end();
		for (tvecActiveMoodsIter It=m_ActiveMoods.begin(); It!=IterEnd; ++It)
		{
			ser.BeginGroup("ActiveMood");

			ser.Value("MoodName",(*It).sFixMoodName.c_str());
			ser.Value("ForceUpdate",(*It).bForceUpdate);
			ser.Value("Unregister",(*It).bUnregisterOnFadeOut);
			ser.Value("FadeCurrent",(*It).fFadeCurrent);
			ser.Value("FadeTarget",(*It).fFadeTarget);
			ser.Value("FadeTime",(*It).nFadeTimeInMS);
			ser.Value("ReadyToRemove",(*It).bReadyToRemove);	
					
			ser.EndGroup();
		}
	}
	else
	{	
		string sTempName;
		m_ActiveMoods.clear();
		int nActiveMoodsNum = 0;
		ser.Value("MoodCount",nActiveMoodsNum);

		for (int i=0; i<nActiveMoodsNum; ++i)
		{
			ser.BeginGroup("ActiveMood");

			ser.Value("MoodName",sTempName);

			if (GetMoodPtr(sTempName)) // mood exist
			{
				SSoundMoodProp MP;
				MP.sFixMoodName.assign(sTempName);
				ser.Value("ForceUpdate",MP.bForceUpdate);
				ser.Value("Unregister",MP.bUnregisterOnFadeOut);
				ser.Value("FadeCurrent",MP.fFadeCurrent);
				ser.Value("FadeTarget",MP.fFadeTarget);
				ser.Value("FadeTime",MP.nFadeTimeInMS);			
				ser.Value("ReadyToRemove",MP.bReadyToRemove);			
				m_ActiveMoods.push_back(MP);
			}
			ser.EndGroup();
		}

		m_bNeedUpdate = true;
		m_tLastUpdate = gEnv->pTimer->GetFrameStartTime();

	}

}

void CMoodManager::GetMemoryUsage( ICrySizer *pSizer ) const
{
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_MoodArray);	
	pSizer->AddObject(m_ActiveMoods);		
}
// takes in string = "group name : sound name" we make 2 strings from one
//bool CMoodManager::ParseGroupString(string sGroupAndSoundName, string &sGroupName, string &sSoundName)
//{
//	int32 nPos = 0;
//	nPos = sGroupAndSoundName.rfind(':');
//
//	if (nPos != -1)
//		sSoundName = sGroupAndSoundName.substr(nPos+1);
//
//	sGroupName = sGroupAndSoundName.substr(0, nPos);
//
//	return false;
//}

#include UNIQUE_VIRTUAL_WRAPPER(IMood)
#include UNIQUE_VIRTUAL_WRAPPER(ICategory)
