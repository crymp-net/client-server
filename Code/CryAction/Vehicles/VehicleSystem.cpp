#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "CrySystem/CryLog.h"

#include "Actions/VehicleActionEnter.h"

#include "DamageBehaviors/VehicleDamageBehaviorAISignal.h"
#include "DamageBehaviors/VehicleDamageBehaviorDestroy.h"
#include "DamageBehaviors/VehicleDamageBehaviorDetachPart.h"
#include "DamageBehaviors/VehicleDamageBehaviorEffect.h"
#include "DamageBehaviors/VehicleDamageBehaviorGroup.h"
#include "DamageBehaviors/VehicleDamageBehaviorHitPassenger.h"
#include "DamageBehaviors/VehicleDamageBehaviorImpulse.h"
#include "DamageBehaviors/VehicleDamageBehaviorIndicator.h"
#include "DamageBehaviors/VehicleDamageBehaviorMovementNotification.h"
#include "DamageBehaviors/VehicleDamageBehaviorSink.h"
#include "DamageBehaviors/VehicleDamageBehaviorSpawnDebris.h"

#include "Parts/VehiclePartAnimated.h"
#include "Parts/VehiclePartAnimatedJoint.h"
#include "Parts/VehiclePartBase.h"
#include "Parts/VehiclePartEntityAttachment.h"
#include "Parts/VehiclePartLight.h"
#include "Parts/VehiclePartMassBox.h"
#include "Parts/VehiclePartStatic.h"
#include "Parts/VehiclePartSubPart.h"
#include "Parts/VehiclePartSubPartWheel.h"
#include "Parts/VehiclePartTread.h"

#include "SeatActions/VehicleSeatActionAnimation.h"
#include "SeatActions/VehicleSeatActionLights.h"
#include "SeatActions/VehicleSeatActionMovement.h"
#include "SeatActions/VehicleSeatActionPassengerIK.h"
#include "SeatActions/VehicleSeatActionRotateTurret.h"
#include "SeatActions/VehicleSeatActionSound.h"
#include "SeatActions/VehicleSeatActionSteeringWheel.h"
#include "SeatActions/VehicleSeatActionWeapons.h"

#include "Views/VehicleViewActionThirdPerson.h"
#include "Views/VehicleViewFirstPerson.h"
#include "Views/VehicleViewThirdPerson.h"

#include "Vehicle.h"
#include "VehicleCVars.h"
#include "VehicleDamagesTemplateRegistry.h"
#include "VehicleIterator.h"
#include "VehiclePartDetached.h"
#include "VehicleSeatSerializer.h"
#include "VehicleSystem.h"

VehicleSystem::VehicleSystem(IGameFramework* pGameFramework) : m_pGameFramework(pGameFramework)
{
	// see m_reserved comment
#ifdef BUILD_64BIT
	static_assert(offsetof(VehicleSystem, m_reserved) <= 0x1f8);
	static_assert(sizeof(VehicleSystem) >= (0x1f8 + 8));
#else
	static_assert(offsetof(VehicleSystem, m_reserved) <= 0xfc);
	static_assert(sizeof(VehicleSystem) >= (0xfc + 4));
#endif

	m_pDamagesTemplateRegistry = std::make_unique<VehicleDamagesTemplateRegistry>();
	m_pCVars = std::make_unique<VehicleCVars>();
}

VehicleSystem::~VehicleSystem()
{
}

void VehicleSystem::RegisterVehicleFactory(const char* name, IGameFramework::IVehicleCreator* pCreator)
{
	IEntityClassRegistry::SEntityClassDesc vehicleClass;
	vehicleClass.flags = 0;
	vehicleClass.sName = name;
	vehicleClass.sScriptFile = "Scripts/Entities/Vehicles/VehiclePool.lua";
	vehicleClass.pUserProxyCreateFunc = nullptr;
	vehicleClass.pUserProxyData = nullptr;

	m_pGameFramework->GetIGameObjectSystem()->RegisterExtension(name, pCreator, &vehicleClass);

	m_vehicleFactories[name] = pCreator;
}

