#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class DefaultVehicle : public Vehicle
{
public:
	DefaultVehicle();
	~DefaultVehicle();

	bool Init(IGameObject* pGameObject) override;
};
