#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Asian_helicopter : public Vehicle
{
public:
	Asian_helicopter();
	~Asian_helicopter();

	bool Init(IGameObject* pGameObject) override;
};
