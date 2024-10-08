#include <cstdint>

#include "EffectSystem.h"

extern std::uintptr_t CRYACTION_BASE;

EffectSystem::EffectSystem()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x27d390;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1b6280;
#endif

	(this->*reinterpret_cast<void(EffectSystem::*&)()>(ctor))();
}

EffectSystem::~EffectSystem()
{
}

////////////////////////////////////////////////////////////////////////////////
// IEffectSystem
////////////////////////////////////////////////////////////////////////////////

bool EffectSystem::Init()
{
	return false;
}

void EffectSystem::Update(float delta)
{
}

void EffectSystem::Shutdown()
{
}

void EffectSystem::GetMemoryStatistics(ICrySizer*)
{
}

EffectId EffectSystem::GetEffectId(const char* name)
{
	return 0;
}

void EffectSystem::Activate(const EffectId& eid)
{
}

bool EffectSystem::BindEffect(const char* name, IEffect* pEffect)
{
	return false;
}

IGroundEffect* EffectSystem::CreateGroundEffect(IEntity* pEntity)
{
	return nullptr;
}

void EffectSystem::RegisterFactory(const char *name, IEffect* (*)(), bool isAI)
{
}

////////////////////////////////////////////////////////////////////////////////
