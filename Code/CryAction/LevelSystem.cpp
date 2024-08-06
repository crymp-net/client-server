#include <cstdint>

#include "LevelSystem.h"

LevelSystem::LevelSystem(ISystem* pSystem, const char* levelsFolder)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3050f6b0;
#else
	std::uintptr_t ctor = 0x3050d740;
#endif

	(this->*reinterpret_cast<void(ILevelSystem::*&)(ISystem*, const char*)>(ctor))(pSystem, levelsFolder);
}

LevelSystem::~LevelSystem()
{
}

////////////////////////////////////////////////////////////////////////////////
// ILevelSystemListener
////////////////////////////////////////////////////////////////////////////////

void LevelSystem::OnLevelNotFound(const char* levelName)
{
}

void LevelSystem::OnLoadingStart(ILevelInfo* pLevel)
{
}

void LevelSystem::OnLoadingComplete(ILevel* pLevel)
{
}

void LevelSystem::OnLoadingError(ILevelInfo* pLevel, const char* error)
{
}

void LevelSystem::OnLoadingProgress(ILevelInfo* pLevel, int progressAmount)
{
}

////////////////////////////////////////////////////////////////////////////////
// ILevelSystem
////////////////////////////////////////////////////////////////////////////////

void LevelSystem::Rescan(const char* levelsFolder)
{
}

void LevelSystem::LoadRotation()
{
}

int LevelSystem::GetLevelCount()
{
	return 0;
}

ILevelInfo* LevelSystem::GetLevelInfo(int level)
{
	return nullptr;
}

ILevelInfo* LevelSystem::GetLevelInfo(const char* levelName)
{
	return nullptr;
}

std::uint64_t LevelSystem::CalcLevelChecksum(const char* levelName)
{
	return 0;
}

void LevelSystem::AddListener(ILevelSystemListener* pListener)
{
}

void LevelSystem::RemoveListener(ILevelSystemListener* pListener)
{
}

ILevel* LevelSystem::GetCurrentLevel() const
{
	return nullptr;
}

ILevel* LevelSystem::LoadLevel(const char* levelName)
{
	return nullptr;
}

bool LevelSystem::IsLevelLoaded()
{
	return false;
}

void LevelSystem::PrepareNextLevel(const char* levelName)
{
}

ILevelRotation* LevelSystem::GetLevelRotation()
{
	return nullptr;
}

float LevelSystem::GetLastLevelLoadTime()
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// ISystem::ILoadingProgressListener
////////////////////////////////////////////////////////////////////////////////

void LevelSystem::OnLoadingProgress(int steps)
{
}

////////////////////////////////////////////////////////////////////////////////
