#pragma once

struct ISystem;

class TimeDemoRecorder
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x180] = {};
#else
	unsigned char m_data[0x128] = {};
#endif

public:
	explicit TimeDemoRecorder(ISystem* pSystem);
};