void VehicleSystem::Reload()
{
	this->InitDamagesTemplateRegistry();
	this->InitLightDefaults();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSystem
////////////////////////////////////////////////////////////////////////////////

void VehicleSystem::RegisterFactory(const char* name, IVehicleMovement* (*pFactory)(), bool isAI)
{
	m_movementFactories[name] = pFactory;
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleView* (*pFactory)(), bool isAI)
{
	m_viewFactories[name] = pFactory;
}

void VehicleSystem::RegisterFactory(const char* name, IVehiclePart* (*pFactory)(), bool isAI)
{
	m_partFactories[name] = pFactory;
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleDamageBehavior* (*pFactory)(), bool isAI)
{
	m_damageBehaviorFactories[name] = pFactory;
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleSeatAction* (*pFactory)(), bool isAI)
{
	m_seatActionFactories[name] = pFactory;
}

void VehicleSystem::RegisterFactory(const char* name, IVehicleAction* (*pFactory)(), bool isAI)
{
	m_actionFactories[name] = pFactory;
}

bool VehicleSystem::Init()
{
	m_nextVehicleObjectId = 1;

	this->InitDamagesTemplateRegistry();
	this->InitLightDefaults();

	return true;
}

void VehicleSystem::Release()
{
}

void VehicleSystem::Reset()
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	if (gEnv->pSystem->IsSerializingFile() != 1)
	{
		return;
	}

	std::erase_if(m_vehicles, [pEntitySystem = gEnv->pEntitySystem](const auto& vehicle)
	{
		const auto& [vehicleId, pVehicle] = vehicle;
		return pEntitySystem->GetEntity(vehicleId) == nullptr;
	});
}

void VehicleSystem::RegisterVehicles(IGameFramework* pGameFramework)
{
	VehicleSeatSerializer::Register(m_pGameFramework);
	VehiclePartDetached::Register(m_pGameFramework);

	// Implementations - originally in XMLs
	static IGameFramework::CVehicleCreator<Vehicle> creator;
	this->RegisterVehicleFactory("Alien_warrior", &creator);
	this->RegisterVehicleFactory("Asian_aaa", &creator);
	this->RegisterVehicleFactory("Asian_apc", &creator);
	this->RegisterVehicleFactory("Asian_helicopter", &creator);
	this->RegisterVehicleFactory("Asian_ltv", &creator);
	this->RegisterVehicleFactory("Asian_patrolboat", &creator);
	this->RegisterVehicleFactory("Asian_tank", &creator);
	this->RegisterVehicleFactory("Asian_truck", &creator);
	this->RegisterVehicleFactory("Civ_car1", &creator);
	this->RegisterVehicleFactory("Civ_speedboat", &creator);
	this->RegisterVehicleFactory("DefaultVehicle", &creator);
	this->RegisterVehicleFactory("US_apc", &creator);
	this->RegisterVehicleFactory("US_hovercraft", &creator);
	this->RegisterVehicleFactory("US_ltv", &creator);
	this->RegisterVehicleFactory("US_smallboat", &creator);
	this->RegisterVehicleFactory("US_tank", &creator);
	this->RegisterVehicleFactory("US_transportVTOL", &creator);
	this->RegisterVehicleFactory("US_trolley", &creator);
	this->RegisterVehicleFactory("US_vtol", &creator);

	// Views
	VehicleViewActionThirdPerson::Register(this);
	VehicleViewFirstPerson::Register(this);
	VehicleViewThirdPerson::Register(this);

	// Parts
	VehiclePartBase::Register(this);
	VehiclePartAnimated::Register(this);
	VehiclePartAnimatedJoint::Register(this);
	VehiclePartLight::Register(this);
	VehiclePartMassBox::Register(this);
	VehiclePartStatic::Register(this);
	VehiclePartSubPart::Register(this);
	VehiclePartSubPartWheel::Register(this);
	VehiclePartTread::Register(this);
	VehiclePartEntityAttachment::Register(this);

	// Damage behaviors
	VehicleDamageBehaviorAISignal::Register(this);
	VehicleDamageBehaviorDestroy::Register(this);
	VehicleDamageBehaviorDetachPart::Register(this);
	VehicleDamageBehaviorEffect::Register(this);
	VehicleDamageBehaviorGroup::Register(this);
	VehicleDamageBehaviorHitPassenger::Register(this);
	VehicleDamageBehaviorImpulse::Register(this);
	VehicleDamageBehaviorIndicator::Register(this);
	VehicleDamageBehaviorMovementNotification::Register(this);
	VehicleDamageBehaviorSink::Register(this);
	VehicleDamageBehaviorSpawnDebris::Register(this);

	// Seat actions
	VehicleSeatActionLights::Register(this);
	VehicleSeatActionMovement::Register(this);
	VehicleSeatActionPassengerIK::Register(this);
	VehicleSeatActionRotateTurret::Register(this);
	VehicleSeatActionSound::Register(this);
	VehicleSeatActionSteeringWheel::Register(this);
	VehicleSeatActionWeapons::Register(this);
	VehicleSeatActionAnimation::Register(this);

	// Actions
	VehicleActionEnter::Register(this);

	// originally executed in ScriptBind_VehicleSystem constructor
	// it has to be here because Vehicle.GetVehicleImplementations returns nothing before vehicle registration
	gEnv->pScriptSystem->ExecuteFile("Scripts/Entities/Vehicles/VehicleSystem.lua");
}

