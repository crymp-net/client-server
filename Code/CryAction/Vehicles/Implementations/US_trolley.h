#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_trolley : public Vehicle
{
public:
	US_trolley();
	~US_trolley();

	bool Init(IGameObject* pGameObject) override;
};
