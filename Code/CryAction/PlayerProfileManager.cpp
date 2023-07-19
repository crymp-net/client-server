#include <cstdint>

#include "PlayerProfileManager.h"

PlayerProfileManager::PlayerProfileManager()
{
	struct Something
	{
		void* reserved[3] = {};

		Something()
		{
#ifdef BUILD_64BIT
			std::uintptr_t ctor = 0x30787c90;
#else
			std::uintptr_t ctor = 0x306bcf20;
#endif

			(this->*reinterpret_cast<void(Something::*&)()>(ctor))();
		}
	};

#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30798820;
#else
	std::uintptr_t ctor = 0x306c5010;
#endif

	(this->*reinterpret_cast<void(PlayerProfileManager::*&)(Something*)>(ctor))(new Something());
}

PlayerProfileManager::~PlayerProfileManager()
{
}

////////////////////////////////////////////////////////////////////////////////
// IPlayerProfileManager
////////////////////////////////////////////////////////////////////////////////

bool PlayerProfileManager::Initialize()
{
	return {};
}

bool PlayerProfileManager::Shutdown()
{
	return {};
}

void PlayerProfileManager::GetMemoryStatistics(ICrySizer*)
{
}

int PlayerProfileManager::GetUserCount()
{
	return {};
}

bool PlayerProfileManager::GetUserInfo(int index, IPlayerProfileManager::SUserInfo& outInfo)
{
	return {};
}

bool PlayerProfileManager::LoginUser(const char* userId, bool& outFirstTime)
{
	return {};
}

bool PlayerProfileManager::LogoutUser(const char* userId)
{
	return {};
}

int PlayerProfileManager::GetProfileCount(const char* userId)
{
	return {};
}

bool PlayerProfileManager::GetProfileInfo(const char* userId, int index, IPlayerProfileManager::SProfileDescription& outInfo)
{
	return {};
}

bool PlayerProfileManager::CreateProfile(const char* userId, const char* profileName, bool overrideIfPresent, EProfileOperationResult& result)
{
	return {};
}

bool PlayerProfileManager::DeleteProfile(const char* userId, const char* profileName, EProfileOperationResult& result)
{
	return {};
}

bool PlayerProfileManager::RenameProfile(const char* userId, const char* newName, EProfileOperationResult& result)
{
	return {};
}

bool PlayerProfileManager::SaveProfile(const char* userId, EProfileOperationResult& result)
{
	return {};
}

IPlayerProfile* PlayerProfileManager::ActivateProfile(const char* userId, const char* profileName)
{
	return {};
}

IPlayerProfile* PlayerProfileManager::GetCurrentProfile(const char* userId)
{
	return {};
}

const char* PlayerProfileManager::GetCurrentUser()
{
	return {};
}

bool PlayerProfileManager::ResetProfile(const char* userId)
{
	return {};
}

IPlayerProfile* PlayerProfileManager::GetDefaultProfile()
{
	return {};
}

const IPlayerProfile* PlayerProfileManager::PreviewProfile(const char* userId, const char* profileName)
{
	return {};
}

void PlayerProfileManager::SetSharedSaveGameFolder(const char* sharedSaveGameFolder)
{
}

const char* PlayerProfileManager::GetSharedSaveGameFolder()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
