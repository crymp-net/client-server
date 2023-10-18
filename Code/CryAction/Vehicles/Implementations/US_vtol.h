#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_vtol : public Vehicle
{
public:
	US_vtol();
	~US_vtol();

	bool Init(IGameObject* pGameObject) override;
};
