#include "US_smallboat.h"

US_smallboat::US_smallboat()
{
	// enable US_smallboat::Init function
	this->Vehicle::PatchVTable();
}

US_smallboat::~US_smallboat()
{
}

bool US_smallboat::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "seavehicle";

	// Physics
	m_physics.damping = 0.050000001f;
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 70.0f;
	m_buoyancy.waterResistance = 300.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.16f;
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
			chassis_part->SetValue("disablePhysics", false);
			SmartScriptTable chassis_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable steering_wheel_part(gEnv->pScriptSystem);
				steering_wheel_part->SetValue("name", "steering_wheel");
				steering_wheel_part->SetValue("class", "AnimatedJoint");
				steering_wheel_part->SetValue("mass", 0.0f);
				SmartScriptTable steering_wheel_part_animatedjoint(gEnv->pScriptSystem);
				steering_wheel_part->SetValue("AnimatedJoint", steering_wheel_part_animatedjoint);
				chassis_part_parts->PushBack(steering_wheel_part);
			}
			{
				SmartScriptTable shitenTurret_part(gEnv->pScriptSystem);
				shitenTurret_part->SetValue("name", "shitenTurret");
				shitenTurret_part->SetValue("class", "AnimatedJoint");
				shitenTurret_part->SetValue("mass", 0.0f);
				SmartScriptTable shitenTurret_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable shiten_part(gEnv->pScriptSystem);
					shiten_part->SetValue("name", "shiten");
					shiten_part->SetValue("class", "AnimatedJoint");
					shiten_part->SetValue("mass", 0.0f);
					SmartScriptTable shiten_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable gunnerEyePos_helper(gEnv->pScriptSystem);
						gunnerEyePos_helper->SetValue("name", "gunnerEyePos");
						gunnerEyePos_helper->SetValue("position", Vec3(0.0038000001, 3.6566999, 1.4743));
						gunnerEyePos_helper->SetValue("direction", Vec3(0, 1, 0.001));
						shiten_part_helpers->PushBack(gunnerEyePos_helper);
					}
					{
						SmartScriptTable searchLightPos_helper(gEnv->pScriptSystem);
						searchLightPos_helper->SetValue("name", "searchLightPos");
						searchLightPos_helper->SetValue("position", Vec3(0.16, 4.26, 1.17));
						searchLightPos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(searchLightPos_helper);
					}
					shiten_part->SetValue("Helpers", shiten_part_helpers);
					SmartScriptTable shiten_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable shiten_part_animatedjoint_rotation(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation->SetValue("pitchSpeed", 60.0f);
					shiten_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
					SmartScriptTable shiten_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation_pitchlimits->PushBack(-10.0f);
					shiten_part_animatedjoint_rotation_pitchlimits->PushBack(60.0f);
					shiten_part_animatedjoint_rotation->SetValue("pitchLimits", shiten_part_animatedjoint_rotation_pitchlimits);
					shiten_part_animatedjoint->SetValue("Rotation", shiten_part_animatedjoint_rotation);
					shiten_part->SetValue("AnimatedJoint", shiten_part_animatedjoint);
					shitenTurret_part_parts->PushBack(shiten_part);
				}
				{
					SmartScriptTable searchLight_part(gEnv->pScriptSystem);
					searchLight_part->SetValue("name", "searchLight");
					searchLight_part->SetValue("class", "Light");
					searchLight_part->SetValue("component", "searchLight");
					searchLight_part->SetValue("helper", "searchLightPos");
					SmartScriptTable searchLight_part_light(gEnv->pScriptSystem);
					searchLight_part_light->SetValue("type", "HeadLightDynamic");
					searchLight_part_light->SetValue("radius", 30.0f);
					searchLight_part_light->SetValue("diffuseMult", 15.0f);
					searchLight_part_light->SetValue("directFactor", 10.0f);
					searchLight_part->SetValue("Light", searchLight_part_light);
					shitenTurret_part_parts->PushBack(searchLight_part);
				}
				{
					SmartScriptTable gunnerLightFlare_part(gEnv->pScriptSystem);
					gunnerLightFlare_part->SetValue("name", "gunnerLightFlare");
					gunnerLightFlare_part->SetValue("class", "Light");
					gunnerLightFlare_part->SetValue("component", "searchLight");
					gunnerLightFlare_part->SetValue("helper", "searchLightPos");
					SmartScriptTable gunnerLightFlare_part_light(gEnv->pScriptSystem);
					gunnerLightFlare_part_light->SetValue("type", "HeadLightFlare");
					gunnerLightFlare_part->SetValue("Light", gunnerLightFlare_part_light);
					shitenTurret_part_parts->PushBack(gunnerLightFlare_part);
				}
				shitenTurret_part->SetValue("Parts", shitenTurret_part_parts);
				SmartScriptTable shitenTurret_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable helper_passenger02_pos_helper(gEnv->pScriptSystem);
					helper_passenger02_pos_helper->SetValue("name", "helper_passenger02_pos");
					helper_passenger02_pos_helper->SetValue("position", Vec3(0, 0, 0));
					helper_passenger02_pos_helper->SetValue("direction", Vec3(0, 1, 0));
					shitenTurret_part_helpers->PushBack(helper_passenger02_pos_helper);
				}
				shitenTurret_part->SetValue("Helpers", shitenTurret_part_helpers);
				SmartScriptTable shitenTurret_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable shitenTurret_part_animatedjoint_rotation(gEnv->pScriptSystem);
				shitenTurret_part_animatedjoint_rotation->SetValue("yawSpeed", 130.0f);
				shitenTurret_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
				SmartScriptTable shitenTurret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(-120.0f);
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(120.0f);
				shitenTurret_part_animatedjoint_rotation->SetValue("yawLimits", shitenTurret_part_animatedjoint_rotation_yawlimits);
				shitenTurret_part_animatedjoint->SetValue("Rotation", shitenTurret_part_animatedjoint_rotation);
				shitenTurret_part->SetValue("AnimatedJoint", shitenTurret_part_animatedjoint);
				chassis_part_parts->PushBack(shitenTurret_part);
			}
			chassis_part->SetValue("Parts", chassis_part_parts);
			SmartScriptTable chassis_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable propeller_left_pos_helper(gEnv->pScriptSystem);
				propeller_left_pos_helper->SetValue("name", "propeller_left_pos");
				propeller_left_pos_helper->SetValue("position", Vec3(-0.60210001, -5.0022001, 0.085600004));
				propeller_left_pos_helper->SetValue("direction", Vec3(0, -0.76639998, 0.64230001));
				chassis_part_helpers->PushBack(propeller_left_pos_helper);
			}
			{
				SmartScriptTable propeller_right_pos_helper(gEnv->pScriptSystem);
				propeller_right_pos_helper->SetValue("name", "propeller_right_pos");
				propeller_right_pos_helper->SetValue("position", Vec3(0.65310001, -5.0022001, 0.084600002));
				propeller_right_pos_helper->SetValue("direction", Vec3(0, -0.76639998, 0.64230001));
				chassis_part_helpers->PushBack(propeller_right_pos_helper);
			}
			{
				SmartScriptTable driver_eye_pos_helper(gEnv->pScriptSystem);
				driver_eye_pos_helper->SetValue("name", "driver_eye_pos");
				driver_eye_pos_helper->SetValue("position", Vec3(0, 0.99980003, 1.4978));
				driver_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(driver_eye_pos_helper);
			}
			{
				SmartScriptTable leftEngineSmokePos_helper(gEnv->pScriptSystem);
				leftEngineSmokePos_helper->SetValue("name", "leftEngineSmokePos");
				leftEngineSmokePos_helper->SetValue("position", Vec3(-0.60009998, -4.9001002, 0.00039999999));
				leftEngineSmokePos_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(leftEngineSmokePos_helper);
			}
			{
				SmartScriptTable rightEngineSmokePos_helper(gEnv->pScriptSystem);
				rightEngineSmokePos_helper->SetValue("name", "rightEngineSmokePos");
				rightEngineSmokePos_helper->SetValue("position", Vec3(0.60009998, -4.9001002, -0.0057999999));
				rightEngineSmokePos_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(rightEngineSmokePos_helper);
			}
			{
				SmartScriptTable wakePos_helper(gEnv->pScriptSystem);
				wakePos_helper->SetValue("name", "wakePos");
				wakePos_helper->SetValue("position", Vec3(0, 2.5160999, -1.2959));
				wakePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(wakePos_helper);
			}
			{
				SmartScriptTable sprayLeftPos_helper(gEnv->pScriptSystem);
				sprayLeftPos_helper->SetValue("name", "sprayLeftPos");
				sprayLeftPos_helper->SetValue("position", Vec3(-0.47220001, 2.0427001, 0.061299998));
				sprayLeftPos_helper->SetValue("direction", Vec3(-1, 0, 0));
				chassis_part_helpers->PushBack(sprayLeftPos_helper);
			}
			{
				SmartScriptTable sprayRightPos_helper(gEnv->pScriptSystem);
				sprayRightPos_helper->SetValue("name", "sprayRightPos");
				sprayRightPos_helper->SetValue("position", Vec3(0.47679999, 2.0014999, 0.067299999));
				sprayRightPos_helper->SetValue("direction", Vec3(1, 0, 0));
				chassis_part_helpers->PushBack(sprayRightPos_helper);
			}
			{
				SmartScriptTable splashPos_helper(gEnv->pScriptSystem);
				splashPos_helper->SetValue("name", "splashPos");
				splashPos_helper->SetValue("position", Vec3(0, 4.3123002, -1.1456));
				splashPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(splashPos_helper);
			}
			{
				SmartScriptTable cornerForcePos_helper(gEnv->pScriptSystem);
				cornerForcePos_helper->SetValue("name", "cornerForcePos");
				cornerForcePos_helper->SetValue("position", Vec3(0, -0.25560001, 0.17));
				cornerForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(cornerForcePos_helper);
			}
			{
				SmartScriptTable pushForcePos_helper(gEnv->pScriptSystem);
				pushForcePos_helper->SetValue("name", "pushForcePos");
				pushForcePos_helper->SetValue("position", Vec3(0, -3, -0.80400002));
				pushForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(pushForcePos_helper);
			}
			{
				SmartScriptTable engineSoundPos_helper(gEnv->pScriptSystem);
				engineSoundPos_helper->SetValue("name", "engineSoundPos");
				engineSoundPos_helper->SetValue("position", Vec3(0.039099999, -4.8112998, 0.1173));
				engineSoundPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(engineSoundPos_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(0, 0, -0.1886));
				centerPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(centerPos_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, 0.64920002, -0.0081000002));
				burningPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(burningPos_helper);
			}
			chassis_part->SetValue("Helpers", chassis_part_helpers);
			SmartScriptTable chassis_part_animated(gEnv->pScriptSystem);
			if (m_modName == "Asian")
			{
				chassis_part_animated->SetValue("filename", "objects/vehicles/asian_smallboat/asian_smallboat.cga");
			}
			else if (m_modName == "Asian_MP")
			{
				chassis_part_animated->SetValue("filename", "objects/vehicles/asian_smallboat/asian_smallboat.cga");
			}
			else
			{
				chassis_part_animated->SetValue("filename", "Objects/Vehicles/US_Smallboat/US_Smallboat.cga");
			}
			if (m_modName == "Asian")
			{
				chassis_part_animated->SetValue("filenameDestroyed", "objects/vehicles/asian_smallboat/asian_smallboat_damaged.cga");
			}
			else if (m_modName == "Asian_MP")
			{
				chassis_part_animated->SetValue("filenameDestroyed", "objects/vehicles/asian_smallboat/asian_smallboat_damaged.cga");
			}
			else
			{
				chassis_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/US_Smallboat/US_Smallboat_damaged.cga");
			}
			chassis_part->SetValue("Animated", chassis_part_animated);
			parts->PushBack(chassis_part);
		}
		{
			SmartScriptTable mass_part(gEnv->pScriptSystem);
			mass_part->SetValue("name", "mass");
			mass_part->SetValue("class", "MassBox");
			mass_part->SetValue("mass", 1500.0f);
			mass_part->SetValue("position", Vec3(0, -0.38890001, -0.4));
			SmartScriptTable mass_part_massbox(gEnv->pScriptSystem);
			mass_part_massbox->SetValue("size", Vec3(1.80001, 4.5, 0.6));
			mass_part->SetValue("MassBox", mass_part_massbox);
			parts->PushBack(mass_part);
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
		component->SetValue("damageMax", 250.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.06f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.15f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 2.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "c4");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
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
			damagebehavior->SetValue("class", "Indicator");
			damagebehavior->SetValue("damageRatioMin", 0.75f);
			SmartScriptTable damagebehavior_indicator(gEnv->pScriptSystem);
			SmartScriptTable damagebehavior_indicator_light(gEnv->pScriptSystem);
			damagebehavior_indicator_light->SetValue("material", "");
			damagebehavior_indicator_light->SetValue("sound", "sounds/vehicles:vehicle_accessories:warning_beep_boat");
			damagebehavior_indicator_light->SetValue("soundRatioMin", 0.75f);
			damagebehavior_indicator_light->SetValue("helper", "");
			damagebehavior_indicator->SetValue("Light", damagebehavior_indicator_light);
			damagebehavior->SetValue("Indicator", damagebehavior_indicator);
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
		component->SetValue("name", "leftEngine");
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-0.92989999, -5.3101001, -1.4));
		component->SetValue("maxBound", Vec3(-0.30000001, -4.48, 0.31));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.2f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 2.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.01f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.1f);
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
			damagebehavior->SetValue("class", "MovementNotification");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "leftEngineSmoke");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* leftEngine = new VehicleComponent();
		leftEngine->Init(this, component);
		m_components.push_back(leftEngine);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "rightEngine");
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0.3301, -5.3101001, -1.4));
		component->SetValue("maxBound", Vec3(0.95999998, -4.48, 0.31));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.2f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 2.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.01f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.1f);
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
			damagebehavior->SetValue("class", "MovementNotification");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "rightEngineSmoke");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* rightEngine = new VehicleComponent();
		rightEngine->Init(this, component);
		m_components.push_back(rightEngine);
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
			damagebehavior_effect->SetValue("effect", "VehicleFlipped");
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
			damagebehavior_burn->SetValue("selfDamage", 40.0f);
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
		particles_exhaust->SetValue("insideWater", true);
		particles_exhaust->SetValue("outsideWater", false);
		SmartScriptTable particles_exhaust_helpers(gEnv->pScriptSystem);
		particles_exhaust_helpers->PushBack("propeller_right_pos");
		particles_exhaust_helpers->PushBack("propeller_left_pos");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "vehicle_fx.vehicles_surface_fx.small_boat");
		particles_exhaust_enginerunning->SetValue("boostEffect", "");
		particles_exhaust_enginerunning->SetValue("baseSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 0.69999999f);
		particles_exhaust_enginerunning->SetValue("minSpeedSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeed", 20.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedSizeScale", 1.4f);
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
		particles_exhaust_enginerunning->SetValue("disableWithNegativePower", true);
		particles_exhaust->SetValue("EngineRunning", particles_exhaust_enginerunning);
		particles->SetValue("Exhaust", particles_exhaust);
		SmartScriptTable particles_shattereffect(gEnv->pScriptSystem);
		particles_shattereffect->SetValue("effect", "vehicle_fx.frozen_shatter.small");
		particles->SetValue("ShatterEffect", particles_shattereffect);
		SmartScriptTable particles_damageeffects(gEnv->pScriptSystem);
		{
			SmartScriptTable leftEngineSmoke_damageeffect(gEnv->pScriptSystem);
			leftEngineSmoke_damageeffect->SetValue("name", "leftEngineSmoke");
			leftEngineSmoke_damageeffect->SetValue("helper", "leftEngineSmokePos");
			leftEngineSmoke_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.USsmallBoat_engine_smoke_b");
			particles_damageeffects->PushBack(leftEngineSmoke_damageeffect);
		}
		{
			SmartScriptTable rightEngineSmoke_damageeffect(gEnv->pScriptSystem);
			rightEngineSmoke_damageeffect->SetValue("name", "rightEngineSmoke");
			rightEngineSmoke_damageeffect->SetValue("helper", "rightEngineSmokePos");
			rightEngineSmoke_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.USsmallBoat_engine_smoke_b");
			particles_damageeffects->PushBack(rightEngineSmoke_damageeffect);
		}
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "driver_eye_pos");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.Large_Vehicle.a");
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable VehicleFlipped_damageeffect(gEnv->pScriptSystem);
			VehicleFlipped_damageeffect->SetValue("name", "VehicleFlipped");
			VehicleFlipped_damageeffect->SetValue("helper", "centerPos");
			VehicleFlipped_damageeffect->SetValue("effect", "smoke_and_fire.Jeep.flipped_jeep");
			particles_damageeffects->PushBack(VehicleFlipped_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "burningPos");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.USsmallBoat_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.USsmallBoat_hull_smoke_b");
			HullDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 1.0f);
			environmentlayer->SetValue("minSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			environmentlayer->SetValue("minSpeedSpeedScale", 1.0f);
			environmentlayer->SetValue("maxSpeed", 10.0f);
			environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			environmentlayer->SetValue("maxSpeedSpeedScale", 1.0f);
			environmentlayer->SetValue("minPowerSizeScale", 1.0f);
			environmentlayer->SetValue("minPowerCountScale", 1.0f);
			environmentlayer->SetValue("maxPowerSizeScale", 1.0f);
			environmentlayer->SetValue("maxPowerCountScale", 1.0f);
			SmartScriptTable environmentlayer_alignment(gEnv->pScriptSystem);
			environmentlayer_alignment->SetValue("alignGroundHeight", 0.0f);
			environmentlayer_alignment->SetValue("maxHeightSizeScale", 1.0f);
			environmentlayer_alignment->SetValue("maxHeightCountScale", 1.0f);
			environmentlayer_alignment->SetValue("alignToWater", true);
			environmentlayer->SetValue("Alignment", environmentlayer_alignment);
			SmartScriptTable environmentlayer_emitters(gEnv->pScriptSystem);
			environmentlayer_emitters->PushBack("wakePos");
			environmentlayer->SetValue("Emitters", environmentlayer_emitters);
			particles_environmentlayers->PushBack(environmentlayer);
		}
		{
			SmartScriptTable spray_environmentlayer(gEnv->pScriptSystem);
			spray_environmentlayer->SetValue("name", "spray");
			spray_environmentlayer->SetValue("minSpeed", 0.1f);
			spray_environmentlayer->SetValue("minSpeedSizeScale", 0.5f);
			spray_environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			spray_environmentlayer->SetValue("maxSpeed", 1.0f);
			spray_environmentlayer->SetValue("maxSpeedSizeScale", 2.0f);
			spray_environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			spray_environmentlayer->SetValue("minPowerSizeScale", 1.0f);
			spray_environmentlayer->SetValue("minPowerCountScale", 1.0f);
			spray_environmentlayer->SetValue("maxPowerSizeScale", 1.0f);
			spray_environmentlayer->SetValue("maxPowerCountScale", 1.0f);
			SmartScriptTable spray_environmentlayer_alignment(gEnv->pScriptSystem);
			spray_environmentlayer_alignment->SetValue("alignGroundHeight", 0.0f);
			spray_environmentlayer_alignment->SetValue("maxHeightSizeScale", 1.0f);
			spray_environmentlayer_alignment->SetValue("maxHeightCountScale", 1.0f);
			spray_environmentlayer_alignment->SetValue("alignToWater", true);
			spray_environmentlayer->SetValue("Alignment", spray_environmentlayer_alignment);
			SmartScriptTable spray_environmentlayer_emitters(gEnv->pScriptSystem);
			spray_environmentlayer_emitters->PushBack("sprayLeftPos");
			spray_environmentlayer_emitters->PushBack("sprayRightPos");
			spray_environmentlayer->SetValue("Emitters", spray_environmentlayer_emitters);
			SmartScriptTable spray_environmentlayer_wheels(gEnv->pScriptSystem);
			spray_environmentlayer->SetValue("Wheels", spray_environmentlayer_wheels);
			particles_environmentlayers->PushBack(spray_environmentlayer);
		}
		particles->SetValue("EnvironmentLayers", particles_environmentlayers);

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Particles", particles);
		this->InitParticles(table);
	}

	// Animations
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "steeringWheel");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable _state(gEnv->pScriptSystem);
			_state->SetValue("name", "");
			_state->SetValue("animation", "smallboat_steeringWheel");
			_state->SetValue("isLooped", true);
			_state->SetValue("speedDefault", 1.0f);
			_state->SetValue("speedMin", 1.0f);
			_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* steeringWheel = new VehicleAnimation();
		steeringWheel->Init(this, animation);
		m_animations.push_back({ "steeringWheel", steeringWheel });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "propeller");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable _state(gEnv->pScriptSystem);
			_state->SetValue("name", "");
			_state->SetValue("animation", "smallboat_rotor");
			_state->SetValue("isLooped", true);
			_state->SetValue("speedDefault", 0.1f);
			_state->SetValue("speedMin", 0.1f);
			_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* propeller = new VehicleAnimation();
		propeller->Init(this, animation);
		m_animations.push_back({ "propeller", propeller });
	}

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("velMax", 27.0f);
		movement->SetValue("velMaxReverse", 10.0f);
		movement->SetValue("pedalLimitReverse", 1.0f);
		movement->SetValue("acceleration", 5.5f);
		movement->SetValue("accelerationVelMax", 0.60000002f);
		movement->SetValue("accelerationMultiplier", 1.2f);
		movement->SetValue("pushHelper", "pushForcePos");
		movement->SetValue("pushTilt", 0.0f);
		movement->SetValue("turnRateMax", 0.89999998f);
		movement->SetValue("turnAccel", 0.89999998f);
		movement->SetValue("turnAccelMultiplier", 1.35f);
		movement->SetValue("turnDamping", 0.2f);
		movement->SetValue("cornerForce", 2.5f);
		movement->SetValue("cornerHelper", "cornerForcePos");
		movement->SetValue("cornerTilt", 0.0f);
		movement->SetValue("waveIdleStrength", Vec3(0.80000001, 0.2, 0));
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSoundPos");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("rpmPitchSpeed", 1.0f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_animations(gEnv->pScriptSystem);
		movement_animations->SetValue("engine", "propeller");
		movement->SetValue("Animations", movement_animations);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_tweakgroup(gEnv->pScriptSystem);
			player_tweakgroup->SetValue("name", "player");
			SmartScriptTable player_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable velMax_tweak(gEnv->pScriptSystem);
				velMax_tweak->SetValue("name", "velMax");
				velMax_tweak->SetValue("value", 18.0f);
				velMax_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(velMax_tweak);
			}
			player_tweakgroup->SetValue("Tweaks", player_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_tweakgroup);
		}
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable velMax_tweak(gEnv->pScriptSystem);
				velMax_tweak->SetValue("name", "velMax");
				velMax_tweak->SetValue("value", 27.0f);
				velMax_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(velMax_tweak);
			}
			{
				SmartScriptTable acceleration_tweak(gEnv->pScriptSystem);
				acceleration_tweak->SetValue("name", "acceleration");
				acceleration_tweak->SetValue("value", 8.5f);
				acceleration_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(acceleration_tweak);
			}
			{
				SmartScriptTable pushTilt_tweak(gEnv->pScriptSystem);
				pushTilt_tweak->SetValue("name", "pushTilt");
				pushTilt_tweak->SetValue("value", 13.5f);
				pushTilt_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(pushTilt_tweak);
			}
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		{
			SmartScriptTable ai_tweakgroup(gEnv->pScriptSystem);
			ai_tweakgroup->SetValue("name", "ai");
			SmartScriptTable ai_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable velMax_tweak(gEnv->pScriptSystem);
				velMax_tweak->SetValue("name", "velMax");
				velMax_tweak->SetValue("value", 22.0f);
				velMax_tweak->SetValue("op", 0);
				ai_tweakgroup_tweaks->PushBack(velMax_tweak);
			}
			ai_tweakgroup->SetValue("Tweaks", ai_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(ai_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);

		if (!this->SetMovement("StdBoat", movement))
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
		seat->SetValue("enterHelper", "helper_passenger01_enterLeft");
		seat->SetValue("exitOffsetPlayer", Vec3(2, 0, 0.4));
		seat->SetValue("sitHelper", "helper_passenger01_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("transitionAnim", true);
		if (m_modName == "MP")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else if (m_modName == "Asian_MP")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else if (m_modName == "Gauss")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else if (m_modName == "MOAC")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else if (m_modName == "MOAR")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else
		{
			seat->SetValue("remotelyUseActionsFromSeat", "gunner");
		}
		seat->SetValue("blurMult", 0.1f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMax", Vec3(0, 0, 65));
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "SteeringWheel");
			SmartScriptTable seataction_steeringwheel(gEnv->pScriptSystem);
			seataction_steeringwheel->SetValue("vehicleAnimation", "steeringWheel");
			SmartScriptTable seataction_steeringwheel_actions(gEnv->pScriptSystem);
			seataction_steeringwheel_actions->SetValue("anglesMax", Vec3(0, 130, 0));
			seataction_steeringwheel->SetValue("Actions", seataction_steeringwheel_actions);
			seataction->SetValue("SteeringWheel", seataction_steeringwheel);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Sound");
			SmartScriptTable seataction_sound(gEnv->pScriptSystem);
			seataction_sound->SetValue("sound", "Sounds/vehicles:us_smallboat:horn");
			seataction_sound->SetValue("helper", "centerPos");
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
		seat->SetValue("name", "gunner");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger02_enterLeft");
		seat->SetValue("exitOffsetPlayer", Vec3(2.7, 1, 0.4));
		seat->SetValue("sitHelper", "helper_passenger02_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "shiten");
		seat->SetValue("seatGroupIndex", 2);
		seat->SetValue("transitionAnim", true);
		seat->SetValue("blurMult", 0.1f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			view->SetValue("hidePlayer", true);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "gunnerEyePos");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			view->SetValue("isAvailableRemotely", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.5, 0.5));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "RotateTurret");
			seataction->SetValue("isAvailableRemotely", true);
			SmartScriptTable seataction_rotateturret(gEnv->pScriptSystem);
			seataction_rotateturret->SetValue("pitchPart", "shiten");
			seataction_rotateturret->SetValue("yawPart", "shitenTurret");
			seataction->SetValue("RotateTurret", seataction_rotateturret);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Weapons");
			seataction->SetValue("isAvailableRemotely", true);
			SmartScriptTable seataction_weapons(gEnv->pScriptSystem);
			SmartScriptTable seataction_weapons_weapons(gEnv->pScriptSystem);
			{
				SmartScriptTable weapon(gEnv->pScriptSystem);
				if (m_modName == "Asian")
				{
					weapon->SetValue("class", "VehicleShiTenV2");
				}
				else if (m_modName == "Asian_MP")
				{
					weapon->SetValue("class", "VehicleShiTenV2");
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
					weapon->SetValue("class", "VehicleUSMachinegun");
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
			seataction->SetValue("class", "Lights");
			seataction->SetValue("isAvailableRemotely", true);
			SmartScriptTable seataction_lights(gEnv->pScriptSystem);
			seataction_lights->SetValue("activation", "toggle");
			seataction_lights->SetValue("sound", 2);
			SmartScriptTable seataction_lights_lightparts(gEnv->pScriptSystem);
			seataction_lights_lightparts->PushBack("searchLight");
			seataction_lights_lightparts->PushBack("gunnerLightFlare");
			seataction_lights->SetValue("LightParts", seataction_lights_lightparts);
			seataction->SetValue("Lights", seataction_lights);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* gunner = new VehicleSeat();
		gunner->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "gunner", gunner });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger03");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger03_enter");
		seat->SetValue("sitHelper", "helper_passenger03_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
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
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger04");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger04_enter");
		seat->SetValue("sitHelper", "helper_passenger04_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		seats->PushBack(seat);

		VehicleSeat* passenger04 = new VehicleSeat();
		passenger04->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger04", passenger04 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger05");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger05_enter");
		seat->SetValue("sitHelper", "helper_passenger05_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
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

		VehicleSeat* passenger05 = new VehicleSeat();
		passenger05->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger05", passenger05 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger06");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger06_enterLeft");
		seat->SetValue("sitHelper", "helper_passenger06_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
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

		VehicleSeat* passenger06 = new VehicleSeat();
		passenger06->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger06", passenger06 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger07");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger07_enter");
		seat->SetValue("sitHelper", "helper_passenger07_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
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

		VehicleSeat* passenger07 = new VehicleSeat();
		passenger07->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger07", passenger07 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger08");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger08_enterLeft");
		seat->SetValue("sitHelper", "helper_passenger08_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
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

		VehicleSeat* passenger08 = new VehicleSeat();
		passenger08->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger08", passenger08 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger09");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "helper_passenger09_enterRight");
		seat->SetValue("sitHelper", "helper_passenger09_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0.75));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 2.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(3, 0.2, 0.25));
			view_actionthirdperson->SetValue("heightAboveWater", 0.75f);
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

		VehicleSeat* passenger09 = new VehicleSeat();
		passenger09->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger09", passenger09 });
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
		this->SetPaintMaterial("objects/vehicles/us_smallboat/us_smallboat_nk.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/us_smallboat/us_smallboat.mtl");
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("collDamageThreshold", 100.0f);
		damages->SetValue("vehicleCollisionDestructionSpeed", 16.0f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.15f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "c4");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
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
