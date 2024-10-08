#include <cstdint>

#include "GameplayRecorder.h"

extern std::uintptr_t CRYACTION_BASE;

GameplayRecorder::GameplayRecorder(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2f3dc0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x203160;
#endif

	(this->*reinterpret_cast<void(GameplayRecorder::*&)(IGameFramework*)>(ctor))(pGameFramework);
}

GameplayRecorder::~GameplayRecorder()
{
}

void GameplayRecorder::Update(float frameTime)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2f3960;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x202970;
#endif

	(this->*reinterpret_cast<void(GameplayRecorder::*&)(float)>(func))(frameTime);
}

////////////////////////////////////////////////////////////////////////////////
// IGameplayRecorder
////////////////////////////////////////////////////////////////////////////////

void GameplayRecorder::RegisterListener(IGameplayListener* pGameplayListener)
{
}

void GameplayRecorder::UnregisterListener(IGameplayListener* pGameplayListener)
{
}

CTimeValue GameplayRecorder::GetSampleInterval() const
{
	return {};
}

void GameplayRecorder::Event(IEntity* pEntity, const GameplayEvent& event)
{
}

void GameplayRecorder::OnGameData(const IMetadata* pGameData)
{
}

////////////////////////////////////////////////////////////////////////////////
