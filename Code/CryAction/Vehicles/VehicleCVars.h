#pragma once

struct VehicleCVars
{
	unsigned char data[0x88] = {};

	VehicleCVars();
};

static_assert(sizeof(VehicleCVars) == 0x88);
