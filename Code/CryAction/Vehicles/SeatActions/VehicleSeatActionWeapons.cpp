#include "VehicleSeatActionWeapons.h"

VehicleSeatActionWeapons::VehicleSeatActionWeapons()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305a68b0;
#else
	std::uintptr_t ctor = 0x30579350;
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
	*reinterpret_cast<TVehicleObjectId*>(0x30927384) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a868c) = id;
#endif
}
