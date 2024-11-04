#pragma once

#include "CryCommon/CryAction/IGameObject.h"
#include "CryCommon/CryEntitySystem/IEntityProxy.h"

class GameObject : public IEntityProxy, public IGameObject
{
public:
	static void RegisterCVars();

	static void UpdateSchedulingProfiles();
};
