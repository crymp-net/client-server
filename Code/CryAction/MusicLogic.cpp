#include <cstdint>

#include "MusicLogic.h"

extern std::uintptr_t CRYACTION_BASE;

MusicLogic::MusicLogic(IAnimationGraphState* pMusicGraphState)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x300960;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x20a440;
#endif

	(this->*reinterpret_cast<void(MusicLogic::*&)(IAnimationGraphState*)>(ctor))(pMusicGraphState);
}

////////////////////////////////////////////////////////////////////////////////
// IMusicLogic
////////////////////////////////////////////////////////////////////////////////

bool MusicLogic::Init()
{
	return {};
}

bool MusicLogic::Start()
{
	return {};
}

bool MusicLogic::Stop()
{
	return {};
}

void MusicLogic::Update()
{
}

void MusicLogic::SetEvent(EMusicLogicEvents MusicEvent, const float value)
{
}

void MusicLogic::GetMemoryStatistics(ICrySizer*)
{
}

void MusicLogic::Serialize(TSerialize ser)
{
}

void MusicLogic::DrawInformation(IRenderer* pRenderer, float xpos, float ypos, int soundInfo)
{
}

////////////////////////////////////////////////////////////////////////////////
