#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Civ_speedboat : public Vehicle
{
public:
	Civ_speedboat();
	~Civ_speedboat();

	bool Init(IGameObject* pGameObject) override;
};
