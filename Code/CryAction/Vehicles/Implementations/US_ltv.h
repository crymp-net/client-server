#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_ltv : public Vehicle
{
public:
	US_ltv();
	~US_ltv();

	bool Init(IGameObject* pGameObject) override;
};
