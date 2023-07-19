#pragma once

class TimeOfDayScheduler
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x28] = {};
#else
	unsigned char m_data[0x18] = {};
#endif

public:
	TimeOfDayScheduler();
};
