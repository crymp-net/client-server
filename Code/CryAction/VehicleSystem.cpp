#include <cstdint>

#include "VehicleSystem.h"

VehicleSystem::VehicleSystem(ISystem* pSystem, IEntitySystem* pEntitySystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30537460;
#else
	std::uintptr_t ctor = 0x30528d20;
#endif

	(this->*reinterpret_cast<void(VehicleSystem::*&)(ISystem*, IEntitySystem*)>(ctor))(pSystem, pEntitySystem);
}

VehicleSystem::~VehicleSystem()
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSystem
////////////////////////////////////////////////////////////////////////////////

void VehicleSystem::RegisterFactory(const char* name, IVehicleMovement* (*)(), bool isAI)
{
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleView* (*)(), bool isAI)
{
}

void VehicleSystem::RegisterFactory(const char* name, IVehiclePart* (*)(), bool isAI)
{
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleDamageBehavior* (*)(), bool isAI)
{
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleSeatAction* (*)(), bool isAI)
{
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleAction* (*)(), bool isAI)
{
}

bool VehicleSystem::Init()
{
	return {};
}

void VehicleSystem::Release()
{
}

void VehicleSystem::Reset()
{
}

void VehicleSystem::RegisterVehicles(IGameFramework* pGameFramework)
{
}

IVehicle* VehicleSystem::CreateVehicle(
	uint16 channelId,
	const char* name,
	const char* vehicleClass,
	const Vec3& pos,
	const Quat& rot,
	const Vec3& scale,
	EntityId id)
{
	return {};
}

IVehicle* VehicleSystem::GetVehicle(EntityId entityId)
{
	return {};
}

IVehicle* VehicleSystem::GetVehicleByChannelId(uint16 channelId)
{
	return {};
}

bool VehicleSystem::IsVehicleClass(const char* name) const
{
	return {};
}

IVehicleMovement* VehicleSystem::CreateVehicleMovement(const string& name)
{
	return {};
}

IVehicleView* VehicleSystem::CreateVehicleView(const string& name)
{
	return {};
}

IVehiclePart* VehicleSystem::CreateVehiclePart(const string& name)
{
	return {};
}

IVehicleDamageBehavior* VehicleSystem::CreateVehicleDamageBehavior(const string& name)
{
	return {};
}

IVehicleSeatAction* VehicleSystem::CreateVehicleSeatAction(const string& name)
{
	return {};
}

IVehicleAction* VehicleSystem::CreateVehicleAction(const string& name)
{
	return {};
}

IVehicleDamagesTemplateRegistry* VehicleSystem::GetDamagesTemplateRegistry()
{
	return {};
}

bool VehicleSystem::GetVehicleLightDefaults(const char* type, SmartScriptTable& table)
{
	return {};
}

void VehicleSystem::GetVehicleImplementations(SVehicleImpls& impls)
{
}

bool VehicleSystem::GetOptionalScript(const char* vehicleName, char* buf, size_t len)
{
	return {};
}

void VehicleSystem::AddVehicle(EntityId entityId, IVehicle* pProxy)
{
}

void VehicleSystem::RemoveVehicle(EntityId entityId)
{
}

TVehicleObjectId VehicleSystem::AssignVehicleObjectId()
{
	return {};
}

uint32 VehicleSystem::GetVehicleCount()
{
	return {};
}

IVehicleIteratorPtr VehicleSystem::CreateVehicleIterator()
{
	return {};
}

IVehicleClient* VehicleSystem::GetVehicleClient()
{
	return {};
}

void VehicleSystem::RegisterVehicleClient(IVehicleClient* pVehicleClient)
{
}

////////////////////////////////////////////////////////////////////////////////
