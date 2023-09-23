#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class US_transportVTOL : public Vehicle
{
public:
	US_transportVTOL();
	~US_transportVTOL();

	bool Init(IGameObject* pGameObject) override;
};
