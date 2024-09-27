#pragma once

#include "CryCommon/CryAction/IGameplayRecorder.h"

class GameplayAnalyst : public IGameplayListener
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x38 - 0x8] = {};
#else
	unsigned char m_data[0x1c - 0x4] = {};
#endif

public:
	GameplayAnalyst();
	virtual ~GameplayAnalyst();

	////////////////////////////////////////////////////////////////////////////////
	// IGameplayListener
	////////////////////////////////////////////////////////////////////////////////

	void OnGameplayEvent(IEntity* pEntity, const GameplayEvent& event) override;

	////////////////////////////////////////////////////////////////////////////////
};
