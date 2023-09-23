#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_hovercraft : public Vehicle
{
public:
	US_hovercraft();
	~US_hovercraft();

	bool Init(IGameObject* pGameObject) override;
};