IVehicle* VehicleSystem::CreateVehicle(
	std::uint16_t channelId,
	const char* name,
	const char* vehicleClass,
	const Vec3& pos,
	const Quat& rot,
	const Vec3& scale,
	EntityId id)
{
	IEntitySystem* pEntitySystem = gEnv->pEntitySystem;

	IEntityClass* pClass = pEntitySystem->GetClassRegistry()->FindClass(vehicleClass);
	if (!pClass)
	{
		CryLogErrorAlways("[VehicleSystem] Cannot create vehicle %s of unknown class %s", name, vehicleClass);
		return nullptr;
	}

	if (channelId)
	{
		IVehicle* pExistingVehicle = this->GetVehicleByChannelId(channelId);
		if (pExistingVehicle)
		{
			id = pExistingVehicle->GetEntityId();
		}
	}

	SEntitySpawnParams params;
	params.id = id;
	params.pClass = pClass;
	params.sName = name;
	params.vPosition = pos;
	params.qRotation = rot;
	params.vScale = scale;

	IEntity* pEntity = pEntitySystem->SpawnEntity(params, true);
	if (!pEntity)
	{
		CryLogErrorAlways("[VehicleSystem] Spawning vehicle %s failed", name);
		return nullptr;
	}

	IVehicle* pVehicle = this->GetVehicle(pEntity->GetId());
	if (!pVehicle)
	{
		CryLogErrorAlways("[VehicleSystem] Cannot find spawned vehicle %s", name);
		return nullptr;
	}

	return pVehicle;
}

IVehicle* VehicleSystem::GetVehicle(EntityId entityId)
{
	const auto it = m_vehicles.find(entityId);
	if (it == m_vehicles.end())
	{
		return nullptr;
	}

	return it->second;
}

IVehicle* VehicleSystem::GetVehicleByChannelId(std::uint16_t channelId)
{
	for (auto& [entityId, pVehicle] : m_vehicles)
	{
		if (pVehicle->GetChannelId() == channelId)
		{
			return pVehicle;
		}
	}

	return nullptr;
}

bool VehicleSystem::IsVehicleClass(const char* name) const
{
	if (!name)
	{
		return false;
	}

	return m_vehicleFactories.contains(name);
}

IVehicleMovement* VehicleSystem::CreateVehicleMovement(const string& name)
{
	const auto it = m_movementFactories.find(std::string_view(name.c_str(), name.length()));
	if (it == m_movementFactories.end())
	{
		CryLogErrorAlways("[VehicleSystem] Cannot create unknown vehicle movement %s", name.c_str());
		return nullptr;
	}

	return it->second();
}

