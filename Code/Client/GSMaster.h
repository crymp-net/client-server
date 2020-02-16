/**
 * @file
 * @brief GameSpy master server hostname hook.
 */

#pragma once

#include <string>

#include "Library/DLL.h"

struct ICVar;

typedef void *(__stdcall *TGetHostByName)(const char *name);

class GSMaster
{
	std::string m_hostname;
	ICVar *m_pHostnameCVar;
	DLL m_libGetHostByName;
	TGetHostByName m_pGetHostByName;

	static void* __stdcall CryNetwork_gethostbyname_hook(const char *name);
	static void OnHostnameCVarChanged(ICVar *pCVar);

public:
	GSMaster()
	: m_hostname(),
	  m_pHostnameCVar(),
	  m_libGetHostByName(),
	  m_pGetHostByName()
	{
	}

	bool init();

	const std::string & getHostname() const
	{
		return m_hostname;
	}
};
