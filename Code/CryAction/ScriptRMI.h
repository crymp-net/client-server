#pragma once

class ScriptRMI
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x90] = {};
#else
	unsigned char m_data[0x48] = {};
#endif

public:
	ScriptRMI();

	static void RegisterCVars();
};
