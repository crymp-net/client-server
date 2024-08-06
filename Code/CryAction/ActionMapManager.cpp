#include <cstdint>

#include "ActionMapManager.h"

ActionMapManager::ActionMapManager(IInput* pInput)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305171e0;
#else
	std::uintptr_t ctor = 0x30514df0;
#endif

	(this->*reinterpret_cast<void(IActionMapManager::*&)(IInput*)>(ctor))(pInput);
}

ActionMapManager::~ActionMapManager()
{
}

////////////////////////////////////////////////////////////////////////////////
// IActionMapManager
////////////////////////////////////////////////////////////////////////////////

void ActionMapManager::Update()
{
}

void ActionMapManager::Reset()
{
}

void ActionMapManager::Clear()
{
}

void ActionMapManager::LoadFromXML(const XmlNodeRef& node, bool checkVersion)
{
}

void ActionMapManager::SaveToXML(const XmlNodeRef& node)
{
}

IActionMap* ActionMapManager::CreateActionMap(const char* name)
{
	return nullptr;
}

IActionMap* ActionMapManager::GetActionMap(const char* name)
{
	return nullptr;
}

IActionFilter* ActionMapManager::CreateActionFilter(const char* name, EActionFilterType type)
{
	return nullptr;
}

IActionFilter* ActionMapManager::GetActionFilter(const char* name)
{
	return nullptr;
}

IActionMapIteratorPtr ActionMapManager::CreateActionMapIterator()
{
	return {};
}

IActionFilterIteratorPtr ActionMapManager::CreateActionFilterIterator()
{
	return {};
}

void ActionMapManager::Enable(bool enable)
{
}

void ActionMapManager::EnableActionMap(const char* name, bool enable)
{
}

void ActionMapManager::EnableFilter(const char* name, bool enable)
{
}

bool ActionMapManager::IsFilterEnabled(const char* name)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// IInputEventListener
////////////////////////////////////////////////////////////////////////////////

bool ActionMapManager::OnInputEvent(const SInputEvent& event)
{
	return false;
}

bool ActionMapManager::OnInputEventUI(const SInputEvent& event)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
