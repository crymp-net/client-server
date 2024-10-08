#include <cstdint>

#include "AnimationGraphSystem.h"

extern std::uintptr_t CRYACTION_BASE;

AnimationGraphSystem::AnimationGraphSystem()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x1f46c0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x155ff0;
#endif

	(this->*reinterpret_cast<void(AnimationGraphSystem::*&)()>(ctor))();
}

void AnimationGraphSystem::RegisterFactories(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1f9b60;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1590e0;
#endif

	(this->*reinterpret_cast<void(AnimationGraphSystem::*&)(IGameFramework*)>(func))(pGameFramework);
}

IAnimationGraphPtr AnimationGraphSystem::LoadGraph(const char* filename, bool reload, bool loadBinary)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1f53f0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x156430;
#endif

	return (this->*reinterpret_cast<
		IAnimationGraphPtr(AnimationGraphSystem::*&)(const CryStringT<char>&, bool, bool)
	>(func))(filename, reload, loadBinary);
}

////////////////////////////////////////////////////////////////////////////////
// IAnimationGraphSystem
////////////////////////////////////////////////////////////////////////////////

IAnimationGraphCategoryIteratorPtr AnimationGraphSystem::CreateCategoryIterator()
{
	return {};
}

IAnimationGraphStateFactoryIteratorPtr AnimationGraphSystem::CreateStateFactoryIterator()
{
	return {};
}

bool AnimationGraphSystem::TrialAnimationGraph(const char* name, XmlNodeRef animGraph, bool loadBinary)
{
	return {};
}

void AnimationGraphSystem::FindDeadInputValues(IAnimationGraphDeadInputReportCallback* pCallback, XmlNodeRef animGraph)
{
}

void AnimationGraphSystem::SetAnimationGraphActivation(EntityId id, bool bActivation)
{
}

IAnimationGraphStateIteratorPtr AnimationGraphSystem::CreateStateIterator(EntityId id)
{
	return {};
}

IAnimationGraphInputsPtr AnimationGraphSystem::RetrieveInputs(EntityId id)
{
	return {};
}

void AnimationGraphSystem::ExportXMLToBinary(const char* filename, XmlNodeRef animGraph)
{
}

////////////////////////////////////////////////////////////////////////////////
