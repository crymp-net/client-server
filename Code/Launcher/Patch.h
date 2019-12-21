/**
 * @file
 * @brief Functions for patching Crysis code.
 */

#pragma once

namespace Patch
{
	// CryAction
	int AllowDX9ImmersiveMultiplayer(void *pCryAction);

	// CryNetwork
	int AllowSameCDKeys(void *pCryNetwork);

	// CrySystem
	int RemoveSecuROM(void *pCrySystem);
	int AllowDX9VeryHighSpec(void *pCrySystem);
	int AllowMultipleInstances(void *pCrySystem);
	int UnhandledExceptions(void *pCrySystem);
	int Disable3DNow(void *pCrySystem);
}
