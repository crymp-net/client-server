/**
 * @file
 * @brief Implementation of CryMP player profile.
 */

#include <cstdio>

#include "Library/StringBuffer.h"

#include "Profile.h"
#include "Client.h"
#include "Log.h"

void Profile::sendRequest(const char *urlPath, const char *urlParams, const char *data)
{
	StringBuffer<1024> url;

	url += "https://crymp.net";
	url += urlPath;
	url += "?";

	if (urlParams)
	{
		url += urlParams;
		url += "&";
	}

	Telemetry & telemetry = Client::GetTelemetry();

	url += "hwid=";
	url += telemetry.getHWID();
	url += "&lng=";
	url += telemetry.getLocale();
	url += "&tz=";
	url += std::to_string(telemetry.getTimeZoneBias());

	if (data)
	{
		Client::GetHTTPClient().POST(url.get(), data, "application/x-www-form-urlencoded", RequestCallback, this);
	}
	else
	{
		Client::GetHTTPClient().GET(url.get(), RequestCallback, this);
	}
}

void Profile::doLogin()
{
	StringBuffer<256> data;

	data += "a=";
	data += HTTP::URLEncode(m_name);
	data += "&b=";
	data += HTTP::URLEncode(m_secret);

	sendRequest("/api/login", nullptr, data.get());
}

void Profile::RequestCallback(int status, const std::string & result, void *param)
{
	Profile *self = static_cast<Profile*>(param);

	EState state = self->m_state;

	self->m_state = IDLE;

	switch (state)
	{
		case IDLE:
		{
			// logout called during login request
			break;
		}
		case LOGIN:
		case TOKEN_REFRESH:
		{
			if (status != HTTP::STATUS_OK)
			{
				LogError("Login request failed: %d (%s)", status, HTTP::StatusCodeToString(status));
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

			if (state == LOGIN)
			{
				LogInfo("$3Successfully logged-in as %s (%d)", nameBuffer, id);
			}
			else if (state == TOKEN_REFRESH)
			{
				LogInfo("$3Profile token successfully updated");
			}

			break;
		}
		case GET_STATIC_PROFILE:
		{
			if (status != HTTP::STATUS_OK)
			{
				LogError("Failed to get static profile: %d (%s)", status, HTTP::StatusCodeToString(status));
				return;
			}

			int id;
			char keyBuffer[256];

			int count = std::sscanf(result.c_str(), "%d/%255[0-9a-zA-Z]", &id, keyBuffer);
			if (count != 2 || id <= 0)
			{
				LogError("Received invalid static profile data!");
				return;
			}

			self->m_id = id;
			self->m_name = "::tr:";
			self->m_name += std::to_string(id);
			self->m_secret = keyBuffer;

			self->m_state = LOGIN;

			self->doLogin();

			break;
		}
	}
}

void Profile::initStatic()
{
	if (m_state != IDLE)
	{
		return;
	}

	logout();

	m_state = GET_STATIC_PROFILE;

	sendRequest("/api/id", nullptr, nullptr);
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

	doLogin();
}

void Profile::refreshToken()
{
	if (!isLoggedIn() || m_state != IDLE)
	{
		return;
	}

	m_state = TOKEN_REFRESH;

	LogInfo("Obtaining new profile token...");

	doLogin();
}

void Profile::logout()
{
	m_id = 0;
	m_name.clear();
	m_token.clear();
	m_secret.clear();
	m_state = IDLE;
}
