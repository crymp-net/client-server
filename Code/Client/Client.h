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

struct ICVar;

class Client : public IGameFrameworkListener, public ILevelSystemListener
{
	GSMaster m_GSMaster;
	HTTPClient m_HTTPClient;
	Profile m_profile;
	ICVar *m_pClAutoValidateCVar;

	static Client *s_pInstance;

public:
	Client()
	: m_GSMaster(),
	  m_HTTPClient(),
	  m_profile(),
	  m_pClAutoValidateCVar()
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

	static Profile & GetProfile()
	{
		return s_pInstance->m_profile;
	}
};
