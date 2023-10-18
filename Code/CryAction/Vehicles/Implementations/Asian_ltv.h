#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Asian_ltv : public Vehicle
{
public:
	Asian_ltv();
	~Asian_ltv();

	bool Init(IGameObject* pGameObject) override;
};
