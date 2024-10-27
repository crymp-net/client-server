#include <cstdint>

#include "DebugHistoryManager.h"

extern std::uintptr_t CRYACTION_BASE;

DebugHistoryManager::DebugHistoryManager()
{
#ifdef BUILD_64BIT
	static_assert(sizeof(DebugHistoryManager) == 0x38);
#else
	static_assert(sizeof(DebugHistoryManager) == 0x1c);
#endif

#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x3259B0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x248E60;
#endif

	(this->*reinterpret_cast<void(DebugHistoryManager::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IDebugHistoryManager
////////////////////////////////////////////////////////////////////////////////

IDebugHistory* DebugHistoryManager::CreateHistory(const char* id, const char* name)
{
	return nullptr;
}

void DebugHistoryManager::RemoveHistory(const char* name)
{
}

IDebugHistory* DebugHistoryManager::GetHistory(const char* name)
{
	return nullptr;
}

void DebugHistoryManager::Clear()
{
}

void DebugHistoryManager::GetMemoryStatistics(ICrySizer*)
{
}

void DebugHistoryManager::Release()
{
}


void DebugHistoryManager::LayoutHelper(const char* id, const char* name, bool visible, float minout, float maxout,
	float minin, float maxin, float x, float y, float w, float h)
{
}

////////////////////////////////////////////////////////////////////////////////
