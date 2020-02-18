/**
 * @file
 * @brief CryMP Client.
 */

#pragma once

#include "CryCommon/IGameFramework.h"
#include "CryAction/ILevelSystem.h"

#include "GSMaster.h"
#include "HTTPClient.h"
#include "Profile.h"
#include "Telemetry.h"

enum struct EProfile
{
	AUTO, USER, STATIC
};

class Client : public IGameFrameworkListener, public ILevelSystemListener
{
	GSMaster m_GSMaster;
	HTTPClient m_HTTPClient;
	Profile m_userProfile;
	Profile m_staticProfile;
	Telemetry m_telemetry;

	static Client *s_pInstance;

public:
	Client()
	: m_GSMaster(),
	  m_HTTPClient(),
	  m_userProfile(),
	  m_staticProfile(),
	  m_telemetry()
	{
		s_pInstance = this;
	}

	bool init();

	// IGameFrameworkListener
	void OnPostUpdate(float deltaTime) override;
	void OnSaveGame(ISaveGame *pSaveGame) override;
	void OnLoadGame(ILoadGame *pLoadGame) override;
	void OnLevelEnd(const char *nextLevel) override;
	void OnActionEvent(const SActionEvent & event) override;

	// ILevelSystemListener
	void OnLevelNotFound(const char *levelName) override;
	void OnLoadingStart(ILevelInfo *pLevel) override;
	void OnLoadingComplete(ILevel *pLevel) override;
	void OnLoadingError(ILevelInfo *pLevel, const char *error) override;
	void OnLoadingProgress(ILevelInfo *pLevel, int progressAmount) override;

	static GSMaster & GetGSMaster()
	{
		return s_pInstance->m_GSMaster;
	}

	static HTTPClient & GetHTTPClient()
	{
		return s_pInstance->m_HTTPClient;
	}

	static Profile & GetProfile(EProfile type = EProfile::AUTO)
	{
		Profile & userProfile = s_pInstance->m_userProfile;
		Profile & staticProfile = s_pInstance->m_staticProfile;

		switch (type)
		{
			case EProfile::AUTO:   break;
			case EProfile::USER:   return userProfile;
			case EProfile::STATIC: return staticProfile;
		}

		return userProfile.isLoggedIn() ? userProfile : staticProfile;
	}

	static Telemetry & GetTelemetry()
	{
		return s_pInstance->m_telemetry;
	}
};
