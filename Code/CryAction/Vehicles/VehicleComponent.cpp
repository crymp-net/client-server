#include <algorithm>

#include "VehicleComponent.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleComponent::VehicleComponent()
{
#ifdef BUILD_64BIT
	static_assert(sizeof(VehicleComponent) == 0xd0);
	static_assert(offsetof(VehicleComponent, m_isMajorComponent) == 0xb2);
	static_assert(offsetof(VehicleComponent, m_vehicleHealthProportion) == 0xb4);
#else
	static_assert(sizeof(VehicleComponent) == 0x8c);
	static_assert(offsetof(VehicleComponent, m_isMajorComponent) == 0x6e);
	static_assert(offsetof(VehicleComponent, m_vehicleHealthProportion) == 0x70);
#endif

#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x8fc90;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x67fa0;
#endif

	(this->*reinterpret_cast<void(VehicleComponent::*&)()>(ctor))();
}

bool VehicleComponent::Init(IVehicle* pVehicle, const SmartScriptTable& component)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x8f2d0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x679e0;
#endif

	return (this->*reinterpret_cast<bool(VehicleComponent::*&)(IVehicle*, const SmartScriptTable&)>(func))(
		pVehicle, component
	);
}

void VehicleComponent::AddPart(IVehiclePart* pPart)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x8ecd0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x678c0;
#endif

	(this->*reinterpret_cast<void(VehicleComponent::*&)(IVehiclePart*)>(func))(pPart);
}

void VehicleComponent::SetVehicleHealthProportion(float proportion)
{
	m_vehicleHealthProportion = std::clamp(proportion, 0.0f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleComponent
////////////////////////////////////////////////////////////////////////////////

const char* VehicleComponent::GetComponentName() const
{
	return {};
}

unsigned int VehicleComponent::GetPartCount() const
{
	return {};
}

IVehiclePart* VehicleComponent::GetPart(unsigned int index) const
{
	return {};
}

const AABB& VehicleComponent::GetBounds()
{
	static const AABB bounds;
	return bounds;
}

float VehicleComponent::GetDamageRatio() const
{
	return {};
}

void VehicleComponent::SetDamageRatio(float ratio)
{
}

float VehicleComponent::GetMaxDamage() const
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
