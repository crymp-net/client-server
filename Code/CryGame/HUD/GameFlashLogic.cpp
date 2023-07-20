/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:

-------------------------------------------------------------------------
History:
- 18:08:2005: Created by Julien Darre

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "GameFlashLogic.h"
#include "CryCommon/CryGame/IGame.h"
#include "CryCommon/CrySystem/IFlashPlayer.h"
#include "CryCommon/CryAction/IGameFramework.h"

//-----------------------------------------------------------------------------------------------------

CGameFlashLogic::CGameFlashLogic(CGameFlashAnimation *pGameFlashAnimation)
{
	m_pGameFlashAnimation = pGameFlashAnimation;
}

//-----------------------------------------------------------------------------------------------------

CGameFlashLogic::~CGameFlashLogic()
{
	gEnv->pGame->GetIGameFramework()->GetIGameTokenSystem()->UnregisterListener(m_token.c_str(), this);
}

//-----------------------------------------------------------------------------------------------------

void CGameFlashLogic::Init(const char *strASControl,const char *strVariable,const char *strToken,float fScale,float fOffset)
{
	m_token = strToken;
	m_invoke = string(strASControl) + "." + string(strVariable);
	m_fScale = fScale;
	m_fOffset = fOffset;

	gEnv->pGame->GetIGameFramework()->GetIGameTokenSystem()->RegisterListener(m_token.c_str(),this,true,true);
}

//-----------------------------------------------------------------------------------------------------

void CGameFlashLogic::ReInit()
{
	gEnv->pGame->GetIGameFramework()->GetIGameTokenSystem()->RegisterListener(m_token.c_str(),this,true,true);
}

//-----------------------------------------------------------------------------------------------------

void CGameFlashLogic::OnGameTokenEvent( EGameTokenEvent event,IGameToken *pGameToken )
{
	if(EGAMETOKEN_EVENT_CHANGE == event)
	{
		float fValue = 0.0f;
		if(pGameToken->GetValueAs(fValue))
		{
			static char strValue[32];
			sprintf(strValue,"%f",fValue*m_fScale+m_fOffset);
			m_pGameFlashAnimation->SetVariable(m_invoke.c_str(), strValue);
		}
	}
}

//-----------------------------------------------------------------------------------------------------
void CGameFlashLogic::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
}
