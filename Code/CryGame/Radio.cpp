// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CryInput/IInput.h"
#include "Radio.h"
#include "GameRules.h"
#include "Game.h"
#include "HUD/HUD.h"
#include "HUD/HUDRadar.h"
#include "HUD/HUDTextChat.h"

static const int RADIO_GROUPS=4;
static const int RADIO_GROUP_SIZE=5;
const int CRadio::RADIO_MESSAGE_NUM=RADIO_GROUPS*RADIO_GROUP_SIZE;//sizeof(RadioMessages)/sizeof(RadioMessage_s);
static const int RADIO_AUTOCLOSE_TIME = 5;

//------------------------------------------------------------------------

CRadio::CRadio(CGameRules* gr):m_currentGroup(-1),m_pGameRules(gr),m_lastMessageTime(0.0f),m_menuOpenTime(0.0f)
{
//	gEnv->pInput->AddEventListener(this);
	for (int i = 0; i < 10; ++i)
	{
		m_keyState[i] = false;
		m_keyIgnored[i] = false;
	}
	m_requestedGroup = -1;
	m_inputEventConsumedKey = false;
	m_waitForInputEvents = false;
}

CRadio::~CRadio()
{
	if (gEnv->pInput) gEnv->pInput->RemoveEventListener(this);
	CancelRadio();
}

static void PlaySound(const char* name)
{
	_smart_ptr<ISound> pSound = gEnv->pSoundSystem->CreateSound(name,0);
	if (pSound)
		pSound->Play();
}

static void PlayVoice(const char* name)
{
	_smart_ptr<ISound> pSound = gEnv->pSoundSystem->CreateSound(name,FLAG_SOUND_2D|FLAG_SOUND_VOICE);
	if (pSound)
	{
		pSound->SetPosition(g_pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetWorldPos());
		pSound->Play();
	}
}

void CRadio::CancelRadio()
{
	if(m_currentGroup!=-1)
	{
		m_currentGroup=-1;
		if (gEnv->pInput) gEnv->pInput->SetExclusiveListener(0);
	}
}

bool CRadio::GetTeamRadioTable(const string& team_name,SmartScriptTable& out_table)
{
	if(!m_pGameRules)
		return false;

	IScriptTable *pTable= m_pGameRules->GetEntity()->GetScriptTable();

	if(!pTable)
		return false;

	SmartScriptTable pTeamRadio;
	if(!pTable->GetValue("teamRadio",pTeamRadio))
		return false;

	if(!pTeamRadio->GetValue(team_name.c_str(),out_table))
		return false;

	return true;
}

bool CRadio::GetRadioSoundName(const string &teamName,const int groupId,const int keyId,char **ppSoundName=0,char **ppSoundText=0, int* pVariations = 0)
{
	SmartScriptTable radioTable;
	if (!GetTeamRadioTable(teamName, radioTable))
		return false;

	SmartScriptTable groupTable;
	if(!radioTable->GetAt(groupId,groupTable))
		return false;

	SmartScriptTable soundTable;
	if(!groupTable->GetAt(keyId,soundTable))
		return false;

	ScriptAnyValue soundName;
	ScriptAnyValue soundText;
	ScriptAnyValue soundVariations = 1;
	
	if(!soundTable->GetAtAny(1,soundName) || !soundTable->GetAtAny(2,soundText))
		return false;

	soundTable->GetAtAny(3, soundVariations);
	if(pVariations)
		soundVariations.CopyTo(*pVariations);

	if(ppSoundName)
	{
		if(!soundName.CopyTo(*ppSoundName))
			return false;
	}

	if(ppSoundText)
	{
		if(!soundText.CopyTo(*ppSoundText))
			return false;
	}
	
	return true;
}

int CRadio::GetExtendedRadioId(int keyId, int groupId)
{
	SmartScriptTable radioTable;
	if (!GetTeamRadioTable(m_TeamName, radioTable))
		return 0;

	//CryMP: Extended radio. Default radio has 5 radios per column
	int addedRadioCounter = 0;
	for (int index = 1; index < RADIO_GROUP_SIZE; ++index)
	{
		SmartScriptTable sGroupTable;
		if (radioTable->GetAt(index, sGroupTable))
		{
			SmartScriptTable sRadioInfo;
			if (index == groupId)
			{
				if (sGroupTable->GetAt(keyId, sRadioInfo))
				{
					int radioId = (RADIO_GROUP_SIZE * RADIO_GROUPS);
					radioId += (addedRadioCounter + (keyId - RADIO_GROUP_SIZE));
					return radioId;
				}
			}
			else
			{
				addedRadioCounter += (MAX(0, (sGroupTable->Count() - RADIO_GROUP_SIZE)));
			}
		}
	}

	return 0;
}

