#include <cstdint>

#include "DialogSystem.h"

DialogSystem::DialogSystem()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307a79b0;
#else
	std::uintptr_t ctor = 0x306cdf70;
#endif

	(this->*reinterpret_cast<void(DialogSystem::*&)()>(ctor))();
}

DialogSystem::~DialogSystem()
{
}

bool DialogSystem::Init()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
// IDialogSystem
////////////////////////////////////////////////////////////////////////////////

void DialogSystem::Update(const float dt)
{
}

void DialogSystem::Reset()
{
}

bool DialogSystem::ReloadScripts()
{
	return {};
}

IDialogScriptIteratorPtr DialogSystem::CreateScriptIterator()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
