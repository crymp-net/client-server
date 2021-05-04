/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: HUD score board for multiplayer

-------------------------------------------------------------------------
History:
- 27:03:2006: Created by Jan Müller

*************************************************************************/
#ifndef __HUDSCORE_H__
#define __HUDSCORE_H__

#include "HUDObject.h"

struct IActor;
class CGameFlashAnimation;

class CHUDScore : public CHUDObject
{
	friend class CHUD;

	struct ScoreEntry
	{
		int					m_kills;
		int					m_deaths;
		int					m_ping;
		int					m_team;
		int					m_currentRank;
		EntityId		m_entityId;
		bool				m_alive, m_spectating; //updating during sorting

		ScoreEntry(EntityId id, int kills, int deaths, int ping);
		bool operator<(const ScoreEntry& entry) const;
		void UpdateLiveStats();
	};

public:
	struct SRankStats
	{
		int currentRank;
		const char* nextRankName;
		const char* currentRankName;
		int currentRankCP;
		int nextRankCP;
		int currentRankPP;
		int nextRankPP;
		int playerCP;

		SRankStats()
		{
			currentRank = 0;
			nextRankName = 0;
			currentRankName = 0;
			currentRankCP = 0;
			nextRankCP = 0;
			currentRankPP = 0;
			nextRankPP = 0;
			playerCP = 0;
		}

		void Update(IScriptTable* pGameRulesScript, IActor* pActor, CGameFlashAnimation* pFlashBoard = 0);
	};

	CHUDScore(CHUD* pHUD)
	{
		m_pHUD = pHUD;
		m_bShow = false;
		m_lastUpdate = 0.0f;
		m_lastShowSwitch = 0;
		m_pFlashBoard = NULL;
		m_currentClientTeam = -1;
	}

	~CHUDScore()
	{
	}

	virtual void Update(float fDeltaTime);

	virtual bool OnInputEvent(const SInputEvent &event )
	{
		return false;
	}

	std::vector<ScoreEntry> m_scoreBoard;

	void Reset();
	void AddEntry(EntityId player, int kills, int deaths, int ping);

	void GetMemoryStatistics(ICrySizer * s);

	ILINE void SetVisible(bool visible, CGameFlashAnimation *board = NULL)
	{
		m_bShow = visible;
		m_pFlashBoard = board;
		m_lastShowSwitch = gEnv->pTimer->GetFrameStartTime().GetSeconds();
	}
private:

	CHUD* m_pHUD;
	float			m_lastUpdate;
	int				m_currentClientTeam;
	bool			m_bShow;
	float			m_lastShowSwitch;
	string		m_currentServer;
	string    m_currentServerIp;	
	CGameFlashAnimation *m_pFlashBoard;

	void Render();
};

#endif