bool CRadio::IsExtendedRadio()
{
	SmartScriptTable radioTable;
	if (!GetTeamRadioTable(m_TeamName, radioTable))
		return false;

	SmartScriptTable sGroupTable;
	if (radioTable->GetAt(1, sGroupTable))
	{
		//There are more than 5 radios -> show m
		return sGroupTable->Count() > RADIO_GROUP_SIZE;
	}

	return false;
}

bool CRadio::GetGroupAndKeyFromExtendedRadioId(int radioId, int &groupId, int &keyId)
{
	SmartScriptTable radioTable;
	if (!GetTeamRadioTable(m_TeamName, radioTable))
		return false;

	int totalDefaultRadioCount = (RADIO_GROUP_SIZE * RADIO_GROUPS);

	/*
	//CryMP: Extended radio. Default radio has 5 radios per column
	//The extended radio is 'hardcoded' like the original, the previously unused radio commands
	//have been added. Don't expect anyone to create any new radio commands from scratch :)
	
	//1st group + 4 commands
	//2nd group + 2 commands
	//3rd group + 1 command
	//4th group + 1 command

	//lets say, newId comes in as '24' - will be the 4th new command 
	*/
	int subtract = 0;
	if (radioId > 19 && radioId < 24)
	{
		groupId = 0;
	}
	else if (radioId > 23 && radioId < 26)
	{
		groupId = 1;
		subtract = 4;
	}
	else if (radioId == 26)
	{
		groupId = 2;
		subtract = 6;
	}
	else if (radioId == 27)
	{
		groupId = 3;
		subtract = 7;
	}

	keyId = radioId - totalDefaultRadioCount + RADIO_GROUP_SIZE - subtract;

	SmartScriptTable groupTable;
	if (!radioTable->GetAt(groupId+1, groupTable))
		return false;

	return true;
}

bool CRadio::OnAction(const ActionId& actionId, int activationMode, float value)
{
	if(!gEnv->bMultiplayer)
		return false;

	//CryLogAlways("RADIO: Action[%3.2f] %s", gEnv->pTimer->GetCurrTime(), m_ignoreMenuThisFrame ? "ignored" : "");

	// not sure why this is needed, maybe not anymore (but it works regardless).
	if (m_inputEventConsumedKey)
		return false;

	// prevent spamming radio messages
	float timeNow = gEnv->pTimer->GetCurrTime();
	if((timeNow - m_lastMessageTime) < 2.0f)
	{
		return false;
	}

	if (gEnv->pInput) 
		if(gEnv->pInput->GetExclusiveListener())
			return false;

	IActor *pClient = g_pGame->GetIGameFramework()->GetClientActor();

	if(!pClient || (intptr_t)pClient<=0 || (static_cast<CPlayer*>(pClient))->GetSpectatorMode())
		return false;

	if(m_TeamName.empty())
		return false;

	const CGameActions& actions = g_pGame->Actions();

	m_requestedGroup = -1;

	if(actions.radio_group_0 == actionId)
		m_requestedGroup = 0;
	else if(actions.radio_group_1 == actionId)
		m_requestedGroup = 1;
	else if(actions.radio_group_2 == actionId)
		m_requestedGroup = 2;
	else if(actions.radio_group_3 == actionId)
		m_requestedGroup = 3;

	// Signal that pending menu request should not be processed before input events have been processed and sent.
	m_waitForInputEvents = true;

	// if menu is not open, ignore first keypress, to prevent same key to be used in sublevel.
	if (m_currentGroup == -1)
		for (int i = 0; i < 10; ++i)
			m_keyIgnored[i] = true;

	return true;
}

