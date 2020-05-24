/**
 * @file
 * @brief Implementation of CryMP client telemetry services.
 */

#include <cctype>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CryCommon/ISystem.h"
#include "Library/PicoSHA2.h"

#include "Telemetry.h"

static std::string GetMachineGUID()
{
	const char *keyName = "SOFTWARE\\Microsoft\\Cryptography";
	const char *valueName = "MachineGuid";

	HKEY key;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyName, 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key))
	{
		return std::string();
	}

	char buffer[256];
	DWORD length = static_cast<DWORD>(sizeof buffer);
	if (RegQueryValueExA(key, valueName, NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &length) || length == 0)
	{
		return std::string();
	}

	length--;  // skip terminating null character

	return std::string(buffer, length);
}

bool Telemetry::init()
{
	std::string machineGUID = GetMachineGUID();
	if (!machineGUID.empty())
	{
		// use SHA-256 hash of machine GUID as HWID
		m_hwid = picosha2::hash256_hex_string(machineGUID);

		// uppercase
		std::transform(m_hwid.begin(), m_hwid.end(), m_hwid.begin(), std::toupper);
	}
	else
	{
		CryLogErrorAlways("Telemetry: Failed to get machine GUID - error code %lu", GetLastError());
		m_hwid.clear();
	}

	char localeNameBuffer[LOCALE_NAME_MAX_LENGTH];
	if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SNAME, localeNameBuffer, LOCALE_NAME_MAX_LENGTH))
	{
		m_locale = localeNameBuffer;
	}
	else
	{
		CryLogErrorAlways("Telemetry: Failed to get locale information - error code %lu", GetLastError());
		m_locale.clear();
	}

	TIME_ZONE_INFORMATION tz;
	if (GetTimeZoneInformation(&tz) != TIME_ZONE_ID_INVALID)
	{
		m_tzBias = tz.Bias;
	}
	else
	{
		CryLogErrorAlways("Telemetry: Failed to get time zone information - error code %lu", GetLastError());
		m_tzBias = 0;
	}

	return true;
}

std::string Telemetry::generateUUID(const char *salt) const
{
	if (!salt)
	{
		salt = "idsvc";
	}

	if (m_hwid.empty())
	{
		return std::string();
	}

	std::string result = picosha2::hash256_hex_string(m_hwid + salt);

	// uppercase
	std::transform(result.begin(), result.end(), result.begin(), std::toupper);

	return m_hwid + ':' + result;
}
