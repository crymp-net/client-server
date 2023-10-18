#pragma once

#include "CryCommon/CryAction/IPlayerProfiles.h"

class PlayerProfileManager : public IPlayerProfileManager
{
	void* reserved[9 - 1] = {};

public:
	PlayerProfileManager();
	virtual ~PlayerProfileManager();

	////////////////////////////////////////////////////////////////////////////////
	// IPlayerProfileManager
	////////////////////////////////////////////////////////////////////////////////

	bool Initialize() override;
	bool Shutdown() override;

	void GetMemoryStatistics(ICrySizer*) override;

	int GetUserCount() override;
	bool GetUserInfo(int index, IPlayerProfileManager::SUserInfo& outInfo) override;
	bool LoginUser(const char* userId, bool& outFirstTime) override;
	bool LogoutUser(const char* userId) override;

	int GetProfileCount(const char* userId) override;
	bool GetProfileInfo(const char* userId, int index, IPlayerProfileManager::SProfileDescription& outInfo) override;

	bool CreateProfile(const char* userId, const char* profileName, bool overrideIfPresent, EProfileOperationResult& result) override;
	bool DeleteProfile(const char* userId, const char* profileName, EProfileOperationResult& result) override;
	bool RenameProfile(const char* userId, const char* newName, EProfileOperationResult& result) override;
	bool SaveProfile(const char* userId, EProfileOperationResult& result) override;
	IPlayerProfile* ActivateProfile(const char* userId, const char* profileName) override;
	IPlayerProfile* GetCurrentProfile(const char* userId) override;
	const char* GetCurrentUser() override;
	bool ResetProfile(const char* userId) override;
	IPlayerProfile* GetDefaultProfile() override;
	const IPlayerProfile* PreviewProfile(const char* userId, const char* profileName) override;

	void SetSharedSaveGameFolder(const char* sharedSaveGameFolder) override;
	const char* GetSharedSaveGameFolder() override;

	////////////////////////////////////////////////////////////////////////////////
};
