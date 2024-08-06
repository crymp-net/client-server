#include <cstdint>

#include "MusicLogic.h"

MusicLogic::MusicLogic(IAnimationGraphState* pMusicGraphState)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30800960;
#else
	std::uintptr_t ctor = 0x3070a440;
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
