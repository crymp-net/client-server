#include <cstddef>
#include <cstdint>

#include "ActionGame.h"

extern std::uintptr_t CRYACTION_BASE;

ActionGame::ActionGame(ScriptRMI* pScriptRMI)
{
#ifdef BUILD_64BIT
	static_assert(sizeof(ActionGame) == 0xb70);
	static_assert(offsetof(ActionGame, m_pEntitySystem) == 0x20);
	static_assert(offsetof(ActionGame, m_pNetwork) == 0x28);
	static_assert(offsetof(ActionGame, m_pClientNub) == 0x30);
	static_assert(offsetof(ActionGame, m_pServerNub) == 0x38);
	static_assert(offsetof(ActionGame, m_pGameClientNub) == 0x40);
	static_assert(offsetof(ActionGame, m_pGameServerNub) == 0x48);
	static_assert(offsetof(ActionGame, m_pGameContext) == 0x50);
	static_assert(offsetof(ActionGame, m_pPhysicalWorld) == 0x68);
	static_assert(offsetof(ActionGame, m_globalPhysicsCallbacks) == 0x78);
	static_assert(offsetof(ActionGame, m_brokenObjs) == 0x258);
	static_assert(offsetof(ActionGame, m_breakEvents) == 0x270);
	static_assert(offsetof(ActionGame, m_brokenEntParts) == 0x288);
	static_assert(offsetof(ActionGame, m_brokenVegParts) == 0x2a0);
	static_assert(offsetof(ActionGame, m_broken2dChunkIds) == 0x2b8);
	static_assert(offsetof(ActionGame, m_entPieceIdx) == 0x2d0);
	static_assert(offsetof(ActionGame, m_mapBrokenTreesByPhysEnt) == 0x300);
	static_assert(offsetof(ActionGame, m_mapBrokenTreesByCGF) == 0x330);
	static_assert(offsetof(ActionGame, m_mapBrokenTreesChunks) == 0x360);
	static_assert(offsetof(ActionGame, m_lstCachedEffects) == 0x4b0);
#else
	static_assert(sizeof(ActionGame) == 0x7dc);
	static_assert(offsetof(ActionGame, m_pEntitySystem) == 0x10);
	static_assert(offsetof(ActionGame, m_pNetwork) == 0x14);
	static_assert(offsetof(ActionGame, m_pClientNub) == 0x18);
	static_assert(offsetof(ActionGame, m_pServerNub) == 0x1c);
	static_assert(offsetof(ActionGame, m_pGameClientNub) == 0x20);
	static_assert(offsetof(ActionGame, m_pGameServerNub) == 0x24);
	static_assert(offsetof(ActionGame, m_pGameContext) == 0x28);
	static_assert(offsetof(ActionGame, m_pPhysicalWorld) == 0x34);
	static_assert(offsetof(ActionGame, m_globalPhysicsCallbacks) == 0x3c);
	static_assert(offsetof(ActionGame, m_brokenObjs) == 0x12c);
	static_assert(offsetof(ActionGame, m_breakEvents) == 0x138);
	static_assert(offsetof(ActionGame, m_brokenEntParts) == 0x144);
	static_assert(offsetof(ActionGame, m_brokenVegParts) == 0x150);
	static_assert(offsetof(ActionGame, m_broken2dChunkIds) == 0x15c);
	static_assert(offsetof(ActionGame, m_entPieceIdx) == 0x168);
	static_assert(offsetof(ActionGame, m_mapBrokenTreesByPhysEnt) == 0x180);
	static_assert(offsetof(ActionGame, m_mapBrokenTreesByCGF) == 0x198);
	static_assert(offsetof(ActionGame, m_mapBrokenTreesChunks) == 0x1b0);
	static_assert(offsetof(ActionGame, m_lstCachedEffects) == 0x25c);
#endif

#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x308650;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x20F220;
#endif

	(this->*reinterpret_cast<void(IHitListener::*&)(ScriptRMI*)>(ctor))(pScriptRMI);
}

ActionGame::~ActionGame()
{
	// we use the original vtable for now -> virtual destructor -> the original one is used
}

bool ActionGame::Init(const SGameStartParams* pGameContextParams)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x313BB0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x217220;
#endif

	return (this->*reinterpret_cast<bool(IHitListener::*&)(const SGameStartParams*)>(func))(pGameContextParams);
}

bool ActionGame::Update()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x3036B0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20CFF0;
#endif

	return (this->*reinterpret_cast<bool(IHitListener::*&)()>(func))();
}

bool ActionGame::ChangeGameContext(const SGameContextParams* pGameContextParams)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x303320;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20CE70;
#endif

	return (this->*reinterpret_cast<bool(IHitListener::*&)(const SGameContextParams*)>(func))(pGameContextParams);
}

bool ActionGame::BlockingSpawnPlayer()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x303F80;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20D6C0;
#endif

	return (this->*reinterpret_cast<bool(IHitListener::*&)()>(func))();
}

void ActionGame::FixBrokenObjects()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x307380;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20EE30;
#endif

	(this->*reinterpret_cast<void(IHitListener::*&)()>(func))();
}

void ActionGame::ClearBreakHistory()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x307650;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20EFF0;
#endif

	(this->*reinterpret_cast<void(IHitListener::*&)()>(func))();
}

void ActionGame::OnEditorSetGameMode(bool isGameMode)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x308B30;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20F4B0;
#endif

	(this->*reinterpret_cast<void(IHitListener::*&)(bool)>(func))(isGameMode);
}

IActor* ActionGame::GetClientActor()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x303350;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20CEA0;
#endif

	return (this->*reinterpret_cast<IActor*(IHitListener::*&)()>(func))();
}

////////////////////////////////////////////////////////////////////////////////
// IHitListener
////////////////////////////////////////////////////////////////////////////////

void ActionGame::OnHit(const HitInfo&)
{
}

void ActionGame::OnExplosion(const ExplosionInfo&)
{
}

void ActionGame::OnServerExplosion(const ExplosionInfo&) 
{
}

////////////////////////////////////////////////////////////////////////////////
