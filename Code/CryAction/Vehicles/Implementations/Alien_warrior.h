#pragma once

#include "CryAction/Vehicles/Vehicle.h"

class Alien_warrior : public Vehicle
{
public:
	Alien_warrior();
	~Alien_warrior();

	bool Init(IGameObject* pGameObject) override;
};
