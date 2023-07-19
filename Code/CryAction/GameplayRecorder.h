#pragma once

#include "CryCommon/CryAction/IGameplayRecorder.h"

struct IGameFramework;

class GameplayRecorder : public IGameplayRecorder
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x48 - 0x8] = {};
#else
	unsigned char m_data[0x30 - 0x4] = {};
#endif

public:
	explicit GameplayRecorder(IGameFramework* pGameFramework);
	virtual ~GameplayRecorder();

	////////////////////////////////////////////////////////////////////////////////
	// IGameplayRecorder
	////////////////////////////////////////////////////////////////////////////////

	void RegisterListener(IGameplayListener* pGameplayListener) override;
	void UnregisterListener(IGameplayListener* pGameplayListener) override;

	CTimeValue GetSampleInterval() const override;

	void Event(IEntity* pEntity, const GameplayEvent& event) override;

	void OnGameData(const IMetadata* pGameData) override;

	////////////////////////////////////////////////////////////////////////////////
};
