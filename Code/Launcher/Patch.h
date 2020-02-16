/**
 * @file
 * @brief Functions for patching Crysis code.
 */

#pragma once

namespace Patch
{
	// CryAction
	bool AllowDX9ImmersiveMultiplayer(void *pCryAction);

	// CryNetwork
	bool AllowConnectWithoutGS(void *pCryNetwork);
	bool AllowSameCDKeys(void *pCryNetwork);

	// CrySystem
	bool RemoveSecuROM(void *pCrySystem);
	bool AllowDX9VeryHighSpec(void *pCrySystem);
	bool AllowMultipleInstances(void *pCrySystem);
	bool UnhandledExceptions(void *pCrySystem);
	bool Disable3DNow(void *pCrySystem);
}
