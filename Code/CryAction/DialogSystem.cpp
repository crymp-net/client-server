#include <cstdint>

#include "DialogSystem.h"

extern std::uintptr_t CRYACTION_BASE;

DialogSystem::DialogSystem()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2a79b0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1cdf70;
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
