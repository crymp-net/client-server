/**
 * @file
 * @brief Implementation of CryMP player profile.
 */

#include <cstdio>

#include "Library/StringBuffer.h"

#include "Profile.h"
#include "Client.h"
#include "Log.h"

void Profile::RequestCallback(int status, const std::string & result, void *param)
{
	Profile *self = static_cast<Profile*>(param);

	if (self->m_state == IDLE)
	{
		// logout called during login request
		return;
	}

	bool isLogin = (self->m_state == LOGIN);

	self->m_state = IDLE;

	if (status != HTTP::STATUS_OK)
	{
		LogError("Login request failed with status %d (%s)", status, HTTP::StatusCodeToString(status));
		return;
	}

	if (result == "FAIL")
	{
		LogError("Incorrect login credentials!");
		return;
	}

	int id;
	char tokenBuffer[256];
	char nameBuffer[128];

	int count = std::sscanf(result.c_str(), "%d,%255[0-9a-zA-Z_],%127s", &id, tokenBuffer, nameBuffer);
	if (count != 3 || id <= 0)
	{
		LogError("Received invalid login data!");
		return;
	}

	self->m_id = id;
	self->m_name = nameBuffer;
	self->m_token = tokenBuffer;

	if (isLogin)
	{
		LogInfo("$3Successfully logged-in as %s (%d)", nameBuffer, id);
	}
	else
	{
		LogInfo("$3Profile token successfully updated");
	}
}

void Profile::login(const char *name, const char *secret)
{
	if (m_state != IDLE)
	{
		LogError("Another login request is already in progress!");
		return;
	}

	logout();

	m_name = name;
	m_secret = secret;

	m_state = LOGIN;

	StringBuffer<1024> url;

	url += "https://crymp.net/api/login_svc.php?a=";
	url += m_name;
	url += "&b=";
	url += m_secret;

	Client::GetHTTPClient().GET(url.get(), RequestCallback, this);
}

void Profile::refreshToken()
{
	if (!isLoggedIn() || m_state != IDLE)
	{
		return;
	}

	m_state = TOKEN_REFRESH;

	LogInfo("Obtaining new profile token...");

	StringBuffer<1024> url;

	url += "https://crymp.net/api/login_svc.php?a=";
	url += m_name;
	url += "&b=";
	url += m_secret;

	Client::GetHTTPClient().GET(url.get(), RequestCallback, this);
}

void Profile::logout()
{
	m_id = 0;
	m_name.clear();
	m_token.clear();
	m_secret.clear();
	m_state = IDLE;
}