bool CRadio::UpdatePendingGroup()
{
	// key used in menu request was already consumed by sub level,
	// or the menu is open, but sub level have not yet got a change to update.
	if (m_inputEventConsumedKey || 	
			((m_currentGroup != -1) && m_waitForInputEvents))
	{
		m_inputEventConsumedKey = false;
		m_requestedGroup = -1;
		return false;
	}

	if (m_requestedGroup == -1)
		return false;

	if (m_requestedGroup == m_currentGroup)
	{
		CloseRadioMenu();
		m_currentGroup = -1;
		m_requestedGroup = -1;
		return true;
	}

	m_currentGroup = m_requestedGroup;
	m_requestedGroup = -1;

	if (g_pGame->GetHUD())
	{
		g_pGame->GetHUD()->SetRadioButtons(true, m_currentGroup + 1, IsExtendedRadio());
	}

	m_menuOpenTime = gEnv->pTimer->GetCurrTime();

	PlaySound("Sounds/interface:menu:pop_up");

	if (gEnv->pInput) gEnv->pInput->AddEventListener(this);
	g_pGameActions->FilterMPRadio()->Enable(true);

	SmartScriptTable radioTable;
	if(!GetTeamRadioTable(m_TeamName,radioTable))
		return false;

	SmartScriptTable groupTable;
	if(!radioTable->GetAt(m_currentGroup+1,groupTable))
		return false;

	return true;
}	

bool CRadio::OnInputEvent( const SInputEvent &event )
{
	if (event.deviceId!=eDI_Keyboard)
		return false;

	if (!gEnv->bMultiplayer)
		return false;

	const EKeyId keyId = event.keyId;
	if (keyId == eKI_Escape) //CryMP
	{
		CloseRadioMenu();
		return true;
	}

	//CryLogAlways("RADIO: Input[%3.2f] %s %d", gEnv->pTimer->GetCurrTime(), event.keyName, event.state);

	// Signal that input events have been processed and sent to all listeners.
	m_waitForInputEvents = false;

	const char* sKey = event.keyName;
	// nasty check, but fastest early out
	int iKey = -1;

	if(sKey && sKey[0] && !sKey[1])
	{
		iKey = atoi(sKey);
		if(iKey == 0 && sKey[0] != '0')
			iKey = -1;
	}

	if(iKey==-1)
		return false;

	if (event.state == eIS_Released)
		m_keyState[iKey] = false;

	bool pressed = false;
	if ((!m_keyState[iKey]) && ((event.state == eIS_Pressed) || (event.state == eIS_Down)))
	{
		pressed = true;
		m_keyState[iKey] = true;
	}

	if(gEnv->pConsole->GetStatus())
		return false;

	// ignore sub keys while menu is closed.
	bool menuOpen = (m_currentGroup != -1);
	if (!menuOpen)
		return false;

	// ignore this key, since it was pressed before or when menu opened.
	if (m_keyIgnored[iKey])
		return false;

	if (!pressed)
		return false;

	const bool bSuccess = GetRadioSoundName(m_TeamName, m_currentGroup + 1, iKey);

	if(!bSuccess)
		return false;

	m_keyState[iKey] = false; // release will never come, since the input event listened is unregistered.
	m_inputEventConsumedKey = true; // next pending menu update, ignore menu change, since this key was used in the sublevel.

	// reset key ignorance.
	for (int i = 0; i < 10; ++i)
		m_keyIgnored[i] = false;

	// prevent spamming radio messages
	m_lastMessageTime = gEnv->pTimer->GetCurrTime();

	//PlayVoice(pSoundName);

	int id=(m_currentGroup*RADIO_GROUP_SIZE+iKey)-1;

	//CryMP: Extended Menu support
	const bool bExtendedMenu = iKey > RADIO_GROUP_SIZE;
	if (bExtendedMenu)
	{
		int radioId = GetExtendedRadioId(iKey, m_currentGroup + 1);
		id = radioId - 1;
	}

	m_pGameRules->SendRadioMessage(gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntityId(),id);

	CancelRadio();

	if(g_pGame->GetHUD())
		g_pGame->GetHUD()->SetRadioButtons(false);

	m_menuOpenTime = 0.0f;

	g_pGameActions->FilterMPRadio()->Enable(false);
	if (gEnv->pInput) gEnv->pInput->RemoveEventListener(this);

	return true;
}

