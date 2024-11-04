#pragma once

#include <map>

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleSystem;

class VehicleIterator : public IVehicleIterator
{
	int m_refCount = 0;
	VehicleSystem* m_pVehicleSystem = nullptr;
	std::map<EntityId, IVehicle*>::iterator m_iterator;

	explicit VehicleIterator(VehicleSystem* pVehicleSystem);

public:
	static VehicleIterator* Create(VehicleSystem* pVehicleSystem);

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleIterator
	////////////////////////////////////////////////////////////////////////////////

	size_t Count() override;
	IVehicle* Next() override;
	void AddRef() override;
	void Release() override;

	////////////////////////////////////////////////////////////////////////////////
};