IVehicleView* VehicleSystem::CreateVehicleView(const string& name)
{
	const auto it = m_viewFactories.find(std::string_view(name.c_str(), name.length()));
	if (it == m_viewFactories.end())
	{
		CryLogErrorAlways("[VehicleSystem] Cannot create unknown vehicle view %s", name.c_str());
		return nullptr;
	}

	return it->second();
}

IVehiclePart* VehicleSystem::CreateVehiclePart(const string& name)
{
	const auto it = m_partFactories.find(std::string_view(name.c_str(), name.length()));
	if (it == m_partFactories.end())
	{
		CryLogErrorAlways("[VehicleSystem] Cannot create unknown vehicle part %s", name.c_str());
		return nullptr;
	}

	return it->second();
}

IVehicleDamageBehavior* VehicleSystem::CreateVehicleDamageBehavior(const string& name)
{
	const auto it = m_damageBehaviorFactories.find(std::string_view(name.c_str(), name.length()));
	if (it == m_damageBehaviorFactories.end())
	{
		CryLogErrorAlways("[VehicleSystem] Cannot create unknown vehicle damage behavior %s", name.c_str());
		return nullptr;
	}

	return it->second();
}

IVehicleSeatAction* VehicleSystem::CreateVehicleSeatAction(const string& name)
{
	const auto it = m_seatActionFactories.find(std::string_view(name.c_str(), name.length()));
	if (it == m_seatActionFactories.end())
	{
		CryLogErrorAlways("[VehicleSystem] Cannot create unknown vehicle seat action %s", name.c_str());
		return nullptr;
	}

	return it->second();
}

IVehicleAction* VehicleSystem::CreateVehicleAction(const string& name)
{
	const auto it = m_actionFactories.find(std::string_view(name.c_str(), name.length()));
	if (it == m_actionFactories.end())
	{
		CryLogErrorAlways("[VehicleSystem] Cannot create unknown vehicle action %s", name.c_str());
		return nullptr;
	}

	return it->second();
}

IVehicleDamagesTemplateRegistry* VehicleSystem::GetDamagesTemplateRegistry()
{
	return m_pDamagesTemplateRegistry.get();
}

bool VehicleSystem::GetVehicleLightDefaults(const char* type, SmartScriptTable& table)
{
	const auto it = m_lightDefaults.find(type);
	if (it == m_lightDefaults.end())
	{
		return false;
	}

	table = it->second;

	return true;
}

void VehicleSystem::GetVehicleImplementations(SVehicleImpls& impls)
{
	// unused
	CryLogErrorAlways("[VehicleSystem::GetVehicleImplementations] Not implemented!");
}

bool VehicleSystem::GetOptionalScript(const char* vehicleName, char* buf, size_t len)
{
	// unused
	CryLogErrorAlways("[VehicleSystem::GetOptionalScript] Not implemented!");

	return false;
}

void VehicleSystem::AddVehicle(EntityId entityId, IVehicle* pProxy)
{
	m_vehicles[entityId] = pProxy;
}

void VehicleSystem::RemoveVehicle(EntityId entityId)
{
	m_vehicles.erase(entityId);
}

TVehicleObjectId VehicleSystem::AssignVehicleObjectId()
{
	return m_nextVehicleObjectId++;
}

std::uint32_t VehicleSystem::GetVehicleCount()
{
	return static_cast<std::uint32_t>(m_vehicles.size());
}

IVehicleIteratorPtr VehicleSystem::CreateVehicleIterator()
{
	return IVehicleIteratorPtr(VehicleIterator::Create(this));
}

IVehicleClient* VehicleSystem::GetVehicleClient()
{
	return m_pVehicleClient;
}

