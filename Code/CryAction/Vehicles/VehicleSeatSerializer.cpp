#include "VehicleSeatSerializer.h"

VehicleSeatSerializer::VehicleSeatSerializer()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305b4e40;
#else
	std::uintptr_t ctor = 0x30582cc0;
#endif

	(this->*reinterpret_cast<void(VehicleSeatSerializer::*&)()>(ctor))();
}

VehicleSeatSerializer::~VehicleSeatSerializer()
{
}

////////////////////////////////////////////////////////////////////////////////
// IGameObjectExtension
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatSerializer::Init(IGameObject* pGameObject)
{
	return {};
}

void VehicleSeatSerializer::PostInit(IGameObject* pGameObject)
{
}

void VehicleSeatSerializer::InitClient(int channelId)
{
}

void VehicleSeatSerializer::PostInitClient(int channelId)
{
}

void VehicleSeatSerializer::Release()
{
}

void VehicleSeatSerializer::FullSerialize(TSerialize ser)
{
}

bool VehicleSeatSerializer::NetSerialize(TSerialize ser, EEntityAspects aspect, std::uint8_t profile, int flags)
{
	return {};
}

void VehicleSeatSerializer::PostSerialize()
{
}

void VehicleSeatSerializer::SerializeSpawnInfo(TSerialize ser)
{
}

ISerializableInfoPtr VehicleSeatSerializer::GetSpawnInfo()
{
	return {};
}

void VehicleSeatSerializer::Update(SEntityUpdateContext& context, int updateSlot)
{
}

void VehicleSeatSerializer::HandleEvent(const SGameObjectEvent& event)
{
}

void VehicleSeatSerializer::ProcessEvent(SEntityEvent& event)
{
}

void VehicleSeatSerializer::GetMemoryStatistics(ICrySizer* s)
{
}

void VehicleSeatSerializer::SetChannelId(std::uint16_t id)
{
}

void VehicleSeatSerializer::SetAuthority(bool auth)
{
}

const void* VehicleSeatSerializer::GetRMIBase() const
{
	return {};
}

void VehicleSeatSerializer::PostUpdate(float frameTime)
{
}

void VehicleSeatSerializer::PostRemoteSpawn()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatSerializer::Register(IGameFramework* pGameFramework)
{
	IEntityClassRegistry::SEntityClassDesc entityClass;
	entityClass.flags = ECLF_INVISIBLE;
	entityClass.sName = "VehicleSeatSerializer";
	entityClass.sScriptFile = "";
	entityClass.pUserProxyCreateFunc = nullptr;
	entityClass.pUserProxyData = nullptr;

	static IGameFramework::CGameObjectExtensionCreator<VehicleSeatSerializer> creator;

	pGameFramework->GetIGameObjectSystem()->RegisterExtension(entityClass.sName, &creator, &entityClass);
}
