#include "Asian_patrolboat.h"

Asian_patrolboat::Asian_patrolboat()
{
	// enable Asian_patrolboat::Init function
	this->Vehicle::PatchVTable();
}

Asian_patrolboat::~Asian_patrolboat()
{
}

bool Asian_patrolboat::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "seavehicle";

	// Physics
	m_physics.damping = 0.0f;
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 110.0f;
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
				SmartScriptTable shitenTurret_part(gEnv->pScriptSystem);
				shitenTurret_part->SetValue("name", "shitenTurret");
				shitenTurret_part->SetValue("class", "AnimatedJoint");
				shitenTurret_part->SetValue("mass", 0.0f);
				shitenTurret_part->SetValue("isHidden", false);
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
						gunnerEyePos_helper->SetValue("position", Vec3(0.0060999999, 6.8709002, 3.2750001));
						gunnerEyePos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(gunnerEyePos_helper);
					}
					{
						SmartScriptTable gunnerLightPos_helper(gEnv->pScriptSystem);
						gunnerLightPos_helper->SetValue("name", "gunnerLightPos");
						gunnerLightPos_helper->SetValue("position", Vec3(0.16, 7.5, 3.05));
						gunnerLightPos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(gunnerLightPos_helper);
					}
					shiten_part->SetValue("Helpers", shiten_part_helpers);
					SmartScriptTable shiten_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable shiten_part_animatedjoint_rotation(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation->SetValue("pitchSpeed", 70.0f);
					shiten_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
					SmartScriptTable shiten_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation_pitchlimits->PushBack(-8.0f);
					shiten_part_animatedjoint_rotation_pitchlimits->PushBack(60.0f);
					shiten_part_animatedjoint_rotation->SetValue("pitchLimits", shiten_part_animatedjoint_rotation_pitchlimits);
					shiten_part_animatedjoint->SetValue("Rotation", shiten_part_animatedjoint_rotation);
					shiten_part->SetValue("AnimatedJoint", shiten_part_animatedjoint);
					shitenTurret_part_parts->PushBack(shiten_part);
				}
				{
					SmartScriptTable gunnerLightBeam_part(gEnv->pScriptSystem);
					gunnerLightBeam_part->SetValue("name", "gunnerLightBeam");
					gunnerLightBeam_part->SetValue("class", "Light");
					gunnerLightBeam_part->SetValue("component", "searchLight");
					gunnerLightBeam_part->SetValue("helper", "gunnerLightPos");
					SmartScriptTable gunnerLightBeam_part_light(gEnv->pScriptSystem);
					gunnerLightBeam_part_light->SetValue("type", "HeadLightDynamic");
					gunnerLightBeam_part->SetValue("Light", gunnerLightBeam_part_light);
					shitenTurret_part_parts->PushBack(gunnerLightBeam_part);
				}
				{
					SmartScriptTable gunnerLightFlare_part(gEnv->pScriptSystem);
					gunnerLightFlare_part->SetValue("name", "gunnerLightFlare");
					gunnerLightFlare_part->SetValue("class", "Light");
					gunnerLightFlare_part->SetValue("component", "searchLight");
					gunnerLightFlare_part->SetValue("helper", "gunnerLightPos");
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
				shitenTurret_part_animatedjoint_rotation->SetValue("yawSpeed", 135.0f);
				shitenTurret_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
				SmartScriptTable shitenTurret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(-140.0f);
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(140.0f);
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
				turret_pos_helper->SetValue("position", Vec3(-0.1001, 4, 1.2018));
				turret_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(turret_pos_helper);
			}
			{
				SmartScriptTable propeller_left_pos_helper(gEnv->pScriptSystem);
				propeller_left_pos_helper->SetValue("name", "propeller_left_pos");
				propeller_left_pos_helper->SetValue("position", Vec3(-2.5009999, -11.6821, -0.77560002));
				propeller_left_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(propeller_left_pos_helper);
			}
			{
				SmartScriptTable propeller_right_pos_helper(gEnv->pScriptSystem);
				propeller_right_pos_helper->SetValue("name", "propeller_right_pos");
				propeller_right_pos_helper->SetValue("position", Vec3(2.3921001, -11.6492, -0.73760003));
				propeller_right_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(propeller_right_pos_helper);
			}
			{
				SmartScriptTable driver_eye_pos_helper(gEnv->pScriptSystem);
				driver_eye_pos_helper->SetValue("name", "driver_eye_pos");
				driver_eye_pos_helper->SetValue("position", Vec3(-0.1091, -0.039799999, 4.7182002));
				driver_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(driver_eye_pos_helper);
			}
			{
				SmartScriptTable enter_driver_left_helper(gEnv->pScriptSystem);
				enter_driver_left_helper->SetValue("name", "enter_driver_left");
				enter_driver_left_helper->SetValue("position", Vec3(-4.5946999, -1.783, 0.42140001));
				enter_driver_left_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(enter_driver_left_helper);
			}
			{
				SmartScriptTable enter_gunner_left_helper(gEnv->pScriptSystem);
				enter_gunner_left_helper->SetValue("name", "enter_gunner_left");
				enter_gunner_left_helper->SetValue("position", Vec3(-4.6162, -2.7460999, 0.39539999));
				enter_gunner_left_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(enter_gunner_left_helper);
			}
			{
				SmartScriptTable enter_passenger_left_01_helper(gEnv->pScriptSystem);
				enter_passenger_left_01_helper->SetValue("name", "enter_passenger_left_01");
				enter_passenger_left_01_helper->SetValue("position", Vec3(-4.6283998, -0.89480001, 0.39320001));
				enter_passenger_left_01_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(enter_passenger_left_01_helper);
			}
			{
				SmartScriptTable enter_passenger_left_02_helper(gEnv->pScriptSystem);
				enter_passenger_left_02_helper->SetValue("name", "enter_passenger_left_02");
				enter_passenger_left_02_helper->SetValue("position", Vec3(-4.6355, 0.28099999, 0.42199999));
				enter_passenger_left_02_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(enter_passenger_left_02_helper);
			}
			{
				SmartScriptTable enter_passenger_right_01_helper(gEnv->pScriptSystem);
				enter_passenger_right_01_helper->SetValue("name", "enter_passenger_right_01");
				enter_passenger_right_01_helper->SetValue("position", Vec3(-4.6210999, -3.6787, 0.42640001));
				enter_passenger_right_01_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(enter_passenger_right_01_helper);
			}
			{
				SmartScriptTable enter_passenger_right_02_helper(gEnv->pScriptSystem);
				enter_passenger_right_02_helper->SetValue("name", "enter_passenger_right_02");
				enter_passenger_right_02_helper->SetValue("position", Vec3(-4.6410999, -4.7421999, 0.4386));
				enter_passenger_right_02_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(enter_passenger_right_02_helper);
			}
			{
				SmartScriptTable cornerForcePos_helper(gEnv->pScriptSystem);
				cornerForcePos_helper->SetValue("name", "cornerForcePos");
				cornerForcePos_helper->SetValue("position", Vec3(0, -1.5322, 2.5609));
				cornerForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(cornerForcePos_helper);
			}
			{
				SmartScriptTable pushForcePos_helper(gEnv->pScriptSystem);
				pushForcePos_helper->SetValue("name", "pushForcePos");
				pushForcePos_helper->SetValue("position", Vec3(0, -12.5627, -1.1705));
				pushForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(pushForcePos_helper);
			}
			{
				SmartScriptTable engineSoundPos_helper(gEnv->pScriptSystem);
				engineSoundPos_helper->SetValue("name", "engineSoundPos");
				engineSoundPos_helper->SetValue("position", Vec3(0, -7.3239999, 2.5762));
				engineSoundPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(engineSoundPos_helper);
			}
			{
				SmartScriptTable wakePos_helper(gEnv->pScriptSystem);
				wakePos_helper->SetValue("name", "wakePos");
				wakePos_helper->SetValue("position", Vec3(0, -3.7339001, -1.293));
				wakePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(wakePos_helper);
			}
			{
				SmartScriptTable searchLightPos_helper(gEnv->pScriptSystem);
				searchLightPos_helper->SetValue("name", "searchLightPos");
				searchLightPos_helper->SetValue("position", Vec3(1.2393, 1.5281, 4.3014002));
				searchLightPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(searchLightPos_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, -1.1987, 1.701));
				burningPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(burningPos_helper);
			}
			chassis_part->SetValue("Helpers", chassis_part_helpers);
			SmartScriptTable chassis_part_animated(gEnv->pScriptSystem);
			chassis_part_animated->SetValue("filename", "Objects/Vehicles/Asian_patrolboat/asian_patrolboat.cga");
			chassis_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/Asian_patrolboat/asian_patrolboat_damaged.cga");
			chassis_part->SetValue("Animated", chassis_part_animated);
			parts->PushBack(chassis_part);
		}
		{
			SmartScriptTable mass_part(gEnv->pScriptSystem);
			mass_part->SetValue("name", "mass");
			mass_part->SetValue("class", "MassBox");
			mass_part->SetValue("component", "");
			mass_part->SetValue("mass", 10000.0f);
			mass_part->SetValue("position", Vec3(-0.00050000002, -1.3379, 0.72829998));
			mass_part->SetValue("disablePhysics", false);
			mass_part->SetValue("disableCollision", false);
			mass_part->SetValue("isHidden", false);
			SmartScriptTable mass_part_helpers(gEnv->pScriptSystem);
			mass_part->SetValue("Helpers", mass_part_helpers);
			SmartScriptTable mass_part_massbox(gEnv->pScriptSystem);
			mass_part_massbox->SetValue("size", Vec3(3.0190001, 7.7972999, 1.353));
			mass_part->SetValue("MassBox", mass_part_massbox);
			parts->PushBack(mass_part);
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
			searchLight_part_light->SetValue("diffuseMult", 20.0f);
			searchLight_part_light->SetValue("directFactor", 1.0f);
			searchLight_part->SetValue("Light", searchLight_part_light);
			parts->PushBack(searchLight_part);
		}
		{
			SmartScriptTable searchLightBeam_part(gEnv->pScriptSystem);
			searchLightBeam_part->SetValue("name", "searchLightBeam");
			searchLightBeam_part->SetValue("class", "Light");
			searchLightBeam_part->SetValue("component", "searchLight");
			searchLightBeam_part->SetValue("helper", "searchLightPos");
			SmartScriptTable searchLightBeam_part_light(gEnv->pScriptSystem);
			searchLightBeam_part_light->SetValue("type", "HeadLightBeam");
			searchLightBeam_part->SetValue("Light", searchLightBeam_part_light);
			parts->PushBack(searchLightBeam_part);
		}
		{
			SmartScriptTable searchLightFlare_part(gEnv->pScriptSystem);
			searchLightFlare_part->SetValue("name", "searchLightFlare");
			searchLightFlare_part->SetValue("class", "Light");
			searchLightFlare_part->SetValue("component", "searchLight");
			searchLightFlare_part->SetValue("helper", "searchLightPos");
			SmartScriptTable searchLightFlare_part_light(gEnv->pScriptSystem);
			searchLightFlare_part_light->SetValue("type", "HeadLightFlare");
			searchLightFlare_part->SetValue("Light", searchLightFlare_part_light);
			parts->PushBack(searchLightFlare_part);
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
		component->SetValue("damageMax", 1250.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", false);
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
			damagebehavior->SetValue("class", "Sink");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
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
		component->SetValue("name", "EnterBottom");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-3.7393, -12.8865, -1.5826));
		component->SetValue("maxBound", Vec3(3.7720001, 9.9162998, 2.0163));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterBottom = new VehicleComponent();
		enterBottom->Init(this, component);
		m_components.push_back(enterBottom);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterDriver");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.0233999, -0.18870001, 3.2637));
		component->SetValue("maxBound", Vec3(0.97659999, 1.1423, 4.3629999));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterDriver = new VehicleComponent();
		enterDriver->Init(this, component);
		m_components.push_back(enterDriver);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterGunner");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.9648, 5.8119998, 2.1851001));
		component->SetValue("maxBound", Vec3(1.0225, 8.4761, 3.3677001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterGunner = new VehicleComponent();
		enterGunner->Init(this, component);
		m_components.push_back(enterGunner);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterPassengers");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.8832999, -4.0324998, 2.0281999));
		component->SetValue("maxBound", Vec3(1.7559, -2.7024, 3.1294999));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterPassengers = new VehicleComponent();
		enterPassengers->Init(this, component);
		m_components.push_back(enterPassengers);
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
			damagebehavior_burn->SetValue("selfDamage", 1000.0f);
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
			VehicleDestroyed_damageeffect->SetValue("helper", "engineSoundPos");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.Large_Vehicle.a");
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable VehicleFlipped_damageeffect(gEnv->pScriptSystem);
			VehicleFlipped_damageeffect->SetValue("name", "VehicleFlipped");
			VehicleFlipped_damageeffect->SetValue("helper", "engineSoundPos");
			VehicleFlipped_damageeffect->SetValue("effect", "smoke_and_fire.Jeep.flipped_jeep");
			particles_damageeffects->PushBack(VehicleFlipped_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "burningPos");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.NKPatBoat_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.NKPatBoat_hull_smoke_b");
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
			environmentlayer->SetValue("maxSpeed", 15.0f);
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
		animation->SetValue("name", "steeringWheel");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable _state(gEnv->pScriptSystem);
			_state->SetValue("name", "");
			_state->SetValue("animation", "patrolboat_steeringWheel");
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
			_state->SetValue("animation", "patrolboat_rotor");
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
		movement->SetValue("velMaxReverse", 6.0f);
		movement->SetValue("velLift", 19.5f);
		movement->SetValue("acceleration", 5.0f);
		movement->SetValue("accelerationVelMax", 0.75f);
		movement->SetValue("accelerationMultiplier", 0.75f);
		movement->SetValue("pushHelper", "pushForcePos");
		movement->SetValue("pushTilt", 0.0f);
		movement->SetValue("turnRateMax", 1.3f);
		movement->SetValue("turnAccel", 1.1f);
		movement->SetValue("turnVelocityMult", 2.0f);
		movement->SetValue("turnAccelMultiplier", 0.5f);
		movement->SetValue("turnDamping", 0.0f);
		movement->SetValue("cornerForce", 2.0f);
		movement->SetValue("cornerHelper", "cornerForcePos");
		movement->SetValue("cornerTilt", 0.0f);
		movement->SetValue("waveIdleStrength", Vec3(0.2, 0.2, 0));
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSoundPos");
		movement_soundparams->SetValue("runSoundDelay", 1.25f);
		movement_soundparams->SetValue("rpmPitchSpeed", 0.40000001f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_tweakgroup(gEnv->pScriptSystem);
			player_tweakgroup->SetValue("name", "player");
			SmartScriptTable player_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable velMax_tweak(gEnv->pScriptSystem);
				velMax_tweak->SetValue("name", "velMax");
				velMax_tweak->SetValue("value", 19.0f);
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
				acceleration_tweak->SetValue("value", 8.0f);
				acceleration_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(acceleration_tweak);
			}
			{
				SmartScriptTable turnVelocityMult_tweak(gEnv->pScriptSystem);
				turnVelocityMult_tweak->SetValue("name", "turnVelocityMult");
				turnVelocityMult_tweak->SetValue("value", 1.0f);
				turnVelocityMult_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(turnVelocityMult_tweak);
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
		seat->SetValue("enterHelper", "enter_driver_left");
		seat->SetValue("exitOffsetPlayer", Vec3(4.594, 1.5, 3));
		seat->SetValue("sitHelper", "helper_passenger01_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
		if (m_modName == "MP")
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
		seat->SetValue("blurMult", 0.0f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -110));
			view->SetValue("rotationMax", Vec3(55, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.25));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 17.0f);
			view_thirdperson->SetValue("heightOffset", 0.5f);
			view_thirdperson->SetValue("speed", 5.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
			seataction->SetValue("class", "Lights");
			SmartScriptTable seataction_lights(gEnv->pScriptSystem);
			seataction_lights->SetValue("activation", "toggle");
			seataction_lights->SetValue("sound", 3);
			SmartScriptTable seataction_lights_lightparts(gEnv->pScriptSystem);
			seataction_lights_lightparts->PushBack("searchLight");
			seataction_lights_lightparts->PushBack("searchLightBeam");
			seataction_lights_lightparts->PushBack("searchLightFlare");
			seataction_lights->SetValue("LightParts", seataction_lights_lightparts);
			seataction->SetValue("Lights", seataction_lights);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Sound");
			SmartScriptTable seataction_sound(gEnv->pScriptSystem);
			seataction_sound->SetValue("sound", "Sounds/vehicles:asian_patrolboat:horn");
			seataction_sound->SetValue("helper", "searchLightPos");
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
		seat->SetValue("part", "shitenTurret");
		seat->SetValue("enterHelper", "enter_gunner_left");
		seat->SetValue("exitOffsetPlayer", Vec3(4.61, 12, 2.8));
		seat->SetValue("sitHelper", "helper_passenger02_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "shiten");
		seat->SetValue("seatGroupIndex", 2);
		seat->SetValue("blurMult", 0.0f);
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
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.5, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
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
				if (m_modName == "Gauss")
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
					weapon->SetValue("class", "VehicleShiTenV2");
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
		seat->SetValue("enterHelper", "enter_passenger_left_01");
		seat->SetValue("exitOffsetPlayer", Vec3(3.4, -2, 2.2));
		seat->SetValue("sitHelper", "helper_passenger03_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
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
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 5.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		seats->PushBack(seat);

		VehicleSeat* passenger03 = new VehicleSeat();
		passenger03->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger03", passenger03 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger04");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "enter_passenger_left_02");
		seat->SetValue("exitOffsetPlayer", Vec3(4.2, -3, 2.2));
		seat->SetValue("sitHelper", "helper_passenger04_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
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
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 5.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger04 = new VehicleSeat();
		passenger04->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger04", passenger04 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger05");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "enter_passenger_right_01");
		seat->SetValue("exitOffsetPlayer", Vec3(5, 1, 2.2));
		seat->SetValue("sitHelper", "helper_passenger05_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
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
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 5.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
		seat->SetValue("enterHelper", "enter_passenger_right_02");
		seat->SetValue("exitOffsetPlayer", Vec3(5.8, 2, 2.2));
		seat->SetValue("sitHelper", "helper_passenger06_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
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
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 5.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
		this->SetPaintMaterial("objects/vehicles/asian_patrolboat/asian_patrolboat.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/asian_patrolboat/asian_patrolboat_us.mtl");
	}

	// Actions
	{
		SmartScriptTable actions(gEnv->pScriptSystem);
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "Enter");
			SmartScriptTable action_activations(gEnv->pScriptSystem);
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				activation->SetValue("type", "OnUsed");
				activation->SetValue("param1", "component");
				activation->SetValue("param2", "EnterBottom");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("driver");
			action_enter_seats->PushBack("gunner");
			action_enter_seats->PushBack("passenger03");
			action_enter_seats->PushBack("passenger04");
			action_enter_seats->PushBack("passenger05");
			action_enter_seats->PushBack("passenger06");
			action_enter->SetValue("Seats", action_enter_seats);
			action->SetValue("Enter", action_enter);
			actions->PushBack(action);
		}
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "Enter");
			SmartScriptTable action_activations(gEnv->pScriptSystem);
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				activation->SetValue("type", "OnUsed");
				activation->SetValue("param1", "component");
				activation->SetValue("param2", "EnterDriver");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("driver");
			action_enter->SetValue("Seats", action_enter_seats);
			action->SetValue("Enter", action_enter);
			actions->PushBack(action);
		}
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "Enter");
			SmartScriptTable action_activations(gEnv->pScriptSystem);
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				activation->SetValue("type", "OnUsed");
				activation->SetValue("param1", "component");
				activation->SetValue("param2", "EnterGunner");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("gunner");
			action_enter->SetValue("Seats", action_enter_seats);
			action->SetValue("Enter", action_enter);
			actions->PushBack(action);
		}
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "Enter");
			SmartScriptTable action_activations(gEnv->pScriptSystem);
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				activation->SetValue("type", "OnUsed");
				activation->SetValue("param1", "component");
				activation->SetValue("param2", "EnterPassengers");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("passenger03");
			action_enter_seats->PushBack("passenger04");
			action_enter_seats->PushBack("passenger05");
			action_enter_seats->PushBack("passenger06");
			action_enter->SetValue("Seats", action_enter_seats);
			action->SetValue("Enter", action_enter);
			actions->PushBack(action);
		}

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Actions", actions);
		this->InitActions(table);
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("collDamageThreshold", 200.0f);
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
			damagemultiplier->SetValue("multiplier", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
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
			damagemultiplier->SetValue("multiplier", 1.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 4.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "c4");
			damagemultiplier->SetValue("multiplier", 4.0f);
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
			Destroy_damagesgroup->SetValue("useTemplate", "VehicleDestroy");
			SmartScriptTable Destroy_damagesgroup_damagessubgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.34999999f);
				damagessubgroup->SetValue("randomness", 0.1f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Explosion");
					SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
					damagebehavior_explosion->SetValue("helper", "impulsePos");
					damagebehavior_explosion->SetValue("damage", 500.0f);
					damagebehavior_explosion->SetValue("radius", 9.0f);
					damagebehavior_explosion->SetValue("pressure", 25.0f);
					damagebehavior->SetValue("Explosion", damagebehavior_explosion);
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
