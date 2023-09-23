#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Asian_apc : public Vehicle
{
public:
	Asian_apc();
	~Asian_apc();

	bool Init(IGameObject* pGameObject) override;
};