void CRadio::OnRadioMessage(SRadioMessageParams params)
{
	int groupId=params.id / RADIO_GROUP_SIZE;
	int keyId= params.id % RADIO_GROUP_SIZE;

	//CryMP: Extended menu
	if (params.id > ((RADIO_GROUP_SIZE * RADIO_GROUPS) - 1))
	{
		if (!GetGroupAndKeyFromExtendedRadioId(params.id, groupId, keyId))
			return;
	}

	//CryLogAlways("OnRadioMessage id %d, groupId %d - keyId %d", id, groupId, keyId);

	char *pSoundName,*pSoundText;
	int variations = 1;
	const bool bSuccess=GetRadioSoundName(m_TeamName,groupId+1,keyId+1,&pSoundName,&pSoundText, &variations);

	if (!bSuccess)
		return;

	if(g_pGame->GetGameRules() && g_pGame->GetHUD())
	{		
		ILocalizationManager *pLocalizationMan = gEnv->pSystem->GetLocalizationManager();
		
		wstring completeMsg;
		wstring wRadio, wSoundText;
		pLocalizationMan->LocalizeString("@mp_radio", wRadio);
		pLocalizationMan->LocalizeString(pSoundText, wSoundText);

		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(params.sourceId);
		if(pEntity)
		{
			Vec2i grid=g_pGame->GetHUD()->GetRadar()->GetMapGridPosition(pEntity);
			completeMsg += L"(";
			completeMsg += wRadio;
			completeMsg += L", ";
			wchar_t letter = 'A' + grid.x - 1;
			wchar_t number = '0' + grid.y;
			completeMsg += letter;
			completeMsg += number;
			completeMsg += L"): ";
		}
		completeMsg += wSoundText;


		//pLocalizationMan->FormatStringMessage(localizedString, completeMsg, "@mp_radio", pSoundText);

		CHUDTextChat* pChat = g_pGame->GetHUD()->GetMPChat();
		if(pChat)
			pChat->AddChatMessage(params.sourceId, completeMsg.c_str(), 1, true);	// hardcoded to 'same team'
		//g_pGame->GetGameRules()->OnChatMessage(eChatToTeam, fromId, 0, completeMsg.c_str());

		string sound = pSoundName;
		if(variations > 1)
		{
			int rand = Random(variations);
			sound += string().Format("_0%1d", rand+1);
		}

		if (params.pos) {
			params.msg = string{ pSoundText };
			g_pGame->GetHUD()->AddTrackedRadioMessage(params, 9.0f);
		}

		PlayVoice(sound.c_str());
	}
}

void CRadio::SetTeam(const string& name)
{
	m_TeamName=name;
}

void CRadio::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->Add(m_TeamName);
}

void CRadio::Update()
{
	// if input events has not been processed yet, wait with resetting ignore flags.
	// always reset ignore flag when not in menu, not sure why, but works as is.
	if (!m_waitForInputEvents)
		for (int i = 0; i < 10; ++i)
			if (!m_keyState[i] || (m_currentGroup == -1))
				m_keyIgnored[i] = false;

	// close menu if there has been no relevent userinput for more than 5s
	float timeNow = gEnv->pTimer->GetCurrTime();

/*
	CryLogAlways("RADIO: Update[%3.2f] state=[%d %d %d %d] ignore=[%d %d %d %d]", gEnv->pTimer->GetCurrTime(), 
							m_keyState[1], m_keyState[2], m_keyState[3], m_keyState[4],
							m_keyIgnored[1], m_keyIgnored[2], m_keyIgnored[3], m_keyIgnored[4]);
*/

	// This is where the menu request is actually processed.
	// m_requestedGroup, m_inputEventConsumedKey and m_waitForInputEvents are needed to ensure a certain update order.
	// Without this the order or action, input and update events are somewhat random.
	UpdatePendingGroup();

	if(m_menuOpenTime != 0.0f && (timeNow - m_menuOpenTime) > RADIO_AUTOCLOSE_TIME)
	{
		CloseRadioMenu();
	}
}

void CRadio::CloseRadioMenu()
{
	PlaySound("Sounds/interface:menu:close");
	CancelRadio();
	if(g_pGame->GetHUD())
		g_pGame->GetHUD()->SetRadioButtons(false);

	g_pGameActions->FilterMPRadio()->Enable(false);
#if !defined(CRY_USE_GCM_HUD)
	if (gEnv->pInput) gEnv->pInput->RemoveEventListener(this);
#endif

	m_menuOpenTime = 0.0f;
}
