#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Asian_tank : public Vehicle
{
public:
	Asian_tank();
	~Asian_tank();

	bool Init(IGameObject* pGameObject) override;
};
