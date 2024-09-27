#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Asian_truck : public Vehicle
{
public:
	Asian_truck();
	~Asian_truck();

	bool Init(IGameObject* pGameObject) override;
};
