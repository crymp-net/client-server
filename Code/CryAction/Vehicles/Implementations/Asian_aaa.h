#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Asian_aaa : public Vehicle
{
public:
	Asian_aaa();
	~Asian_aaa();

	bool Init(IGameObject* pGameObject) override;
};
