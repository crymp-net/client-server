#include <cstdint>

#include "GameplayRecorder.h"

GameplayRecorder::GameplayRecorder(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307f3dc0;
#else
	std::uintptr_t ctor = 0x30703160;
#endif

	(this->*reinterpret_cast<void(GameplayRecorder::*&)(IGameFramework*)>(ctor))(pGameFramework);
}

GameplayRecorder::~GameplayRecorder()
{
}

void GameplayRecorder::Update(float frameTime)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x307f3960;
#else
	std::uintptr_t func = 0x30702970;
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
