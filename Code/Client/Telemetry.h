/**
 * @file
 * @brief CryMP client telemetry services.
 */

#pragma once

#include <string>

class Telemetry
{
	std::string m_hwid;
	std::string m_locale;
	int m_tzBias;

public:
	Telemetry()
	: m_hwid(),
	  m_locale(),
	  m_tzBias()
	{
	}

	bool init();

	const std::string & getHWID() const
	{
		return m_hwid;
	}

	const std::string & getLocale() const
	{
		return m_locale;
	}

	int getTimeZoneBias() const
	{
		return m_tzBias;
	}

	std::string generateUUID(const char *salt) const;
};
