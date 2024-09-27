#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_tank : public Vehicle
{
public:
	US_tank();
	~US_tank();

	bool Init(IGameObject* pGameObject) override;
};
