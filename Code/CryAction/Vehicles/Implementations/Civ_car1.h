#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Civ_car1 : public Vehicle
{
public:
	Civ_car1();
	~Civ_car1();

	bool Init(IGameObject* pGameObject) override;
};
