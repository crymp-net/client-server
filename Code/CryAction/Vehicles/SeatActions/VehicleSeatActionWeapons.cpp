#include "VehicleSeatActionWeapons.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleSeatActionWeapons::VehicleSeatActionWeapons()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0xa68b0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x79350;
#endif

	(this->*reinterpret_cast<void(VehicleSeatActionWeapons::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionWeapons::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionWeapons::GetId()
{
	return {};
}

void VehicleSeatActionWeapons::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionWeapons::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionWeapons::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionWeapons::Reset()
{
}

void VehicleSeatActionWeapons::Release()
{
}

void VehicleSeatActionWeapons::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionWeapons::StopUsing()
{
}

void VehicleSeatActionWeapons::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionWeapons::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionWeapons::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionWeapons::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Weapons", VehicleSeatActionWeapons, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x427384) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a868c) = id;
#endif
}
