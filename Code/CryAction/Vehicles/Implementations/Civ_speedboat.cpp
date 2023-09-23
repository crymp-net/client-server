#include "Civ_speedboat.h"

Civ_speedboat::Civ_speedboat()
{
	// enable Civ_speedboat::Init function
	this->Vehicle::PatchVTable();
}

Civ_speedboat::~Civ_speedboat()
{
}

bool Civ_speedboat::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "seavehicle";

	// Physics
	m_physics.damping = 0.1f;
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 75.0f;
	m_buoyancy.waterResistance = 300.0f;
	m_buoyancy.waterDamping = 0.0f;
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
			chassis_part->SetValue("Parts", chassis_part_parts);
			SmartScriptTable chassis_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable propeller_left_pos_helper(gEnv->pScriptSystem);
				propeller_left_pos_helper->SetValue("name", "propeller_left_pos");
				propeller_left_pos_helper->SetValue("position", Vec3(-0.46290001, -4.4889998, 1.013));
				propeller_left_pos_helper->SetValue("direction", Vec3(0, -0.76639998, 0.64230001));
				chassis_part_helpers->PushBack(propeller_left_pos_helper);
			}
			{
				SmartScriptTable propeller_right_pos_helper(gEnv->pScriptSystem);
				propeller_right_pos_helper->SetValue("name", "propeller_right_pos");
				propeller_right_pos_helper->SetValue("position", Vec3(0.48410001, -4.4938998, 1.043));
				propeller_right_pos_helper->SetValue("direction", Vec3(0, -0.76639998, 0.64230001));
				chassis_part_helpers->PushBack(propeller_right_pos_helper);
			}
			{
				SmartScriptTable driver_eye_pos_helper(gEnv->pScriptSystem);
				driver_eye_pos_helper->SetValue("name", "driver_eye_pos");
				driver_eye_pos_helper->SetValue("position", Vec3(0, 1.2009, 2.5739999));
				driver_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(driver_eye_pos_helper);
			}
			{
				SmartScriptTable enter_driver_helper(gEnv->pScriptSystem);
				enter_driver_helper->SetValue("name", "enter_driver");
				enter_driver_helper->SetValue("position", Vec3(-2.615, 1.1399, 0.93300003));
				enter_driver_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(enter_driver_helper);
			}
			{
				SmartScriptTable enter_passenger_01_helper(gEnv->pScriptSystem);
				enter_passenger_01_helper->SetValue("name", "enter_passenger_01");
				enter_passenger_01_helper->SetValue("position", Vec3(-2.615, 0.050000001, 0.93300003));
				enter_passenger_01_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(enter_passenger_01_helper);
			}
			{
				SmartScriptTable enter_passenger_02_helper(gEnv->pScriptSystem);
				enter_passenger_02_helper->SetValue("name", "enter_passenger_02");
				enter_passenger_02_helper->SetValue("position", Vec3(-2.6431, -0.93900001, 0.93300003));
				enter_passenger_02_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(enter_passenger_02_helper);
			}
			{
				SmartScriptTable enter_passenger_03_helper(gEnv->pScriptSystem);
				enter_passenger_03_helper->SetValue("name", "enter_passenger_03");
				enter_passenger_03_helper->SetValue("position", Vec3(-2.6317999, -1.9331, 0.93300003));
				enter_passenger_03_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(enter_passenger_03_helper);
			}
			{
				SmartScriptTable driver_sit_pos_helper(gEnv->pScriptSystem);
				driver_sit_pos_helper->SetValue("name", "driver_sit_pos");
				driver_sit_pos_helper->SetValue("position", Vec3(0, 0.926, 1.6));
				driver_sit_pos_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(driver_sit_pos_helper);
			}
			{
				SmartScriptTable passenger_sit_01_helper(gEnv->pScriptSystem);
				passenger_sit_01_helper->SetValue("name", "passenger_sit_01");
				passenger_sit_01_helper->SetValue("position", Vec3(-0.72070003, -0.20999999, 1.6));
				passenger_sit_01_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(passenger_sit_01_helper);
			}
			{
				SmartScriptTable passenger_sit_02_helper(gEnv->pScriptSystem);
				passenger_sit_02_helper->SetValue("name", "passenger_sit_02");
				passenger_sit_02_helper->SetValue("position", Vec3(-0.017100001, -0.22, 1.6));
				passenger_sit_02_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(passenger_sit_02_helper);
			}
			{
				SmartScriptTable passenger_sit_03_helper(gEnv->pScriptSystem);
				passenger_sit_03_helper->SetValue("name", "passenger_sit_03");
				passenger_sit_03_helper->SetValue("position", Vec3(0.71579999, -0.2339, 1.6));
				passenger_sit_03_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(passenger_sit_03_helper);
			}
			{
				SmartScriptTable leftEngineSmokePos_helper(gEnv->pScriptSystem);
				leftEngineSmokePos_helper->SetValue("name", "leftEngineSmokePos");
				leftEngineSmokePos_helper->SetValue("position", Vec3(-0.4709, -4.6090999, 1.726));
				leftEngineSmokePos_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(leftEngineSmokePos_helper);
			}
			{
				SmartScriptTable rightEngineSmokePos_helper(gEnv->pScriptSystem);
				rightEngineSmokePos_helper->SetValue("name", "rightEngineSmokePos");
				rightEngineSmokePos_helper->SetValue("position", Vec3(0.5, -4.6378999, 1.734));
				rightEngineSmokePos_helper->SetValue("direction", Vec3(0, 1, 0.001));
				chassis_part_helpers->PushBack(rightEngineSmokePos_helper);
			}
			{
				SmartScriptTable wakePos_helper(gEnv->pScriptSystem);
				wakePos_helper->SetValue("name", "wakePos");
				wakePos_helper->SetValue("position", Vec3(-0.028100001, 0.66850001, -0.1987));
				wakePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(wakePos_helper);
			}
			{
				SmartScriptTable sprayLeftPos_helper(gEnv->pScriptSystem);
				sprayLeftPos_helper->SetValue("name", "sprayLeftPos");
				sprayLeftPos_helper->SetValue("position", Vec3(-0.47220001, 2.0427001, 0.064300001));
				sprayLeftPos_helper->SetValue("direction", Vec3(-1, 0, 0));
				chassis_part_helpers->PushBack(sprayLeftPos_helper);
			}
			{
				SmartScriptTable sprayRightPos_helper(gEnv->pScriptSystem);
				sprayRightPos_helper->SetValue("name", "sprayRightPos");
				sprayRightPos_helper->SetValue("position", Vec3(0.47679999, 2.0014999, 0.070299998));
				sprayRightPos_helper->SetValue("direction", Vec3(1, 0, 0));
				chassis_part_helpers->PushBack(sprayRightPos_helper);
			}
			{
				SmartScriptTable splashPos_helper(gEnv->pScriptSystem);
				splashPos_helper->SetValue("name", "splashPos");
				splashPos_helper->SetValue("position", Vec3(0, 3.1592, -0.2823));
				splashPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(splashPos_helper);
			}
			{
				SmartScriptTable cornerForcePos_helper(gEnv->pScriptSystem);
				cornerForcePos_helper->SetValue("name", "cornerForcePos");
				cornerForcePos_helper->SetValue("position", Vec3(0, -0.1875, 0.85000002));
				cornerForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(cornerForcePos_helper);
			}
			{
				SmartScriptTable pushForcePos_helper(gEnv->pScriptSystem);
				pushForcePos_helper->SetValue("name", "pushForcePos");
				pushForcePos_helper->SetValue("position", Vec3(0, -2.7539001, 0.1217));
				pushForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(pushForcePos_helper);
			}
			{
				SmartScriptTable engineSoundPos_helper(gEnv->pScriptSystem);
				engineSoundPos_helper->SetValue("name", "engineSoundPos");
				engineSoundPos_helper->SetValue("position", Vec3(0.0066, -4.2224002, 1.6874));
				engineSoundPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(engineSoundPos_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(-0.0513, 0.0122, 0.85000002));
				centerPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(centerPos_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, 0.55860001, 1.0754));
				burningPos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(burningPos_helper);
			}
			chassis_part->SetValue("Helpers", chassis_part_helpers);
			SmartScriptTable chassis_part_animated(gEnv->pScriptSystem);
			if (m_modName == "Roofed")
			{
				chassis_part_animated->SetValue("filename", "Objects/Vehicles/speedboat/speedboat.cga");
			}
			else
			{
				chassis_part_animated->SetValue("filename", "Objects/Vehicles/speedboat/speedboat_asian.cga");
			}
			if (m_modName == "Roofed")
			{
				chassis_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/speedboat/speedboat_damaged.cga");
			}
			else
			{
				chassis_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/speedboat/speedboat_asian_damaged.cga");
			}
			chassis_part->SetValue("Animated", chassis_part_animated);
			parts->PushBack(chassis_part);
		}
		{
			SmartScriptTable mass_part(gEnv->pScriptSystem);
			mass_part->SetValue("name", "mass");
			mass_part->SetValue("class", "MassBox");
			mass_part->SetValue("component", "");
			mass_part->SetValue("mass", 1000.0f);
			mass_part->SetValue("position", Vec3(0, -0.5, 0.84100002));
			mass_part->SetValue("disablePhysics", false);
			mass_part->SetValue("disableCollision", false);
			mass_part->SetValue("isHidden", false);
			SmartScriptTable mass_part_helpers(gEnv->pScriptSystem);
			mass_part->SetValue("Helpers", mass_part_helpers);
			SmartScriptTable mass_part_massbox(gEnv->pScriptSystem);
			mass_part_massbox->SetValue("size", Vec3(1.8609999, 3.5639999, 0.68000001));
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
		component->SetValue("name", "hull");
		component->SetValue("damageMax", 600.0f);
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
		component->SetValue("minBound", Vec3(-0.84890002, -4.79, 0.31600001));
		component->SetValue("maxBound", Vec3(-0.114, -3.7, 2.2));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.2f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
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
			damagebehavior->SetValue("damageRatioMin", 1.0f);
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
		component->SetValue("minBound", Vec3(0.1082, -4.7930002, 0.28999999));
		component->SetValue("maxBound", Vec3(0.80299997, -3.7, 2.2));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.2f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
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
			damagebehavior->SetValue("damageRatioMin", 1.0f);
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
			damagebehavior_burn->SetValue("interval", 0.5f);
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
		particles_exhaust_enginestart->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke_dark");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "vehicle_fx.vehicles_surface_fx.small_boat");
		particles_exhaust_enginerunning->SetValue("boostEffect", "");
		particles_exhaust_enginerunning->SetValue("baseSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 0.2f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 1.0f);
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
			leftEngineSmoke_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.CivSpeedBoat_engine_smoke_b");
			particles_damageeffects->PushBack(leftEngineSmoke_damageeffect);
		}
		{
			SmartScriptTable rightEngineSmoke_damageeffect(gEnv->pScriptSystem);
			rightEngineSmoke_damageeffect->SetValue("name", "rightEngineSmoke");
			rightEngineSmoke_damageeffect->SetValue("helper", "rightEngineSmokePos");
			rightEngineSmoke_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.CivSpeedBoat_engine_smoke_b");
			particles_damageeffects->PushBack(rightEngineSmoke_damageeffect);
		}
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "");
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
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.CivSpeedBoat_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sea.CivSpeedBoat_hull_smoke_b");
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
			SmartScriptTable environmentlayer_wheels(gEnv->pScriptSystem);
			environmentlayer->SetValue("Wheels", environmentlayer_wheels);
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
			_state->SetValue("animation", "asian_steeringWheel");
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
			_state->SetValue("animation", "asian_rotor");
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
		movement->SetValue("acceleration", 5.0f);
		movement->SetValue("accelerationVelMax", 0.5f);
		movement->SetValue("accelerationMultiplier", 1.0f);
		movement->SetValue("pushHelper", "pushForcePos");
		movement->SetValue("pushTilt", 0.0f);
		movement->SetValue("turnRateMax", 0.89999998f);
		movement->SetValue("turnAccel", 0.89999998f);
		movement->SetValue("turnAccelMultiplier", 1.25f);
		movement->SetValue("turnDamping", 0.0f);
		movement->SetValue("cornerForce", 2.5f);
		movement->SetValue("cornerHelper", "cornerForcePos");
		movement->SetValue("cornerTilt", 0.0f);
		movement->SetValue("waveIdleStrength", Vec3(0.80000001, 0.2, 0));
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSoundPos");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("rpmPitchSpeed", 1.2f);
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
				pushTilt_tweak->SetValue("value", 13.0f);
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
		seat->SetValue("enterHelper", "enter_driver");
		seat->SetValue("exitOffsetPlayer", Vec3(2.6, 2.5, 0.8));
		seat->SetValue("sitHelper", "driver_sit_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
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
			seataction_sound->SetValue("sound", "Sounds/vehicles:civ_speedboat:horn");
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
		seat->SetValue("name", "passenger_01");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "enter_passenger_01");
		seat->SetValue("sitHelper", "passenger_sit_01");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
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

		VehicleSeat* passenger_01 = new VehicleSeat();
		passenger_01->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger_01", passenger_01 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger_02");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "enter_passenger_02");
		seat->SetValue("sitHelper", "passenger_sit_02");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
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

		VehicleSeat* passenger_02 = new VehicleSeat();
		passenger_02->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger_02", passenger_02 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger_03");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "enter_passenger_03");
		seat->SetValue("sitHelper", "passenger_sit_03");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
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

		VehicleSeat* passenger_03 = new VehicleSeat();
		passenger_03->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger_03", passenger_03 });
	}

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetValue("Seats", seats);
	}

	// SeatTransitions
	this->InitSeatTransitions();

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("collDamageThreshold", 50.0f);
		damages->SetValue("vehicleCollisionDestructionSpeed", 16.0f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.15000001f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.15000001f);
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
			damagemultiplier->SetValue("multiplier", 2.0f);
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
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "c4");
			damagemultiplier->SetValue("multiplier", 3.0f);
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
