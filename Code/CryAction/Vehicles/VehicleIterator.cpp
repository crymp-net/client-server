#include "VehicleIterator.h"
#include "VehicleSystem.h"

VehicleIterator::VehicleIterator(VehicleSystem* pVehicleSystem) : m_pVehicleSystem(pVehicleSystem)
{
	// TODO: this might get invalidated by std::map::erase -> unsafe
	m_iterator = pVehicleSystem->m_vehicles.begin();
}

VehicleIterator* VehicleIterator::Create(VehicleSystem* pVehicleSystem)
{
	// TODO: no smart pointer -> unsafe
	return new VehicleIterator(pVehicleSystem);
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleIterator
////////////////////////////////////////////////////////////////////////////////

size_t VehicleIterator::Count()
{
	return m_pVehicleSystem->m_vehicles.size();
}

IVehicle* VehicleIterator::Next()
{
	if (m_iterator == m_pVehicleSystem->m_vehicles.end())
	{
		return nullptr;
	}

	IVehicle* pVehicle = m_iterator->second;
	++m_iterator;

	return pVehicle;
}

void VehicleIterator::AddRef()
{
	++m_refCount;
}

void VehicleIterator::Release()
{
	if (--m_refCount <= 0)
	{
		delete this;
	}
}

////////////////////////////////////////////////////////////////////////////////
