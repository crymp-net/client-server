#pragma once

#include "CryCommon/CrySoundSystem/IMusicSystem.h"

struct IAnimationGraphState;

class MusicLogic : public IMusicLogic
{
#ifdef BUILD_64BIT
	unsigned char m_data[0xb8 - 0x8] = {};
#else
	unsigned char m_data[0x70 - 0x4] = {};
#endif

public:
	MusicLogic(IAnimationGraphState* pMusicGraphState);

	////////////////////////////////////////////////////////////////////////////////
	// IMusicLogic
	////////////////////////////////////////////////////////////////////////////////

	bool Init() override;

	bool Start() override;
	bool Stop() override;

	void Update() override;

	void SetEvent(EMusicLogicEvents MusicEvent, const float value) override;

	void GetMemoryStatistics(ICrySizer*) override;
	void Serialize(TSerialize ser) override;

	void DrawInformation(IRenderer* pRenderer, float xpos, float ypos, int soundInfo) override;

	////////////////////////////////////////////////////////////////////////////////
};
