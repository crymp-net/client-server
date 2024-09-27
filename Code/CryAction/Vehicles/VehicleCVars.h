#pragma once

class VehicleCVars
{
	unsigned char m_data[0x88] = {};

public:
	VehicleCVars();
};

static_assert(sizeof(VehicleCVars) == 0x88);
