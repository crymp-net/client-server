/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:
	Sound moods (from game side)

-------------------------------------------------------------------------
History:
- 31:07:2007: Created by Julien Darre

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySoundSystem/ISound.h"
#include "CryCommon/CrySoundSystem/ISoundMoodManager.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "SoundMoods.h"

//-----------------------------------------------------------------------------------------------------

CSoundMoods::CSoundMoods()
{
	m_pSoundMoodManager = NULL;

	// Note: there is no sound system in a dedicated server
	if (gEnv->pSoundSystem)
	{
		m_pSoundMoodManager = gEnv->pSoundSystem->GetIMoodManager();
		assert(m_pSoundMoodManager);
	}

	m_vecSoundMoods.resize(32);
}

//-----------------------------------------------------------------------------------------------------

CSoundMoods::~CSoundMoods()
{
}

//-----------------------------------------------------------------------------------------------------

void CSoundMoods::AddSoundMood(std::string_view szSoundMood, uint32_t uiFadeIn, float fDuration, uint32_t uiFadeOut, float fFade)
{
	if (!m_pSoundMoodManager)
		return;

	SSoundMood* pSoundMood = nullptr;

	// First pass: we don't want to play the same sound mood twice
	for (auto& iterSoundMood : m_vecSoundMoods)
	{
		if (iterSoundMood.strSoundMood == szSoundMood)
		{
			pSoundMood = &iterSoundMood;
			break;
		}
	}

	if (!pSoundMood)
	{
		// Second pass: create it if not already in our vector
		for (auto& iterSoundMood : m_vecSoundMoods)
		{
			if (!iterSoundMood.bValid)
			{
				pSoundMood = &iterSoundMood;
				break;
			}
		}
	}

	if (pSoundMood)
	{
		if (pSoundMood->bValid)
		{
			m_pSoundMoodManager->UpdateSoundMood(szSoundMood.data(), 0.0f, 0);
		}

		pSoundMood->strSoundMood = szSoundMood;
		pSoundMood->uiFadeOutTime = static_cast<uint32_t>(gEnv->pTimer->GetCurrTime() + (uiFadeIn + fDuration) / 1000.0f);
		pSoundMood->uiFadeOut = uiFadeOut;
		pSoundMood->bValid = true;
		pSoundMood->bUnlimited = fDuration == -1.0f;
		m_pSoundMoodManager->RegisterSoundMood(szSoundMood.data());
		m_pSoundMoodManager->UpdateSoundMood(szSoundMood.data(), fFade, uiFadeIn);
	}
}

//-----------------------------------------------------------------------------------------------------

void CSoundMoods::AddSoundMood(ESOUNDMOOD eSoundMood,float fPercent)
{
	if(!m_pSoundMoodManager)
		return;

	switch(eSoundMood)
	{
	case SOUNDMOOD_ENTER_BINOCULARS:
		AddSoundMood("cryvision",500,-1.0f,0,1.0f);
		break;
	case SOUNDMOOD_LEAVE_BINOCULARS:
		RemoveSoundMood("cryvision",0.0f,500);
		break;
	case SOUNDMOOD_ENTER_CLOAK:
		AddSoundMood("cloak",1000,-1.0f,0,1.0f);
		break;
	case SOUNDMOOD_LEAVE_CLOAK:
		RemoveSoundMood("cloak",0.0f,1000);
		break;
	case SOUNDMOOD_ENTER_CONCENTRATION:
		AddSoundMood("concentration",1000,-1.0f,0,1.0f);
		break;
	case SOUNDMOOD_LEAVE_CONCENTRATION:
		RemoveSoundMood("concentration",0.0f,500);
		break;
	case SOUNDMOOD_EXPLOSION:
		AddSoundMood("explosion",1000,0.0f,3000,fPercent/100.0f);
		break;
	case SOUNDMOOD_ENTER_FREEZE:
		AddSoundMood("frozen",1500,-1.0f,0,1.0f);
		break;
	case SOUNDMOOD_LEAVE_FREEZE:
		RemoveSoundMood("frozen",0.0f,2000);
		break;
	case SOUNDMOOD_LOWHEALTH:
		AddSoundMood("low_health",0,3000.0f,1000,fPercent/100.0f);
		break;
	default:
		assert(0);
		break;
	}
}

//-----------------------------------------------------------------------------------------------------

void CSoundMoods::DisableAllSoundMoods()
{
	if (!m_pSoundMoodManager)
		return;

	for (auto& soundMood : m_vecSoundMoods)
	{
		if (soundMood.bValid)
		{
			m_pSoundMoodManager->UpdateSoundMood(soundMood.strSoundMood.c_str(), 0.0f, 1000.f);
			soundMood.bValid = false;
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void CSoundMoods::RemoveSoundMood(std::string_view szSoundMood, float fFade, uint32_t uiFadeOut)
{
	if (!m_pSoundMoodManager)
		return;

	for (auto& soundMood : m_vecSoundMoods)
	{
		if (soundMood.bValid && soundMood.strSoundMood == szSoundMood)
		{
			m_pSoundMoodManager->UpdateSoundMood(szSoundMood.data(), fFade, uiFadeOut);

			soundMood.bValid = false;
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void CSoundMoods::Serialize(TSerialize ser)
{
	if (!m_pSoundMoodManager)
		return;

	unsigned int uiSoundMood = 0;
	for (auto& soundMood : m_vecSoundMoods)
	{
		char szTemp[256];

		sprintf(szTemp, "strSoundMood_%d", uiSoundMood);
		ser.Value(szTemp, soundMood.strSoundMood);

		sprintf(szTemp, "uiFadeOutTime_%d", uiSoundMood);
		ser.Value(szTemp, soundMood.uiFadeOutTime);

		sprintf(szTemp, "uiFadeOut_%d", uiSoundMood);
		ser.Value(szTemp, soundMood.uiFadeOut);

		sprintf(szTemp, "bValid_%d", uiSoundMood);
		ser.Value(szTemp, soundMood.bValid);

		sprintf(szTemp, "bUnlimited_%d", uiSoundMood);
		ser.Value(szTemp, soundMood.bUnlimited);

		++uiSoundMood;
	}
}

//-----------------------------------------------------------------------------------------------------

void CSoundMoods::Update()
{
	if (!m_pSoundMoodManager)
		return;

	const float fTime = gEnv->pTimer->GetCurrTime();

	for (auto& soundMood : m_vecSoundMoods)
	{
		if (soundMood.bValid && fTime >= soundMood.uiFadeOutTime && !soundMood.bUnlimited)
		{
			m_pSoundMoodManager->UpdateSoundMood(soundMood.strSoundMood.c_str(), 0.0f, soundMood.uiFadeOut);
			soundMood.bValid = false;
		}
	}
}

//-----------------------------------------------------------------------------------------------------
