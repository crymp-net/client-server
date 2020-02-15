/**
 * @file
 * @brief Implementation of functions for patching Crysis code.
 */

#include "Patch.h"
#include "Util.h"

using Util::RVA;
using Util::FillNOP;
using Util::FillMem;

/**
 * @brief Allows connecting to DX10 servers with game running in DX9 mode.
 * @param pCryAction CryAction DLL handle.
 * @return True if no error occurred, otherwise false.
 */
bool Patch::AllowDX9ImmersiveMultiplayer(void *pCryAction)
{
#ifdef BUILD_64BIT
	if (!FillNOP(RVA(pCryAction, 0x2B394D), 0x1E)
	 || !FillNOP(RVA(pCryAction, 0x2B6860), 0x1A))
#else
	if (!FillNOP(RVA(pCryAction, 0x1D698A), 0x1A)
	 || !FillNOP(RVA(pCryAction, 0x1D89FC), 0x15))
#endif
	{
		return false;
	}

	return true;
}

/**
 * @brief Prevents server from kicking players with the same CD key.
 * This is server-side patch.
 * @param pCryNetwork CryNetwork DLL handle.
 * @return True if no error occurred, otherwise false.
 */
bool Patch::AllowSameCDKeys(void *pCryNetwork)
{
#ifdef BUILD_64BIT
	if (!FillNOP(RVA(pCryNetwork, 0xE0328), 0x47))
#else
	if (!FillNOP(RVA(pCryNetwork, 0x606A5), 0x4))
#endif
	{
		return false;
	}

	return true;
}

/**
 * @brief Removes the SecuROM crap from 64-bit CrySystem.
 * It does nothing in 32-bit build.
 * @param pCrySystem CrySystem DLL handle.
 * @return True if no error occurred, otherwise false.
 */
bool Patch::RemoveSecuROM(void *pCrySystem)
{
#ifdef BUILD_64BIT
	if (!FillNOP(RVA(pCrySystem, 0x470B9), 0x16))
	{
		return false;
	}
#endif

	return true;
}

/**
 * @brief Allows Very High settings in DX9 mode.
 * @param pCrySystem CrySystem DLL handle.
 * @return True if no error occurred, otherwise false.
 */
bool Patch::AllowDX9VeryHighSpec(void *pCrySystem)
{
#ifdef BUILD_64BIT
	if (!FillNOP(RVA(pCrySystem, 0x4674C), 0x54))
#else
	if (!FillNOP(RVA(pCrySystem, 0x59DA8), 0x4B))
#endif
	{
		return false;
	}

	return true;
}

/**
 * @brief Allows running multiple instances of Crysis at once.
 * Note that the first check if any instance is already running is normally done in launcher.
 * @param pCrySystem CrySystem DLL handle.
 * @return True if no error occurred, otherwise false.
 */
bool Patch::AllowMultipleInstances(void *pCrySystem)
{
#ifdef BUILD_64BIT
	if (!FillNOP(RVA(pCrySystem, 0x42BFF), 0x68))
#else
	if (!FillNOP(RVA(pCrySystem, 0x5794F), 0x58))
#endif
	{
		return false;
	}

	return true;
}

/**
 * @brief Disables custom handling of unhandled exceptions.
 * This patch prevents the engine from enabling its own unhandled exception callback via SetUnhandledExceptionFilter function.
 * The callback is used to do various things after crash, such as calling ReportFault function or creating minidump. It depends
 * on value of sys_WER cvar (0, 1, 2). However, everything done in this callback is completely useless and causes only problems.
 * Even MSDN documentation recommends that applications shouldn't try do such things at their own. Instead, they should let
 * operating system handle fatal errors for them.
 * @param pCrySystem CrySystem DLL handle.
 * @return True if no error occurred, otherwise false.
 */
bool Patch::UnhandledExceptions(void *pCrySystem)
{
#ifdef BUILD_64BIT
	if (!FillNOP(RVA(pCrySystem, 0x22946), 0x6)
	 || !FillNOP(RVA(pCrySystem, 0x22952), 0x7)
	 || !FillNOP(RVA(pCrySystem, 0x467A5), 0x16))
#else
	if (!FillNOP(RVA(pCrySystem, 0x17D67), 0x5)
	 || !FillNOP(RVA(pCrySystem, 0x17D72), 0xC)
	 || !FillNOP(RVA(pCrySystem, 0x59DF8), 0x13))
#endif
	{
		return false;
	}

	return true;
}

/**
 * @brief Disables use of 3DNow! instructions.
 * This patch correctly fixes the well known crash of 32-bit Crysis on modern AMD processors.
 * @param pCrySystem CrySystem DLL handle.
 * @return True if no error occurred, otherwise false.
 */
bool Patch::Disable3DNow(void *pCrySystem)
{
	const unsigned char flags = 0x18;  // default processor feature flags without CPUF_3DNOW flag

#ifdef BUILD_64BIT
	if (!FillMem(RVA(pCrySystem, 0xA0FF), &flags, sizeof flags))
#else
	if (!FillMem(RVA(pCrySystem, 0x93E2), &flags, sizeof flags))
#endif
	{
		return false;
	}

	return true;
}
