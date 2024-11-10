#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "CryCommon/CryAction/IPlayerProfiles.h"
#include "CryCommon/CrySystem/IConsole.h"

class PlayerProfile;

class PlayerProfileManager final : public IPlayerProfileManager
{
	std::vector<std::string> m_profileNames;
	std::unique_ptr<PlayerProfile> m_currentProfile;
	std::unique_ptr<PlayerProfile> m_previewProfile;

	ICVar* m_pRichSaveGamesCVar = nullptr;
	ICVar* m_pRSFDebugWriteCVar = nullptr;
	ICVar* m_pRSFDebugWriteOnLoadCVar = nullptr;

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
	bool GetUserInfo(int index, SUserInfo& userInfo) override;
	bool LoginUser(const char* userId, bool& firstTime) override;
	bool LogoutUser(const char* userId) override;

	int GetProfileCount(const char* userId) override;
	bool GetProfileInfo(const char* userId, int index, SProfileDescription& profileInfo) override;

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

private:
	void LoadProfileNames();

	static void DumpActionMapsCommand(IConsoleCmdArgs* args);
};