void VehicleSystem::RegisterVehicleClient(IVehicleClient* pVehicleClient)
{
	m_pVehicleClient = pVehicleClient;
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSystem::InitDamagesTemplateRegistry()
{
	m_pDamagesTemplateRegistry->Init(
		"Scripts/Entities/Vehicles/DamagesTemplates/def_vehicledamages.xml",
		"Scripts/Entities/Vehicles/DamagesTemplates/");
}

void VehicleSystem::InitLightDefaults()
{
	IScriptSystem* pScriptSystem = gEnv->pScriptSystem;

	{
		SmartScriptTable light(pScriptSystem);
		light->SetValue("type", "HeadLightBeam");
		light->SetValue("fakeLight", true);
		light->SetValue("material", "materials/lightbeams/lightbeam_floodlight.mtl");
		light->SetValue("texture", "textures/lights/flashlight1.dds");
		light->SetValue("diffuse", Vec3(1,1,1));
		light->SetValue("diffuseMult", 1.5f);
		light->SetValue("diffuseMult_fp", 0.6f);
		light->SetValue("radius", 12.0f);
		light->SetValue("frustumAngle", 10.0f);

		m_lightDefaults["HeadLightBeam"] = light;
	}
	{
		SmartScriptTable light(pScriptSystem);
		light->SetValue("type", "HeadLightFlare");
		light->SetValue("fakeLight", true);
		light->SetValue("material", "materials/lightbeams/lightglow_white.mtl");
		light->SetValue("diffuse", Vec3(1,1,1));
		light->SetValue("diffuseMult", 1.0f);
		light->SetValue("coronaDistSize", 0.002f);
		light->SetValue("coronaDistIntensity", 0.06f);
		light->SetValue("coronaScale", 10.0f);

		m_lightDefaults["HeadLightFlare"] = light;
	}
	{
		SmartScriptTable light(pScriptSystem);
		light->SetValue("type", "Rear");
		light->SetValue("fakeLight", true);
		light->SetValue("material", "materials/lightbeams/lightglow_red.mtl");
		light->SetValue("diffuse", Vec3(1,0,0));
		light->SetValue("diffuseMult", 1.5f);
		light->SetValue("coronaDistSize", 0.002f);
		light->SetValue("coronaDistIntensity", 0.08f);
		light->SetValue("coronaScale", 10.0f);

		m_lightDefaults["Rear"] = light;
	}
	{
		SmartScriptTable light(pScriptSystem);
		light->SetValue("type", "Brake");
		light->SetValue("fakeLight", true);
		light->SetValue("material", "materials/lightbeams/lightglow_red.mtl");
		light->SetValue("diffuse", Vec3(1,0,0));
		light->SetValue("diffuseMult", 1.9f);
		light->SetValue("coronaDistSize", 0.002f);
		light->SetValue("coronaDistIntensity", 0.08f);
		light->SetValue("coronaScale", 10.0f);

		m_lightDefaults["Brake"] = light;
	}
	{
		SmartScriptTable light(pScriptSystem);
		light->SetValue("type", "Search");
		light->SetValue("fakeLight", false);
		light->SetValue("texture", "textures/lights/flashlight1.dds");
		light->SetValue("frustumAngle", 40.0f);  // CryMP: this was originally non-existent "lightFrustumAngle"
		light->SetValue("radius", 20.0f);
		light->SetValue("directFactor", 2.0f);
		light->SetValue("diffuseMult", 5.0f);
		light->SetValue("specularMult", 1.0f);

		m_lightDefaults["Search"] = light;
	}
	{
		SmartScriptTable light(pScriptSystem);
		light->SetValue("type", "HeadLightDynamic");
		light->SetValue("fakeLight", false);
		light->SetValue("texture", "textures/lights/flashlight1.dds");
		light->SetValue("frustumAngle", 40.0f);  // CryMP: this was originally non-existent "lightFrustumAngle"
		light->SetValue("radius", 12.0f);
		light->SetValue("directFactor", 2.0f);
		light->SetValue("diffuseMult", 5.0f);
		light->SetValue("specularMult", 1.0f);

		m_lightDefaults["HeadLightDynamic"] = light;
	}
}
