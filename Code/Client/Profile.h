/**
 * @file
 * @brief CryMP player profile.
 */

#pragma once

#include <string>

enum struct EProfile
{
	AUTO, USER, STATIC
};

class Profile
{
	enum EState
	{
		IDLE, LOGIN, TOKEN_REFRESH, GET_STATIC_PROFILE
	};

	int m_id;
	std::string m_name;
	std::string m_token;
	std::string m_secret;
	EState m_state;

	void sendRequest(const char *urlPath, const char *urlParams, const char *data);
	void doLogin();

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

	void initStatic();
	void login(const char *name, const char *secret);
	void refreshToken();
	void sendAuth();
	void logout();

	bool isLoggedIn() const
	{
		return m_id > 0;
	}

	bool isStatic() const
	{
		return m_id >= 1000000;
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
