#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_smallboat : public Vehicle
{
public:
	US_smallboat();
	~US_smallboat();

	bool Init(IGameObject* pGameObject) override;
};
