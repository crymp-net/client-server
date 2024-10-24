#pragma once

#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CryAction/IGameRulesSystem.h"
#include "CryCommon/CryAction/IMaterialEffects.h"
#include "CryCommon/CryCore/smartptr.h"
#include "Library/StlportMap.h"
#include "Library/StlportSet.h"
#include "Library/StlportVector.h"

class GameContext;
class GameClientNub;
class GameServerNub;
class GameStats;
class ScriptRMI;

struct BrokenObjRec
{
	int itype;
	IRenderNode* pBrush;
	EntityId idEnt;
	int islot;
	IStatObj* pStatObjOrg;
	float mass;
};

struct BreakEvent
{
	int itype;
	EntityId idEnt;
	Vec3 objPos;
	Vec3 objCenter;
	float objVolume;

	Vec3 pos;
	Quat rot;
	Vec3 scale;

	Vec3 pt;
	Vec3 n;
	Vec3 vloc[2];
	float mass[2];
	int partid[2];
	int idmat[2];
	float penetration;
	float energy;
	float radius;

	int idxChunkId0;
	int nChunks;
	int seed;

	void Serialize(TSerialize ser)
	{
		ser.Value("type", itype);
		ser.Value("idEnt", idEnt);
		ser.Value("objPos", objPos);
		ser.Value("objCenter", objCenter);
		ser.Value("objVolume", objVolume);
		ser.Value("pos", pos);
		ser.Value("rot", rot);
		ser.Value("scale", scale);
		ser.Value("pt", pt);
		ser.Value("n", n);
		ser.Value("vloc0", vloc[0]);
		ser.Value("vloc1", vloc[1]);
		ser.Value("mass0", mass[0]);
		ser.Value("mass1", mass[1]);
		ser.Value("partid0", partid[0]);
		ser.Value("partid1", partid[1]);
		ser.Value("idmat0", idmat[0]);
		ser.Value("idmat1", idmat[1]);
		ser.Value("penetration", penetration);
		ser.Value("idxChunkId0", idxChunkId0);
		ser.Value("nChunks", nChunks);
		ser.Value("seed", seed);
		ser.Value("energy", energy);
		ser.Value("radius", radius);
	}
};

struct BrokenEntPart
{
	EntityId idSrcEnt;
	EntityId idNewEnt;

	void Serialize(TSerialize ser)
	{
		ser.Value("idSrcEnt", idSrcEnt);
		ser.Value("idNewEnt", idNewEnt);
	}
};

struct BrokenVegPart
{
	Vec3 pos;
	float volume;
	EntityId idNewEnt;

	void Serialize(TSerialize ser)
	{
		ser.Value("pos", pos);
		ser.Value("volume", volume);
		ser.Value("idNewEnt", idNewEnt);
	}
};

struct TreeBreakInst;
struct TreePieceThunk
{
	IPhysicalEntity* pPhysEntNew;
	TreePieceThunk* pNext;
	TreeBreakInst* pParent;
};

struct TreeBreakInst
{
	IPhysicalEntity* pPhysEntSrc;
	IPhysicalEntity* pPhysEntNew0;
	TreePieceThunk* pNextPiece;
	TreeBreakInst* pThis;
	TreeBreakInst* pNext;
	IStatObj* pStatObj;
	float cutHeight;
	float cutSize;
};

struct EntityCollHist
{
	EntityCollHist* pnext;
	float velImpact;
	float velSlide2;
	float velRoll2;
	int imatImpact[2];
	int imatSlide[2];
	int imatRoll[2];
	float timeRolling;
	float timeNotRolling;
	float rollTimeout;
	float slideTimeout;
	float mass;
};

struct EntityHits
{
	EntityHits* pnext;
	Vec3* pHits;
	Vec3 hit0;
	int* pnHits;
	int nhits0;
	int nHits;
	int nHitsAlloc;
	float hitRadius;
	int hitpoints;
	int maxdmg;
	int nMaxHits;
	float timeUsed;
	float lifeTime;
};

struct VegCollisionStatus
{
	IRenderNode* rn = nullptr;
	IStatObj* pStat = nullptr;
	int branchNum = -1;
};

