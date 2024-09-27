#include <cstdint>

#include "ViewSystem.h"

extern std::uintptr_t CRYACTION_BASE;

ViewSystem::ViewSystem(ISystem* pSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x1d330;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x19bc0;
#endif

	(this->*reinterpret_cast<void(IViewSystem::*&)(ISystem*)>(ctor))(pSystem);
}

void ViewSystem::Update(float frameTime)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1c340;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x18fe0;
#endif

	(this->*reinterpret_cast<void(IViewSystem::*&)(float)>(func))(frameTime);
}

////////////////////////////////////////////////////////////////////////////////
// IViewSystem
////////////////////////////////////////////////////////////////////////////////

IView* ViewSystem::CreateView()
{
	return nullptr;
}

void ViewSystem::SetActiveView(IView* pView)
{
}

void ViewSystem::SetActiveView(unsigned int viewId)
{
}

IView* ViewSystem::GetView(unsigned int viewId)
{
	return nullptr;
}

IView* ViewSystem::GetActiveView()
{
	return nullptr;
}

unsigned int ViewSystem::GetViewId(IView* pView)
{
	return 0;
}

unsigned int ViewSystem::GetActiveViewId()
{
	return 0;
}

IView* ViewSystem::GetViewByEntityId(unsigned int id, bool forceCreate)
{
	return nullptr;
}

bool ViewSystem::AddListener(IViewSystemListener* pListener)
{
	return false;
}

bool ViewSystem::RemoveListener(IViewSystemListener* pListener)
{
	return false;
}

void ViewSystem::Serialize(TSerialize ser)
{
}

float ViewSystem::GetDefaultZNear()
{
	return 0;
}

void ViewSystem::SetBlendParams(float blendPosSpeed, float blendRotSpeed, bool performBlendOut)
{
}

void ViewSystem::SetOverrideCameraRotation(bool enable, Quat rotation)
{
}

bool ViewSystem::IsPlayingCutScene() const
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// IMovieUser
////////////////////////////////////////////////////////////////////////////////

void ViewSystem::SetActiveCamera(const SCameraParams& params)
{
}

void ViewSystem::BeginCutScene(IAnimSequence* pSeq, unsigned long flags, bool resetFX)
{
}

void ViewSystem::EndCutScene(IAnimSequence* pSeq)
{
}

void ViewSystem::SendGlobalEvent(const char* event)
{
}

void ViewSystem::PlaySubtitles(IAnimSequence* pSeq, ISound* pSound)
{
}

////////////////////////////////////////////////////////////////////////////////
