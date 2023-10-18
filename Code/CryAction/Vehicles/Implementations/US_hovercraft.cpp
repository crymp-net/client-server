#include "US_hovercraft.h"

US_hovercraft::US_hovercraft()
{
	// enable US_hovercraft::Init function
	this->Vehicle::PatchVTable();
}

US_hovercraft::~US_hovercraft()
{
}

bool US_hovercraft::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_physics.damping = 0.02f;
	m_physics.dampingFreefall = 0.0099999998f;
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 65.0f;
	m_buoyancy.waterResistance = 500.0f;
	m_buoyancy.waterDamping = 0.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.1f;
	m_physics.maxLoggedCollisions = 2;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable chassis_part(gEnv->pScriptSystem);
			chassis_part->SetValue("name", "chassis");
			chassis_part->SetValue("class", "Animated");
			chassis_part->SetValue("mass", 0.0f);
			chassis_part->SetValue("isHidden", false);
			SmartScriptTable chassis_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable shitenTurret_part(gEnv->pScriptSystem);
				shitenTurret_part->SetValue("name", "shitenTurret");
				shitenTurret_part->SetValue("class", "AnimatedJoint");
				shitenTurret_part->SetValue("helper", "");
				shitenTurret_part->SetValue("mass", 0.0f);
				SmartScriptTable shitenTurret_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable shiten_part(gEnv->pScriptSystem);
					shiten_part->SetValue("name", "shiten");
					shiten_part->SetValue("class", "AnimatedJoint");
					shiten_part->SetValue("helper", "");
					shiten_part->SetValue("mass", 0.0f);
					SmartScriptTable shiten_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable driverEyePos_helper(gEnv->pScriptSystem);
						driverEyePos_helper->SetValue("name", "driverEyePos");
						driverEyePos_helper->SetValue("position", Vec3(0, 1, 2.4979999));
						driverEyePos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(driverEyePos_helper);
					}
					{
						SmartScriptTable shitenOutPos_helper(gEnv->pScriptSystem);
						shitenOutPos_helper->SetValue("name", "shitenOutPos");
						shitenOutPos_helper->SetValue("position", Vec3(0, 1.95, 2.9579999));
						shitenOutPos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(shitenOutPos_helper);
					}
					shiten_part->SetValue("Helpers", shiten_part_helpers);
					SmartScriptTable shiten_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable shiten_part_animatedjoint_rotation(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation->SetValue("pitchSpeed", 60.0f);
					SmartScriptTable shiten_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation_pitchlimits->PushBack(-15.0f);
					shiten_part_animatedjoint_rotation_pitchlimits->PushBack(60.0f);
					shiten_part_animatedjoint_rotation->SetValue("pitchLimits", shiten_part_animatedjoint_rotation_pitchlimits);
					shiten_part_animatedjoint->SetValue("Rotation", shiten_part_animatedjoint_rotation);
					shiten_part->SetValue("AnimatedJoint", shiten_part_animatedjoint);
					shitenTurret_part_parts->PushBack(shiten_part);
				}
				shitenTurret_part->SetValue("Parts", shitenTurret_part_parts);
				SmartScriptTable shitenTurret_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable mounted_gun_pos_helper(gEnv->pScriptSystem);
					mounted_gun_pos_helper->SetValue("name", "mounted_gun_pos");
					mounted_gun_pos_helper->SetValue("position", Vec3(0, 0.69999999, 2.848));
					mounted_gun_pos_helper->SetValue("direction", Vec3(0, 1, 0));
					shitenTurret_part_helpers->PushBack(mounted_gun_pos_helper);
				}
				shitenTurret_part->SetValue("Helpers", shitenTurret_part_helpers);
				SmartScriptTable shitenTurret_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable shitenTurret_part_animatedjoint_rotation(gEnv->pScriptSystem);
				shitenTurret_part_animatedjoint_rotation->SetValue("yawSpeed", 130.0f);
				SmartScriptTable shitenTurret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(-90.0f);
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(90.0f);
				shitenTurret_part_animatedjoint_rotation->SetValue("yawLimits", shitenTurret_part_animatedjoint_rotation_yawlimits);
				shitenTurret_part_animatedjoint->SetValue("Rotation", shitenTurret_part_animatedjoint_rotation);
				shitenTurret_part->SetValue("AnimatedJoint", shitenTurret_part_animatedjoint);
				chassis_part_parts->PushBack(shitenTurret_part);
			}
			chassis_part->SetValue("Parts", chassis_part_parts);
			SmartScriptTable chassis_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable turret_pos_helper(gEnv->pScriptSystem);
				turret_pos_helper->SetValue("name", "turret_pos");
				turret_pos_helper->SetValue("position", Vec3(0, 0.69999999, 2.848));
				turret_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(turret_pos_helper);
			}
			{
				SmartScriptTable driverSitPos_helper(gEnv->pScriptSystem);
				driverSitPos_helper->SetValue("name", "driverSitPos");
				driverSitPos_helper->SetValue("position", Vec3(0, 1.17, 1.5));
				driverSitPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(driverSitPos_helper);
			}
			{
				SmartScriptTable driverExitPos_helper(gEnv->pScriptSystem);
				driverExitPos_helper->SetValue("name", "driverExitPos");
				driverExitPos_helper->SetValue("position", Vec3(-3.1201, 0.87989998, 1.5532));
				driverExitPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(driverExitPos_helper);
			}
			{
				SmartScriptTable exhaustPos_helper(gEnv->pScriptSystem);
				exhaustPos_helper->SetValue("name", "exhaustPos");
				exhaustPos_helper->SetValue("position", Vec3(0, -6, 1.998));
				exhaustPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(exhaustPos_helper);
			}
			{
				SmartScriptTable dustPos_helper(gEnv->pScriptSystem);
				dustPos_helper->SetValue("name", "dustPos");
				dustPos_helper->SetValue("position", Vec3(0, -0.69410002, 0.62400001));
				dustPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(dustPos_helper);
			}
			{
				SmartScriptTable passenger01_sit_pos_helper(gEnv->pScriptSystem);
				passenger01_sit_pos_helper->SetValue("name", "passenger01_sit_pos");
				passenger01_sit_pos_helper->SetValue("position", Vec3(-0.8125, -1.1045001, 1.574));
				passenger01_sit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(passenger01_sit_pos_helper);
			}
			{
				SmartScriptTable passenger02_sit_pos_helper(gEnv->pScriptSystem);
				passenger02_sit_pos_helper->SetValue("name", "passenger02_sit_pos");
				passenger02_sit_pos_helper->SetValue("position", Vec3(-0.0308, -1.0989, 1.5789));
				passenger02_sit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(passenger02_sit_pos_helper);
			}
			{
				SmartScriptTable passenger03_sit_pos_helper(gEnv->pScriptSystem);
				passenger03_sit_pos_helper->SetValue("name", "passenger03_sit_pos");
				passenger03_sit_pos_helper->SetValue("position", Vec3(0.74269998, -1.1066999, 1.5723));
				passenger03_sit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(passenger03_sit_pos_helper);
			}
			{
				SmartScriptTable passenger01_exit_pos_helper(gEnv->pScriptSystem);
				passenger01_exit_pos_helper->SetValue("name", "passenger01_exit_pos");
				passenger01_exit_pos_helper->SetValue("position", Vec3(-3.1814001, -0.6196, 0.96810001));
				passenger01_exit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(passenger01_exit_pos_helper);
			}
			{
				SmartScriptTable passenger02_exit_pos_helper(gEnv->pScriptSystem);
				passenger02_exit_pos_helper->SetValue("name", "passenger02_exit_pos");
				passenger02_exit_pos_helper->SetValue("position", Vec3(-3.1975, -1.4741, 0.96810001));
				passenger02_exit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(passenger02_exit_pos_helper);
			}
			{
				SmartScriptTable passenger03_exit_pos_helper(gEnv->pScriptSystem);
				passenger03_exit_pos_helper->SetValue("name", "passenger03_exit_pos");
				passenger03_exit_pos_helper->SetValue("position", Vec3(3.1973, -0.87400001, 0.96810001));
				passenger03_exit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(passenger03_exit_pos_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(-1.45, 3, 1.78));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				chassis_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(1.45, 3, 1.78));
				headLightRight_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				chassis_part_helpers->PushBack(headLightRight_helper);
			}
			{
				SmartScriptTable headLightCenter_helper(gEnv->pScriptSystem);
				headLightCenter_helper->SetValue("name", "headLightCenter");
				headLightCenter_helper->SetValue("position", Vec3(-0.017100001, 3.9640999, 1.6084));
				headLightCenter_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				chassis_part_helpers->PushBack(headLightCenter_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, 0.12819999, 1.0773));
				burningPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(burningPos_helper);
			}
			chassis_part->SetValue("Helpers", chassis_part_helpers);
			SmartScriptTable chassis_part_animated(gEnv->pScriptSystem);
			chassis_part_animated->SetValue("filename", "Objects/Vehicles/US_Hovercraft_B/US_Hovercraft_B.cga");
			chassis_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/US_Hovercraft_B/US_Hovercraft_B_destroyed.cga");
			chassis_part->SetValue("Animated", chassis_part_animated);
			parts->PushBack(chassis_part);
		}
		{
			SmartScriptTable skirt_part(gEnv->pScriptSystem);
			skirt_part->SetValue("name", "skirt");
			skirt_part->SetValue("class", "Base");
			skirt_part->SetValue("mass", 0.0f);
			skirt_part->SetValue("filename", "objects/vehicles/us_hovercraft_b/us_hovercraft_b.chr");
			parts->PushBack(skirt_part);
		}
		{
			SmartScriptTable mass_part(gEnv->pScriptSystem);
			mass_part->SetValue("name", "mass");
			mass_part->SetValue("class", "MassBox");
			mass_part->SetValue("mass", 2000.0f);
			mass_part->SetValue("position", Vec3(0, -0.1472, 0.85000002));
			SmartScriptTable mass_part_massbox(gEnv->pScriptSystem);
			mass_part_massbox->SetValue("size", Vec3(2.2279999, 4.4499998, 1));
			mass_part->SetValue("MassBox", mass_part_massbox);
			parts->PushBack(mass_part);
		}
		{
			SmartScriptTable headLightCenter_part(gEnv->pScriptSystem);
			headLightCenter_part->SetValue("name", "headLightCenter");
			headLightCenter_part->SetValue("class", "Light");
			headLightCenter_part->SetValue("component", "");
			headLightCenter_part->SetValue("helper", "headLightCenter");
			SmartScriptTable headLightCenter_part_light(gEnv->pScriptSystem);
			headLightCenter_part_light->SetValue("type", "HeadLightDynamic");
			headLightCenter_part->SetValue("Light", headLightCenter_part_light);
			parts->PushBack(headLightCenter_part);
		}
		{
			SmartScriptTable headLightLeft_part(gEnv->pScriptSystem);
			headLightLeft_part->SetValue("name", "headLightLeft");
			headLightLeft_part->SetValue("class", "Light");
			headLightLeft_part->SetValue("component", "headLightLeft");
			headLightLeft_part->SetValue("helper", "headLightLeft");
			SmartScriptTable headLightLeft_part_light(gEnv->pScriptSystem);
			headLightLeft_part_light->SetValue("type", "HeadLightBeam");
			headLightLeft_part_light->SetValue("coronaDistIntensity", -1.0f);
			headLightLeft_part_light->SetValue("coronaDistSize", -1.0f);
			headLightLeft_part->SetValue("Light", headLightLeft_part_light);
			parts->PushBack(headLightLeft_part);
		}
		{
			SmartScriptTable headLightLeftFlare_part(gEnv->pScriptSystem);
			headLightLeftFlare_part->SetValue("name", "headLightLeftFlare");
			headLightLeftFlare_part->SetValue("class", "Light");
			headLightLeftFlare_part->SetValue("component", "headLightLeft");
			headLightLeftFlare_part->SetValue("helper", "headLightLeft");
			SmartScriptTable headLightLeftFlare_part_light(gEnv->pScriptSystem);
			headLightLeftFlare_part_light->SetValue("type", "HeadLightFlare");
			headLightLeftFlare_part->SetValue("Light", headLightLeftFlare_part_light);
			parts->PushBack(headLightLeftFlare_part);
		}
		{
			SmartScriptTable headLightRight_part(gEnv->pScriptSystem);
			headLightRight_part->SetValue("name", "headLightRight");
			headLightRight_part->SetValue("class", "Light");
			headLightRight_part->SetValue("component", "headLightRight");
			headLightRight_part->SetValue("helper", "headLightRight");
			SmartScriptTable headLightRight_part_light(gEnv->pScriptSystem);
			headLightRight_part_light->SetValue("type", "HeadLightBeam");
			headLightRight_part->SetValue("Light", headLightRight_part_light);
			parts->PushBack(headLightRight_part);
		}
		{
			SmartScriptTable headLightRightFlare_part(gEnv->pScriptSystem);
			headLightRightFlare_part->SetValue("name", "headLightRightFlare");
			headLightRightFlare_part->SetValue("class", "Light");
			headLightRightFlare_part->SetValue("component", "headLightRight");
			headLightRightFlare_part->SetValue("helper", "headLightRight");
			SmartScriptTable headLightRightFlare_part_light(gEnv->pScriptSystem);
			headLightRightFlare_part_light->SetValue("type", "HeadLightFlare");
			headLightRightFlare_part->SetValue("Light", headLightRightFlare_part_light);
			parts->PushBack(headLightRightFlare_part);
		}

		this->LoadParts(parts, nullptr, componentParts);

		for (auto& [name, pPart] : m_parts)
		{
			pPart->ChangeState(IVehiclePart::eVGS_Default);
		}
	}

	// Helpers
	this->InitHelpers();

	// Components
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Hull");
		component->SetValue("damageMax", 350.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.35f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.7f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.25f);
			damagemultiplier->SetValue("splash", 0.35f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.2f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_Destroy_group(gEnv->pScriptSystem);
			damagebehavior_Destroy_group->SetValue("name", "Destroy");
			damagebehavior->SetValue("Group", damagebehavior_Destroy_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "VehicleDestroyed");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			damagebehavior->SetValue("damageRatioMax", 0.75f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "HullDamaged25");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.75f);
			damagebehavior->SetValue("damageRatioMax", 1.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "HullDamaged75");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* hull = new VehicleComponent();
		hull->Init(this, component);
		m_components.push_back(hull);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "FlippedOver");
		component->SetValue("damageMax", 1.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "VehicleDestroyed");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Burn");
			SmartScriptTable damagebehavior_burn(gEnv->pScriptSystem);
			damagebehavior_burn->SetValue("helper", "");
			damagebehavior_burn->SetValue("damage", 0.0f);
			damagebehavior_burn->SetValue("selfDamage", 100.0f);
			damagebehavior_burn->SetValue("radius", 5.0f);
			damagebehavior_burn->SetValue("interval", 1.0f);
			damagebehavior->SetValue("Burn", damagebehavior_burn);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* flippedOver = new VehicleComponent();
		flippedOver->Init(this, component);
		m_components.push_back(flippedOver);
	}

	this->RegisterComponentParts(componentParts);

	// Particles
	{
		SmartScriptTable particles(gEnv->pScriptSystem);
		SmartScriptTable particles_exhaust(gEnv->pScriptSystem);
		particles_exhaust->SetValue("insideWater", false);
		particles_exhaust->SetValue("outsideWater", true);
		SmartScriptTable particles_exhaust_helpers(gEnv->pScriptSystem);
		particles_exhaust_helpers->PushBack("exhaustPos");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "");
		particles_exhaust_enginerunning->SetValue("boostEffect", "");
		particles_exhaust_enginerunning->SetValue("baseSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeed", 20.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPower", 0.0f);
		particles_exhaust_enginerunning->SetValue("minPowerSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPowerCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPowerSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPower", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerSpeedScale", 1.0f);
		particles_exhaust->SetValue("EngineRunning", particles_exhaust_enginerunning);
		particles->SetValue("Exhaust", particles_exhaust);
		SmartScriptTable particles_shattereffect(gEnv->pScriptSystem);
		particles_shattereffect->SetValue("effect", "vehicle_fx.frozen_shatter.small");
		particles->SetValue("ShatterEffect", particles_shattereffect);
		SmartScriptTable particles_damageeffects(gEnv->pScriptSystem);
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "vehicleBurnPos");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.Large_Vehicle.a");
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "burningPos");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.UShovercraft_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.UShovercraft_hull_smoke_b");
			HullDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 0.0f);
			environmentlayer->SetValue("minSpeedSizeScale", 0.5f);
			environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			environmentlayer->SetValue("minSpeedSpeedScale", 1.0f);
			environmentlayer->SetValue("maxSpeed", 15.0f);
			environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			environmentlayer->SetValue("maxSpeedSpeedScale", 1.0f);
			SmartScriptTable environmentlayer_alignment(gEnv->pScriptSystem);
			environmentlayer_alignment->SetValue("alignGroundHeight", 2.0f);
			environmentlayer_alignment->SetValue("maxHeightSizeScale", 0.0f);
			environmentlayer_alignment->SetValue("maxHeightCountScale", 0.75f);
			environmentlayer_alignment->SetValue("alignToWater", false);
			environmentlayer->SetValue("Alignment", environmentlayer_alignment);
			SmartScriptTable environmentlayer_emitters(gEnv->pScriptSystem);
			environmentlayer->SetValue("Emitters", environmentlayer_emitters);
			SmartScriptTable environmentlayer_wheels(gEnv->pScriptSystem);
			environmentlayer->SetValue("Wheels", environmentlayer_wheels);
			particles_environmentlayers->PushBack(environmentlayer);
		}
		particles->SetValue("EnvironmentLayers", particles_environmentlayers);

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Particles", particles);
		this->InitParticles(table);
	}

	// Animations
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "rudders");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable _state(gEnv->pScriptSystem);
			_state->SetValue("name", "");
			_state->SetValue("animation", "hovercraft_b_thrusters");
			_state->SetValue("isLooped", true);
			_state->SetValue("speedDefault", 1.0f);
			_state->SetValue("speedMin", 1.0f);
			_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* rudders = new VehicleAnimation();
		rudders->Init(this, animation);
		m_animations.push_back({ "rudders", rudders });
	}

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("hoverHeight", 0.85000002f);
		movement->SetValue("hoverVariance", 0.0f);
		movement->SetValue("hoverFrequency", 0.0f);
		movement->SetValue("numThrusters", 9);
		movement->SetValue("thrusterBottomOffset", 1.0f);
		movement->SetValue("thrusterMaxHeightCoeff", 1.5f);
		movement->SetValue("stiffness", 1.4f);
		movement->SetValue("damping", 1.0f);
		movement->SetValue("dampingLimit", 2.0f);
		movement->SetValue("thrusterTilt", 40.0f);
		movement->SetValue("velMax", 26.0f);
		movement->SetValue("velMaxReverse", 8.0f);
		movement->SetValue("acceleration", 11.0f);
		movement->SetValue("accelerationMultiplier", 1.6f);
		movement->SetValue("pushOffset", Vec3(0, -2.5, 0));
		movement->SetValue("pushTilt", 5.0f);
		movement->SetValue("linearDamping", 0.75f);
		movement->SetValue("turnRateMax", 1.2f);
		movement->SetValue("turnRateReverse", 0.89999998f);
		movement->SetValue("turnAccel", 1.0f);
		movement->SetValue("turnAccelMultiplier", 2.2f);
		movement->SetValue("turnDamping", 0.80000001f);
		movement->SetValue("cornerForce", 1.9f);
		movement->SetValue("cornerOffset", Vec3(0, 0, 1.2));
		movement->SetValue("cornerTilt", 0.0f);
		movement->SetValue("sampleByHelpers", false);
		movement->SetValue("thrusterHeightAdaption", 0.0f);
		movement->SetValue("thrusterUpdate", 0.0f);
		movement->SetValue("bEngineAlwaysOn", false);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("rpmPitchSpeed", 1.5f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable velMax_tweak(gEnv->pScriptSystem);
				velMax_tweak->SetValue("name", "velMax");
				velMax_tweak->SetValue("value", 30.0f);
				velMax_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(velMax_tweak);
			}
			{
				SmartScriptTable acceleration_tweak(gEnv->pScriptSystem);
				acceleration_tweak->SetValue("name", "acceleration");
				acceleration_tweak->SetValue("value", 14.0f);
				acceleration_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(acceleration_tweak);
			}
			{
				SmartScriptTable pushTilt_tweak(gEnv->pScriptSystem);
				pushTilt_tweak->SetValue("name", "pushTilt");
				pushTilt_tweak->SetValue("value", 10.0f);
				pushTilt_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(pushTilt_tweak);
			}
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);
		SmartScriptTable movement_airdamp(gEnv->pScriptSystem);
		movement_airdamp->SetValue("dampAngle", Vec3(1.5, 1, 0));
		movement_airdamp->SetValue("dampAngVel", Vec3(1.5, 1.5, 0));
		movement->SetValue("AirDamp", movement_airdamp);

		if (!this->SetMovement("Hovercraft", movement))
		{
			return false;
		}
	}

	SmartScriptTable seats(gEnv->pScriptSystem);

	// Seats
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "driver");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "driverExitPos");
		seat->SetValue("exitHelper", "driverExitPos");
		seat->SetValue("sitHelper", "driverSitPos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "shiten");
		seat->SetValue("seatGroupIndex", 1);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "driverEyePos");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -7, 1));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 2, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "RotateTurret");
			SmartScriptTable seataction_rotateturret(gEnv->pScriptSystem);
			seataction_rotateturret->SetValue("pitchPart", "shiten");
			seataction_rotateturret->SetValue("yawPart", "shitenTurret");
			seataction->SetValue("RotateTurret", seataction_rotateturret);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Weapons");
			SmartScriptTable seataction_weapons(gEnv->pScriptSystem);
			SmartScriptTable seataction_weapons_weapons(gEnv->pScriptSystem);
			{
				SmartScriptTable weapon(gEnv->pScriptSystem);
				if (m_modName == "MP")
				{
					weapon->SetValue("class", "");
				}
				else if (m_modName == "Gauss")
				{
					weapon->SetValue("class", "VehicleGaussMounted");
				}
				else if (m_modName == "MOAC")
				{
					weapon->SetValue("class", "VehicleMOACMounted");
				}
				else if (m_modName == "MOAR")
				{
					weapon->SetValue("class", "VehicleMOARMounted");
				}
				else
				{
					weapon->SetValue("class", "HovercraftGun");
				}
				weapon->SetValue("part", "shiten");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon->SetValue("Helpers", weapon_helpers);
				seataction_weapons_weapons->PushBack(weapon);
			}
			seataction_weapons->SetValue("Weapons", seataction_weapons_weapons);
			seataction->SetValue("Weapons", seataction_weapons);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "SteeringWheel");
			SmartScriptTable seataction_steeringwheel(gEnv->pScriptSystem);
			seataction_steeringwheel->SetValue("vehicleAnimation", "rudders");
			SmartScriptTable seataction_steeringwheel_actions(gEnv->pScriptSystem);
			seataction_steeringwheel_actions->SetValue("anglesMax", Vec3(0, 130, 0));
			seataction_steeringwheel_actions->SetValue("steeringForce", 5.0f);
			seataction_steeringwheel->SetValue("Actions", seataction_steeringwheel_actions);
			seataction->SetValue("SteeringWheel", seataction_steeringwheel);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Lights");
			SmartScriptTable seataction_lights(gEnv->pScriptSystem);
			seataction_lights->SetValue("activation", "toggle");
			SmartScriptTable seataction_lights_lightparts(gEnv->pScriptSystem);
			seataction_lights_lightparts->PushBack("headLightLeft");
			seataction_lights_lightparts->PushBack("headLightLeftFlare");
			seataction_lights_lightparts->PushBack("headLightRight");
			seataction_lights_lightparts->PushBack("headLightRightFlare");
			seataction_lights_lightparts->PushBack("headLightCenter");
			seataction_lights->SetValue("LightParts", seataction_lights_lightparts);
			seataction->SetValue("Lights", seataction_lights);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Sound");
			SmartScriptTable seataction_sound(gEnv->pScriptSystem);
			seataction_sound->SetValue("sound", "Sounds/vehicles:us_hovercraft:horn");
			seataction_sound->SetValue("helper", "turret_pos");
			seataction->SetValue("Sound", seataction_sound);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* driver = new VehicleSeat();
		driver->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "driver", driver });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger01");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger01_exit_pos");
		seat->SetValue("sitHelper", "passenger01_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -7, 1));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 2, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger01 = new VehicleSeat();
		passenger01->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger01", passenger01 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger02");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger02_exit_pos");
		seat->SetValue("sitHelper", "passenger02_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -7, 1));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 2, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger02 = new VehicleSeat();
		passenger02->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger02", passenger02 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger03");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger03_exit_pos");
		seat->SetValue("sitHelper", "passenger03_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -7, 1));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 2, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger03 = new VehicleSeat();
		passenger03->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger03", passenger03 });
	}

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetValue("Seats", seats);
	}

	// SeatTransitions
	this->InitSeatTransitions();

	// Paints
	if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/us_hovercraft_b/us_hovercraft_nk.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/us_hovercraft_b/us_hovercraft_b.mtl");
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("submergedRatioMax", 1.0f);
		damages->SetValue("submergedDamageMult", 1.0f);
		damages->SetValue("collDamageThreshold", 50.0f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.001f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.02f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.15f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.05f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.05f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.25f);
			damagemultiplier->SetValue("splash", 0.25f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.02f);
			damagemultiplier->SetValue("splash", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "c4");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 2.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.05f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		damages->SetValue("DamageMultipliers", damages_damagemultipliers);
		SmartScriptTable damages_damagesgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable Destroy_damagesgroup(gEnv->pScriptSystem);
			Destroy_damagesgroup->SetValue("name", "Destroy");
			Destroy_damagesgroup->SetValue("useTemplate", "BoatDestroy");
			SmartScriptTable Destroy_damagesgroup_damagessubgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.0f);
				damagessubgroup->SetValue("randomness", 0.5f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Impulse");
					damagebehavior->SetValue("damageRatioMin", 1.0f);
					damagebehavior->SetValue("ignoreVehicleDestruction", false);
					SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
					damagebehavior_impulse->SetValue("forceMin", 6.0f);
					damagebehavior_impulse->SetValue("forceMax", 7.0f);
					damagebehavior_impulse->SetValue("direction", Vec3(0, 0, 1));
					damagebehavior_impulse->SetValue("momentum", Vec3(0.75, 0.85000002, 0));
					damagebehavior_impulse->SetValue("helper", "wakePos");
					damagebehavior->SetValue("Impulse", damagebehavior_impulse);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
				Destroy_damagesgroup_damagessubgroups->PushBack(damagessubgroup);
			}
			Destroy_damagesgroup->SetValue("DamagesSubGroups", Destroy_damagesgroup_damagessubgroups);
			damages_damagesgroups->PushBack(Destroy_damagesgroup);
		}
		damages->SetValue("DamagesGroups", damages_damagesgroups);

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Damages", damages);
		this->InitDamages(table);
	}

	this->InitMaxDamage();
	this->AttachScriptBindToSeats();

	return this->BindVehicleToNetwork();
}