class ActionGame : public IHitListener, public _reference_target_t
{
	// m_reserved_<32-bit-offset>_<64-bit-offset>
	void* m_reserved_0xc_0x18 = nullptr;
	IEntitySystem* m_pEntitySystem = nullptr;  // m_reserved_0x10_0x20
	INetwork* m_pNetwork = nullptr;  // m_reserved_0x14_0x28
	INetNub* m_pClientNub = nullptr;  // m_reserved_0x18_0x30
	INetNub* m_pServerNub = nullptr;  // m_reserved_0x1c_0x38
	GameClientNub* m_pGameClientNub = nullptr;  // m_reserved_0x20_0x40
	GameServerNub* m_pGameServerNub = nullptr;  // m_reserved_0x24_0x48
	GameContext* m_pGameContext = nullptr;  // m_reserved_0x28_0x50
	void* m_reserved_0x2c_0x58 = nullptr;
	void* m_reserved_0x30_0x60 = nullptr;
	IPhysicalWorld* m_pPhysicalWorld = nullptr;  // m_reserved_0x34_0x68
	void* m_reserved_0x38_0x70 = nullptr;
	struct PhysCallbacks
	{
		using CallbackSet = StlportSet<std::pair<void(*)(const EventPhys*, void*), void*>>;
		CallbackSet collision[2];
		CallbackSet postStep[2];
		CallbackSet stateChange[2];
		CallbackSet createEntityPart[2];
		CallbackSet updateMesh[2];
	} m_globalPhysicsCallbacks;  // m_reserved_0x3c_0x78
	StlportVector_CryAction<BrokenObjRec> m_brokenObjs;  // m_reserved_0x12c_0x258
	StlportVector_CryAction<BreakEvent> m_breakEvents;  // m_reserved_0x138_0x270
	StlportVector_CryAction<BrokenEntPart> m_brokenEntParts;  // m_reserved_0x144_0x288
	StlportVector_CryAction<BrokenVegPart> m_brokenVegParts;  // m_reserved_0x150_0x2a0
	StlportVector_CryAction<EntityId> m_broken2dChunkIds;  // m_reserved_0x15c_0x2b8
	StlportMap<EntityId, int> m_entPieceIdx;  // m_reserved_0x168_0x2d0
	StlportMap<IPhysicalEntity*, TreeBreakInst*> m_mapBrokenTreesByPhysEnt;  // m_reserved_0x180_0x300
	StlportMap<IStatObj*, TreeBreakInst*> m_mapBrokenTreesByCGF;  // m_reserved_0x198_0x330
	StlportMap<IPhysicalEntity*, TreePieceThunk*> m_mapBrokenTreesChunks;  // m_reserved_0x1b0_0x360
	int m_currentBreakEvent = 0;
	int m_totalBreakageSize = 0;
	GameStats* m_pGameStats = nullptr;
	EntityCollHist* m_pCHSlotPool = nullptr;
	EntityCollHist* m_pFreeCHSlot0 = nullptr;
	StlportMap<int, EntityCollHist*> m_mapECH;
	EntityHits* m_pEntHits0 = nullptr;
	StlportMap<int, EntityHits*> m_mapEntHits;
	StlportMap<EntityId, Vec3> m_vegStatus;
	StlportMap<EntityId, VegCollisionStatus*> m_treeStatus;
	StlportMap<EntityId, VegCollisionStatus*> m_treeBreakStatus;
	int m_effectCounter = 0;
	SMFXRunTimeEffectParams	m_lstCachedEffects[8];  // m_reserved_0x25c_0x4b0

public:
	explicit ActionGame(ScriptRMI* pScriptRMI);
	~ActionGame();

	bool Init(const SGameStartParams* pGameContextParams);
	bool Update();

	bool ChangeGameContext(const SGameContextParams* pGameContextParams);
	bool BlockingSpawnPlayer();
	void FixBrokenObjects();
	void ClearBreakHistory();
	void OnEditorSetGameMode(bool isGameMode);

	IActor* GetClientActor();

	INetNub* GetClientNetNub() { return m_pClientNub; }
	INetNub* GetServerNetNub() { return m_pServerNub; }
	GameClientNub* GetGameClientNub() { return m_pGameClientNub; }
	GameServerNub* GetGameServerNub() { return m_pGameServerNub; }
	GameContext* GetGameContext() { return m_pGameContext; }

	////////////////////////////////////////////////////////////////////////////////
	// IHitListener
	////////////////////////////////////////////////////////////////////////////////

	void OnHit(const HitInfo&) override;
	void OnExplosion(const ExplosionInfo&) override;
	void OnServerExplosion(const ExplosionInfo&) override;

	////////////////////////////////////////////////////////////////////////////////
};
