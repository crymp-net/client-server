/**
 * @file
 * @brief Implementation of CryMP player profile.
 */

#include <cstdio>

#include "CryCommon/ISystem.h"
#include "CryCommon/IGameFramework.h"
#include "CryAction/IGameRulesSystem.h"
#include "Library/StringBuffer.h"

#include "Profile.h"
#include "Client.h"

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
	url += HTTP::URLEncode(telemetry.generateUUID());
	url += "&lng=";
	url += HTTP::URLEncode(telemetry.getLocale());
	url += "&tz=";
	url += HTTP::URLEncode(std::to_string(telemetry.getTimeZoneBias()));

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
	data += HTTP::URLEncode(isStatic() ? std::string("::tr:") + std::to_string(m_id) : m_name);
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
				const char *description = HTTP::StatusCodeToString(status);
				CryLogErrorAlways("Login request failed: %d (%s)", status, description);
				return;
			}

			if (result == "FAIL")
			{
				CryLogErrorAlways("Incorrect login credentials!");
				return;
			}

			int id;
			char tokenBuffer[256];
			char nameBuffer[128];

			int count = std::sscanf(result.c_str(), "%d,%255[0-9a-zA-Z_],%127s", &id, tokenBuffer, nameBuffer);
			if (count != 3 || id <= 0)
			{
				CryLogErrorAlways("Received invalid login data!");
				return;
			}

			self->m_id = id;
			self->m_name = nameBuffer;
			self->m_token = tokenBuffer;

			if (state == LOGIN)
			{
				CryLogAlways("$3Successfully logged-in as %s (%d)", nameBuffer, id);
			}
			else if (state == TOKEN_REFRESH)
			{
				CryLogAlways("$3Profile token successfully updated");
			}

			break;
		}
		case GET_STATIC_PROFILE:
		{
			if (status != HTTP::STATUS_OK)
			{
				const char *description = HTTP::StatusCodeToString(status);
				CryLogErrorAlways("Failed to get static profile: %d (%s)", status, description);
				return;
			}

			int id;
			char keyBuffer[256];

			int count = std::sscanf(result.c_str(), "%d/%255[0-9a-zA-Z]", &id, keyBuffer);
			if (count != 2 || id <= 0)
			{
				CryLogErrorAlways("Received invalid static profile data!");
				return;
			}

			self->m_id = id;
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
		CryLogErrorAlways("Another login request is already in progress!");
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

	CryLogAlways("Obtaining new profile token...");

	doLogin();
}

void Profile::sendAuth()
{
	if (!isLoggedIn())
	{
		return;
	}

	StringBuffer<128> msg;

	msg += "!validate ";
	msg += std::to_string(m_id);
	msg += ' ';
	msg += m_token;
	msg += ' ';
	msg += m_name;

	IGameFramework *pGameFramework = gEnv->pGame->GetIGameFramework();

	IGameRules *pGameRules = pGameFramework->GetIGameRulesSystem()->GetCurrentGameRules();
	EntityId localActorID = pGameFramework->GetClientActorId();
	if (!pGameRules || !localActorID)
	{
		CryLogErrorAlways("Failed to authenticate profile on server: Unable to send chat message!");
		return;
	}

	pGameRules->SendChatMessage(eChatToTarget, localActorID, localActorID, msg.get());
}

void Profile::logout()
{
	m_id = 0;
	m_name.clear();
	m_token.clear();
	m_secret.clear();
	m_state = IDLE;
}
