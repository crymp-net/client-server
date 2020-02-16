/**
 * @file
 * @brief CryMP player profile.
 */

#pragma once

#include <string>

class Profile
{
	enum EState
	{
		IDLE, LOGIN, TOKEN_REFRESH
	};

	int m_id;
	std::string m_name;
	std::string m_token;
	std::string m_secret;
	EState m_state;

	static void RequestCallback(int status, const std::string & result, void *param);

public:
	Profile()
	: m_id(),
	  m_name(),
	  m_token(),
	  m_secret(),
	  m_state(IDLE)
	{
	}

	void login(const char *name, const char *secret);
	void refreshToken();
	void logout();

	bool isLoggedIn() const
	{
		return m_id > 0;
	}

	int getID() const
	{
		return m_id;
	}

	const std::string & getName() const
	{
		return m_name;
	}

	const std::string & getToken() const
	{
		return m_token;
	}
};
