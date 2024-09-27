#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_apc : public Vehicle
{
public:
	US_apc();
	~US_apc();

	bool Init(IGameObject* pGameObject) override;
};
