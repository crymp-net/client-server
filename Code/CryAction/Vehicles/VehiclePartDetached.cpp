#include "VehiclePartDetached.h"

VehiclePartDetached::VehiclePartDetached()
{
#ifdef BUILD_64BIT
	auto vtable = reinterpret_cast<void*>(0x30843fe8);
#else
	auto vtable = reinterpret_cast<void*>(0x30761118);
#endif

	*reinterpret_cast<void**>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IGameObjectExtension
////////////////////////////////////////////////////////////////////////////////

bool VehiclePartDetached::Init(IGameObject* pGameObject)
{
	return {};
}

void VehiclePartDetached::PostInit(IGameObject* pGameObject)
{
}

void VehiclePartDetached::InitClient(int channelId)
{
}

void VehiclePartDetached::PostInitClient(int channelId)
{
}

void VehiclePartDetached::Release()
{
}

void VehiclePartDetached::FullSerialize(TSerialize ser)
{
}

bool VehiclePartDetached::NetSerialize(TSerialize ser, EEntityAspects aspect, std::uint8_t profile, int flags)
{
	return {};
}

void VehiclePartDetached::PostSerialize()
{
}

void VehiclePartDetached::SerializeSpawnInfo(TSerialize ser)
{
}

ISerializableInfoPtr VehiclePartDetached::GetSpawnInfo()
{
	return {};
}

void VehiclePartDetached::Update(SEntityUpdateContext& context, int updateSlot)
{
}

void VehiclePartDetached::HandleEvent(const SGameObjectEvent& event)
{
}

void VehiclePartDetached::ProcessEvent(SEntityEvent& event)
{
}

void VehiclePartDetached::GetMemoryStatistics(ICrySizer* s)
{
}

void VehiclePartDetached::SetChannelId(std::uint16_t id)
{
}

void VehiclePartDetached::SetAuthority(bool auth)
{
}

const void* VehiclePartDetached::GetRMIBase() const
{
	return {};
}

void VehiclePartDetached::PostUpdate(float frameTime)
{
}

void VehiclePartDetached::PostRemoteSpawn()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehiclePartDetached::Register(IGameFramework *pGameFramework)
{
	IEntityClassRegistry::SEntityClassDesc entityClass;
	entityClass.flags = ECLF_INVISIBLE;
	entityClass.sName = "VehiclePartDetached";
	entityClass.sScriptFile = "";
	entityClass.pUserProxyCreateFunc = nullptr;
	entityClass.pUserProxyData = nullptr;

	static IGameFramework::CGameObjectExtensionCreator<VehiclePartDetached> creator;

	pGameFramework->GetIGameObjectSystem()->RegisterExtension(entityClass.sName, &creator, &entityClass);
}
