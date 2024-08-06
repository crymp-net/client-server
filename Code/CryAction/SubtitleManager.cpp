#include <cstdint>

#include "SubtitleManager.h"

SubtitleManager::SubtitleManager()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307aae80;
#else
	std::uintptr_t ctor = 0x306d0670;
#endif

	(this->*reinterpret_cast<void(SubtitleManager::*&)()>(ctor))();
}

SubtitleManager::~SubtitleManager()
{
}

////////////////////////////////////////////////////////////////////////////////
// ISubtitleManager
////////////////////////////////////////////////////////////////////////////////

void SubtitleManager::SetHandler(ISubtitleHandler* pHandler)
{
}

ISubtitleHandler* SubtitleManager::GetHandler()
{
	return {};
}

void SubtitleManager::SetEnabled(bool enabled)
{
}

void SubtitleManager::SetAutoMode(bool on)
{
}

void SubtitleManager::ShowSubtitle(ISound* pSound, bool show)
{
}

void SubtitleManager::ShowSubtitle(const char* subtitleLabel, bool show)
{
}

////////////////////////////////////////////////////////////////////////////////
