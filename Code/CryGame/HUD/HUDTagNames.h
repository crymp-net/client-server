/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:
	MP TagNames

-------------------------------------------------------------------------
History:
- 21:05:2007: Created by Julien Darre

*************************************************************************/
#ifndef __HUDTAGNAMES_H__
#define __HUDTAGNAMES_H__

#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------------

class CHUDTagNames
{
public:
	explicit CHUDTagNames(CHUD* pHUD);
	~CHUDTagNames();

	void Update();

	//added in MP when client hit another player/vehicle
	void AddEnemyTagName(EntityId uiEntityId);

private:
	const std::wstring& GetPlayerRank(EntityId entityId);

	bool ProjectOnSphere(Vec3 &rvWorldPos,const AABB &rvBBox);

	bool IsFriendlyToClient(EntityId uiEntityId);

	IUIDraw *m_pUIDraw = nullptr;
	IFFont *m_pMPNamesFont = nullptr;
	CHUD* m_pHUD = nullptr;

	struct NameTag
	{
		Vec3 pos;
		ColorF color;
		std::wstring text;
		bool drawOnTop = false;
	};

	struct EnemyTag
	{
		EntityId entityId = 0;
		float spawnTime = 0;
	};

	std::vector<NameTag> m_nameTags;
	std::vector<EnemyTag> m_enemyTags;
	std::vector<std::wstring> m_rankNames;

	void DrawTagName(IActor *pActor,bool bLocalVehicle = false);
	void DrawTagName(IVehicle *pVehicle);
	void DrawTagNames();
};

//-----------------------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------------------
