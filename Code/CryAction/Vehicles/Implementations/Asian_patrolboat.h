#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Asian_patrolboat : public Vehicle
{
public:
	Asian_patrolboat();
	~Asian_patrolboat();

	bool Init(IGameObject* pGameObject) override;
};
