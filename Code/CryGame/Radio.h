// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __RADIO_H__
#define __RADIO_H__

#pragma once

#include "GameActions.h"
#include "CryCommon/CryInput/IInput.h"

class CGameRules;
class SmartScriptTable;

class CRadio:public IInputEventListener
{
public:
	CRadio(CGameRules*);
	~CRadio();
	bool GetRadioSoundName(const string& teamName, const int groupId, const int keyId, char** ppSoundName, char** ppSoundText, int* pVariations);
	bool OnAction(const ActionId& actionId, int activationMode, float value);

	void Update();

	static const int RADIO_MESSAGE_NUM;

	bool IsExtendedRadio();

	//from IInputEventListener
	virtual bool	OnInputEvent( const SInputEvent &event );
	void			OnRadioMessage(int id, EntityId fromId);
	void			CancelRadio();
	void			SetTeam(const string& name);
	void			GetMemoryStatistics(ICrySizer * s);
private:
	CGameRules	*m_pGameRules;
	int			m_currentGroup;
	string		m_TeamName;
	float		m_lastMessageTime;
	float		m_menuOpenTime;		// used to close the menu if no input within n seconds.

	bool	m_keyState[10];
	bool	m_keyIgnored[10];
	int		m_requestedGroup;
	bool	m_inputEventConsumedKey;
	bool	m_waitForInputEvents;
	bool		UpdatePendingGroup();

	void		CloseRadioMenu();

	bool GetTeamRadioTable(const string& team_name, SmartScriptTable& out_table);
	int GetExtendedRadioId(int keyId, int groupId);
	bool GetGroupAndKeyFromExtendedRadioId(int radioId, int& group, int& newId);
};

#endif
