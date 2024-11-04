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
#ifndef __SOUNDMOODS_H__
#define __SOUNDMOODS_H__

//-----------------------------------------------------------------------------------------------------

#include "CryCommon/CryNetwork/ISerialize.h"

struct ISoundMoodManager;

//-----------------------------------------------------------------------------------------------------

enum ESOUNDMOOD
{
	SOUNDMOOD_ENTER_BINOCULARS,
	SOUNDMOOD_LEAVE_BINOCULARS,
	SOUNDMOOD_ENTER_CLOAK,
	SOUNDMOOD_LEAVE_CLOAK,
	SOUNDMOOD_ENTER_CONCENTRATION,
	SOUNDMOOD_LEAVE_CONCENTRATION,
	SOUNDMOOD_ENTER_FREEZE,
	SOUNDMOOD_LEAVE_FREEZE,
	SOUNDMOOD_EXPLOSION,
	SOUNDMOOD_LOWHEALTH
};

//-----------------------------------------------------------------------------------------------------

class CSoundMoods
{
public:

		CSoundMoods();
	~	CSoundMoods();

	void AddSoundMood(ESOUNDMOOD eSoundMood,float fPercent=0.0f);
	void DisableAllSoundMoods();
	void Serialize(TSerialize ser);
	void Update();

private:

	void AddSoundMood(std::string_view,uint32 uiFadeIn,float fDuration,uint32 fFadeOut,float fFade);
	void RemoveSoundMood(std::string_view szSoundMood,float fFade,uint32 uiFadeOut);

	ISoundMoodManager *m_pSoundMoodManager;

	struct SSoundMood
	{
		std::string strSoundMood;
		uint32 uiFadeOutTime;
		uint32 uiFadeOut;
		bool bValid;
		bool bUnlimited;

		SSoundMood() : bValid(false)
		{
		}
	};

	typedef std::vector<SSoundMood> TVectorSoundMoods;
	TVectorSoundMoods m_vecSoundMoods;
};

//-----------------------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------------------
