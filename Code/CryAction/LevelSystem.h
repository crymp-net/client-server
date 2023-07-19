#pragma once

#include "CryCommon/CryAction/ILevelSystem.h"
#include "CryCommon/CrySystem/ISystem.h"

class LevelSystem : public ILevelSystem, public ISystem::ILoadingProgressListener
{
#ifdef BUILD_64BIT
	unsigned char m_data[0xc8 - 0x10] = {};
#else
	unsigned char m_data[0x78 - 0x8] = {};
#endif

public:
	explicit LevelSystem(ISystem* pSystem, const char* levelsFolder);
	virtual ~LevelSystem();

	////////////////////////////////////////////////////////////////////////////////
	// ILevelSystemListener
	////////////////////////////////////////////////////////////////////////////////

	void OnLevelNotFound(const char* levelName) override;
	void OnLoadingStart(ILevelInfo* pLevel) override;
	void OnLoadingComplete(ILevel* pLevel) override;
	void OnLoadingError(ILevelInfo* pLevel, const char* error) override;
	void OnLoadingProgress(ILevelInfo* pLevel, int progressAmount) override;

	////////////////////////////////////////////////////////////////////////////////
	// ILevelSystem
	////////////////////////////////////////////////////////////////////////////////

	void Rescan(const char* levelsFolder) override;
	void LoadRotation() override;
	int GetLevelCount() override;
	ILevelInfo* GetLevelInfo(int level) override;
	ILevelInfo* GetLevelInfo(const char* levelName) override;
	std::uint64_t CalcLevelChecksum(const char* levelName) override;

	void AddListener(ILevelSystemListener* pListener) override;
	void RemoveListener(ILevelSystemListener* pListener) override;

	ILevel* GetCurrentLevel() const override;
	ILevel* LoadLevel(const char* levelName) override;
	bool IsLevelLoaded() override;
	void PrepareNextLevel(const char* levelName) override;

	ILevelRotation* GetLevelRotation() override;

	float GetLastLevelLoadTime() override;

	////////////////////////////////////////////////////////////////////////////////
	// ISystem::ILoadingProgressListener
	////////////////////////////////////////////////////////////////////////////////

	void OnLoadingProgress(int steps) override;

	////////////////////////////////////////////////////////////////////////////////
};
