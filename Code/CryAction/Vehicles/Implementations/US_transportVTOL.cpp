#include "US_transportVTOL.h"

US_transportVTOL::US_transportVTOL()
{
	// enable US_transportVTOL::Init function
	this->Vehicle::PatchVTable();
}

US_transportVTOL::~US_transportVTOL()
{
}

bool US_transportVTOL::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "vtol";

	// Physics
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	// Unknown attribute collDamageThreshold="5000"
	m_buoyancy.waterDensity = 150.0f;
	m_buoyancy.waterResistance = 2500.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.002f;
	m_physics.maxLoggedCollisions = 2;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable crane_part(gEnv->pScriptSystem);
			crane_part->SetValue("name", "crane");
			crane_part->SetValue("class", "Static");
			crane_part->SetValue("component", "Hull");
			crane_part->SetValue("mass", 0.0f);
			crane_part->SetValue("disablePhysics", false);
			crane_part->SetValue("disableCollision", true);
			SmartScriptTable crane_part_static(gEnv->pScriptSystem);
			crane_part_static->SetValue("filename", "objects/vehicles/us_vtol_transport/crane_unit_tank.cgf");
			crane_part_static->SetValue("geometry", "");
			crane_part->SetValue("Static", crane_part_static);
			SmartScriptTable crane_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable tank_pos_helper(gEnv->pScriptSystem);
				tank_pos_helper->SetValue("name", "tank_pos");
				tank_pos_helper->SetValue("position", Vec3(0, -2.55377, -0.56265));
				tank_pos_helper->SetValue("direction", Vec3(0, -1, 0));
				crane_part_helpers->PushBack(tank_pos_helper);
			}
			crane_part->SetValue("Helpers", crane_part_helpers);
			parts->PushBack(crane_part);
		}
		{
			SmartScriptTable chassis_part(gEnv->pScriptSystem);
			chassis_part->SetValue("name", "chassis");
			chassis_part->SetValue("class", "Animated");
			chassis_part->SetValue("component", "Hull");
			chassis_part->SetValue("mass", 30000.0f);
			chassis_part->SetValue("disablePhysics", false);
			SmartScriptTable chassis_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable wing_left_part(gEnv->pScriptSystem);
				wing_left_part->SetValue("name", "wing_left");
				wing_left_part->SetValue("class", "AnimatedJoint");
				wing_left_part->SetValue("component", "WingLeft");
				SmartScriptTable wing_left_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable thruster1_helper(gEnv->pScriptSystem);
					thruster1_helper->SetValue("name", "thruster1");
					thruster1_helper->SetValue("position", Vec3(4.252, 2.081, -3.15));
					thruster1_helper->SetValue("direction", Vec3(0, -1, 0.005));
					wing_left_part_helpers->PushBack(thruster1_helper);
				}
				{
					SmartScriptTable thruster2_helper(gEnv->pScriptSystem);
					thruster2_helper->SetValue("name", "thruster2");
					thruster2_helper->SetValue("position", Vec3(4.241, 0.847, -3.149));
					thruster2_helper->SetValue("direction", Vec3(0, -1, 0.005));
					wing_left_part_helpers->PushBack(thruster2_helper);
				}
				wing_left_part->SetValue("Helpers", wing_left_part_helpers);
				chassis_part_parts->PushBack(wing_left_part);
			}
			{
				SmartScriptTable wing_right_01_part(gEnv->pScriptSystem);
				wing_right_01_part->SetValue("name", "wing_right_01");
				wing_right_01_part->SetValue("class", "AnimatedJoint");
				wing_right_01_part->SetValue("component", "WingRight");
				SmartScriptTable wing_right_01_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable thruster3_helper(gEnv->pScriptSystem);
					thruster3_helper->SetValue("name", "thruster3");
					thruster3_helper->SetValue("position", Vec3(-4.28, 0.855, -3.117));
					thruster3_helper->SetValue("direction", Vec3(0, -1, 0.005));
					wing_right_01_part_helpers->PushBack(thruster3_helper);
				}
				{
					SmartScriptTable thruster4_helper(gEnv->pScriptSystem);
					thruster4_helper->SetValue("name", "thruster4");
					thruster4_helper->SetValue("position", Vec3(-4.28, 2.076, -3.156));
					thruster4_helper->SetValue("direction", Vec3(0, -1, 0.005));
					wing_right_01_part_helpers->PushBack(thruster4_helper);
				}
				wing_right_01_part->SetValue("Helpers", wing_right_01_part_helpers);
				chassis_part_parts->PushBack(wing_right_01_part);
			}
			{
				SmartScriptTable wing_left_end_part(gEnv->pScriptSystem);
				wing_left_end_part->SetValue("name", "wing_left_end");
				wing_left_end_part->SetValue("class", "AnimatedJoint");
				wing_left_end_part->SetValue("component", "WingLeft");
				SmartScriptTable wing_left_end_part_animatedjoint(gEnv->pScriptSystem);
				wing_left_end_part_animatedjoint->SetValue("detachBaseForce", Vec3(-1, 0, 0));
				wing_left_end_part->SetValue("AnimatedJoint", wing_left_end_part_animatedjoint);
				chassis_part_parts->PushBack(wing_left_end_part);
			}
			{
				SmartScriptTable wing_right_01_part(gEnv->pScriptSystem);
				wing_right_01_part->SetValue("name", "wing_right_01");
				wing_right_01_part->SetValue("class", "AnimatedJoint");
				wing_right_01_part->SetValue("component", "WingRight");
				SmartScriptTable wing_right_01_part_animatedjoint(gEnv->pScriptSystem);
				wing_right_01_part_animatedjoint->SetValue("detachBaseForce", Vec3(1, 0, 0));
				wing_right_01_part->SetValue("AnimatedJoint", wing_right_01_part_animatedjoint);
				chassis_part_parts->PushBack(wing_right_01_part);
			}
			{
				SmartScriptTable turret_part(gEnv->pScriptSystem);
				turret_part->SetValue("name", "turret");
				turret_part->SetValue("class", "AnimatedJoint");
				SmartScriptTable turret_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable vtol_gun_pitch_part(gEnv->pScriptSystem);
					vtol_gun_pitch_part->SetValue("name", "vtol_gun_pitch");
					vtol_gun_pitch_part->SetValue("class", "AnimatedJoint");
					SmartScriptTable vtol_gun_pitch_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable gunOut_helper(gEnv->pScriptSystem);
						gunOut_helper->SetValue("name", "gunOut");
						gunOut_helper->SetValue("position", Vec3(0, 9.757, -2.459));
						gunOut_helper->SetValue("direction", Vec3(0, 1, -0.005));
						vtol_gun_pitch_part_helpers->PushBack(gunOut_helper);
					}
					{
						SmartScriptTable gunCamera_helper(gEnv->pScriptSystem);
						gunCamera_helper->SetValue("name", "gunCamera");
						gunCamera_helper->SetValue("position", Vec3(-0.475, 8.55, -2.375));
						gunCamera_helper->SetValue("direction", Vec3(0, 1, -0.005));
						vtol_gun_pitch_part_helpers->PushBack(gunCamera_helper);
					}
					vtol_gun_pitch_part->SetValue("Helpers", vtol_gun_pitch_part_helpers);
					SmartScriptTable vtol_gun_pitch_part_animatedjoint(gEnv->pScriptSystem);
					vtol_gun_pitch_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 1, 0));
					SmartScriptTable vtol_gun_pitch_part_animatedjoint_rotation(gEnv->pScriptSystem);
					vtol_gun_pitch_part_animatedjoint_rotation->SetValue("pitchSpeed", 75.0f);
					SmartScriptTable vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits->PushBack(-50.0f);
					vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits->PushBack(0.0f);
					vtol_gun_pitch_part_animatedjoint_rotation->SetValue("pitchLimits", vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits);
					vtol_gun_pitch_part_animatedjoint->SetValue("Rotation", vtol_gun_pitch_part_animatedjoint_rotation);
					vtol_gun_pitch_part->SetValue("AnimatedJoint", vtol_gun_pitch_part_animatedjoint);
					turret_part_parts->PushBack(vtol_gun_pitch_part);
				}
				turret_part->SetValue("Parts", turret_part_parts);
				SmartScriptTable turret_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable turret_part_animatedjoint_rotation(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation->SetValue("yawSpeed", 75.0f);
				turret_part_animatedjoint->SetValue("Rotation", turret_part_animatedjoint_rotation);
				turret_part->SetValue("AnimatedJoint", turret_part_animatedjoint);
				chassis_part_parts->PushBack(turret_part);
			}
			chassis_part->SetValue("Parts", chassis_part_parts);
			SmartScriptTable chassis_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable gunner_eye_pos_helper(gEnv->pScriptSystem);
				gunner_eye_pos_helper->SetValue("name", "gunner_eye_pos");
				gunner_eye_pos_helper->SetValue("position", Vec3(0.001, 3.621, 1.365));
				gunner_eye_pos_helper->SetValue("direction", Vec3(0, 1, -0.005));
				chassis_part_helpers->PushBack(gunner_eye_pos_helper);
			}
			{
				SmartScriptTable pilot_eye_pos_helper(gEnv->pScriptSystem);
				pilot_eye_pos_helper->SetValue("name", "pilot_eye_pos");
				pilot_eye_pos_helper->SetValue("position", Vec3(0, 6.32, -0.575));
				pilot_eye_pos_helper->SetValue("direction", Vec3(0, 1, -0.005));
				chassis_part_helpers->PushBack(pilot_eye_pos_helper);
			}
			{
				SmartScriptTable passenger01_pos_helper(gEnv->pScriptSystem);
				passenger01_pos_helper->SetValue("name", "passenger01_pos");
				chassis_part_helpers->PushBack(passenger01_pos_helper);
			}
			{
				SmartScriptTable passenger02_pos_helper(gEnv->pScriptSystem);
				passenger02_pos_helper->SetValue("name", "passenger02_pos");
				chassis_part_helpers->PushBack(passenger02_pos_helper);
			}
			{
				SmartScriptTable passenger03_pos_helper(gEnv->pScriptSystem);
				passenger03_pos_helper->SetValue("name", "passenger03_pos");
				chassis_part_helpers->PushBack(passenger03_pos_helper);
			}
			{
				SmartScriptTable passenger04_pos_helper(gEnv->pScriptSystem);
				passenger04_pos_helper->SetValue("name", "passenger04_pos");
				chassis_part_helpers->PushBack(passenger04_pos_helper);
			}
			{
				SmartScriptTable passenger05_pos_helper(gEnv->pScriptSystem);
				passenger05_pos_helper->SetValue("name", "passenger05_pos");
				chassis_part_helpers->PushBack(passenger05_pos_helper);
			}
			{
				SmartScriptTable passenger06_pos_helper(gEnv->pScriptSystem);
				passenger06_pos_helper->SetValue("name", "passenger06_pos");
				chassis_part_helpers->PushBack(passenger06_pos_helper);
			}
			{
				SmartScriptTable passenger07_pos_helper(gEnv->pScriptSystem);
				passenger07_pos_helper->SetValue("name", "passenger07_pos");
				chassis_part_helpers->PushBack(passenger07_pos_helper);
			}
			{
				SmartScriptTable passenger08_pos_helper(gEnv->pScriptSystem);
				passenger08_pos_helper->SetValue("name", "passenger08_pos");
				chassis_part_helpers->PushBack(passenger08_pos_helper);
			}
			{
				SmartScriptTable enter_driver_left_helper(gEnv->pScriptSystem);
				enter_driver_left_helper->SetValue("name", "enter_driver_left");
				enter_driver_left_helper->SetValue("position", Vec3(-3.542, 5.997, -3.218));
				enter_driver_left_helper->SetValue("direction", Vec3(0, 1, -0.005));
				chassis_part_helpers->PushBack(enter_driver_left_helper);
			}
			{
				SmartScriptTable leftRocketOut_helper(gEnv->pScriptSystem);
				leftRocketOut_helper->SetValue("name", "leftRocketOut");
				leftRocketOut_helper->SetValue("position", Vec3(-1.142, 4.004, -0.126));
				leftRocketOut_helper->SetValue("direction", Vec3(0, 1, -0.005));
				chassis_part_helpers->PushBack(leftRocketOut_helper);
			}
			{
				SmartScriptTable rightRocketOut_helper(gEnv->pScriptSystem);
				rightRocketOut_helper->SetValue("name", "rightRocketOut");
				rightRocketOut_helper->SetValue("position", Vec3(1.142, 4.004, -0.129));
				rightRocketOut_helper->SetValue("direction", Vec3(0, 1, -0.005));
				chassis_part_helpers->PushBack(rightRocketOut_helper);
			}
			{
				SmartScriptTable bodyburn_helper(gEnv->pScriptSystem);
				bodyburn_helper->SetValue("name", "bodyburn");
				bodyburn_helper->SetValue("position", Vec3(0, 3.007, 1.411));
				bodyburn_helper->SetValue("direction", Vec3(0, -0.03, 1));
				chassis_part_helpers->PushBack(bodyburn_helper);
			}
			{
				SmartScriptTable gunner_enter_pos_helper(gEnv->pScriptSystem);
				gunner_enter_pos_helper->SetValue("name", "gunner_enter_pos");
				gunner_enter_pos_helper->SetValue("position", Vec3(-3.515, 5.089, -3.223));
				gunner_enter_pos_helper->SetValue("direction", Vec3(0, 1, -0.005));
				chassis_part_helpers->PushBack(gunner_enter_pos_helper);
			}
			{
				SmartScriptTable passenger_enter_pos_helper(gEnv->pScriptSystem);
				passenger_enter_pos_helper->SetValue("name", "passenger_enter_pos");
				passenger_enter_pos_helper->SetValue("position", Vec3(0, -5.819, -3.473));
				passenger_enter_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(passenger_enter_pos_helper);
			}
			{
				SmartScriptTable impulsePos_helper(gEnv->pScriptSystem);
				impulsePos_helper->SetValue("name", "impulsePos");
				impulsePos_helper->SetValue("position", Vec3(0, 2, 0));
				impulsePos_helper->SetValue("direction", Vec3(0, 1, 0));
				chassis_part_helpers->PushBack(impulsePos_helper);
			}
			chassis_part->SetValue("Helpers", chassis_part_helpers);
			SmartScriptTable chassis_part_animated(gEnv->pScriptSystem);
			chassis_part_animated->SetValue("filename", "Objects/Vehicles/US_VTOL_Transport/US_VTOL_Transport.cga");
			chassis_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/US_VTOL_Transport/US_VTOL_Transport_destroyed.cga");
			chassis_part_animated->SetValue("destroyedSuffix", "_destroyed");
			chassis_part->SetValue("Animated", chassis_part_animated);
			parts->PushBack(chassis_part);
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
		component->SetValue("damageMax", 600.0f);
		component->SetValue("major", false);
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
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* hull = new VehicleComponent();
		hull->Init(this, component);
		m_components.push_back(hull);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "WingLeft");
		component->SetValue("damageMax", 500.0f);
		component->SetValue("major", false);
		component->SetValue("hullAffection", 0.5f);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wingLeft = new VehicleComponent();
		wingLeft->Init(this, component);
		m_components.push_back(wingLeft);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "WingRight");
		component->SetValue("damageMax", 500.0f);
		component->SetValue("major", false);
		component->SetValue("hullAffection", 0.5f);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wingRight = new VehicleComponent();
		wingRight->Init(this, component);
		m_components.push_back(wingRight);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "gunnerCockpit");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.6702, 6.0615, -2.0191));
		component->SetValue("maxBound", Vec3(0.67, 7.48, -0.2974));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* gunnerCockpit = new VehicleComponent();
		gunnerCockpit->Init(this, component);
		m_components.push_back(gunnerCockpit);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "pilotCockpit");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.756, 3.1746, -0.2185));
		component->SetValue("maxBound", Vec3(0.7559, 4.7039, 1.8103));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* pilotCockpit = new VehicleComponent();
		pilotCockpit->Init(this, component);
		m_components.push_back(pilotCockpit);
	}

	this->RegisterComponentParts(componentParts);

	// Particles
	{
		SmartScriptTable particles(gEnv->pScriptSystem);
		SmartScriptTable particles_damageeffects(gEnv->pScriptSystem);
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.vtol_explosion.explosion");
			VehicleDestroyed_damageeffect->SetValue("scaleMax", 1.0f);
			VehicleDestroyed_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			VehicleDestroyed_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 0.0f);
			environmentlayer->SetValue("minSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			environmentlayer->SetValue("maxSpeed", 10.0f);
			environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			environmentlayer->SetValue("minPowerSizeScale", 1.0f);
			environmentlayer->SetValue("minPowerCountScale", 1.0f);
			environmentlayer->SetValue("maxPowerSizeScale", 1.0f);
			environmentlayer->SetValue("maxPowerCountScale", 1.0f);
			SmartScriptTable environmentlayer_alignment(gEnv->pScriptSystem);
			environmentlayer_alignment->SetValue("alignGroundHeight", 15.0f);
			environmentlayer_alignment->SetValue("maxHeightSizeScale", 1.0f);
			environmentlayer_alignment->SetValue("maxHeightCountScale", 1.0f);
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
		animation->SetValue("name", "wings");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable tovertical_state(gEnv->pScriptSystem);
			tovertical_state->SetValue("name", "tovertical");
			tovertical_state->SetValue("animation", "VTOL_Transport_EngFront_to_hovering");
			tovertical_state->SetValue("sound", "sounds/vehicles:us_vtol:wings_vertical");
			tovertical_state->SetValue("isLooped", false);
			tovertical_state->SetValue("speedDefault", 1.5f);
			tovertical_state->SetValue("speedMin", 1.0f);
			tovertical_state->SetValue("speedMax", 2.0f);
			animation_states->PushBack(tovertical_state);
		}
		{
			SmartScriptTable tohorizontal_state(gEnv->pScriptSystem);
			tohorizontal_state->SetValue("name", "tohorizontal");
			tohorizontal_state->SetValue("animation", "VTOL_Transport_EngFront_to_flying");
			tohorizontal_state->SetValue("sound", "sounds/vehicles:us_vtol:wings_horizontal");
			tohorizontal_state->SetValue("isLooped", false);
			tohorizontal_state->SetValue("speedDefault", 1.0f);
			tohorizontal_state->SetValue("speedMin", 1.0f);
			tohorizontal_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(tohorizontal_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* wings = new VehicleAnimation();
		wings->Init(this, animation);
		m_animations.push_back({ "wings", wings });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "wingsRear");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable tovertical_state(gEnv->pScriptSystem);
			tovertical_state->SetValue("name", "tovertical");
			tovertical_state->SetValue("animation", "VTOL_Transport_Engrear_to_hovering.");
			tovertical_state->SetValue("isLooped", false);
			tovertical_state->SetValue("speedDefault", 1.5f);
			tovertical_state->SetValue("speedMin", 1.0f);
			tovertical_state->SetValue("speedMax", 2.0f);
			animation_states->PushBack(tovertical_state);
		}
		{
			SmartScriptTable tohorizontal_state(gEnv->pScriptSystem);
			tohorizontal_state->SetValue("name", "tohorizontal");
			tohorizontal_state->SetValue("animation", "VTOL_Transport_EngRear_to_flying");
			tohorizontal_state->SetValue("isLooped", false);
			tohorizontal_state->SetValue("speedDefault", 1.0f);
			tohorizontal_state->SetValue("speedMin", 1.0f);
			tohorizontal_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(tohorizontal_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* wingsRear = new VehicleAnimation();
		wingsRear->Init(this, animation);
		m_animations.push_back({ "wingsRear", wingsRear });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "landingGears");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable opened_state(gEnv->pScriptSystem);
			opened_state->SetValue("name", "opened");
			opened_state->SetValue("animation", "VTOL_Transport_LG_Open");
			opened_state->SetValue("sound", "Sounds/vehicles:us_vtol:landing_gear_out");
			opened_state->SetValue("isLooped", false);
			opened_state->SetValue("speedDefault", 1.0f);
			opened_state->SetValue("speedMin", 1.0f);
			opened_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(opened_state);
		}
		{
			SmartScriptTable closed_state(gEnv->pScriptSystem);
			closed_state->SetValue("name", "closed");
			closed_state->SetValue("animation", "VTOL_Transport_LG_Close");
			closed_state->SetValue("sound", "Sounds/vehicles:us_vtol:landing_gear_in");
			closed_state->SetValue("isLooped", false);
			closed_state->SetValue("speedDefault", 1.0f);
			closed_state->SetValue("speedMin", 1.0f);
			closed_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(closed_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* landingGears = new VehicleAnimation();
		landingGears->Init(this, animation);
		m_animations.push_back({ "landingGears", landingGears });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "mg_fire");
		animation->SetValue("part", "chassis");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			shoot_state->SetValue("animation", "Vtol_MG_fire");
			shoot_state->SetValue("isLooped", false);
			shoot_state->SetValue("speedDefault", 1.0f);
			shoot_state->SetValue("speedMin", 1.0f);
			shoot_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(shoot_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* mg_fire = new VehicleAnimation();
		mg_fire->Init(this, animation);
		m_animations.push_back({ "mg_fire", mg_fire });
	}

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("engineIgnitionTime", 0.0f);
		movement->SetValue("enginePowerMax", 1.45f);
		movement->SetValue("rotationDamping", 19.0f);
		movement->SetValue("altitudeMax", 1050.0f);
		movement->SetValue("rotorDiskTiltScale", 0.8f);
		movement->SetValue("pitchResponsiveness", 10.0f);
		movement->SetValue("rollResponsiveness", 3.0f);
		movement->SetValue("yawResponsiveness", 14.0f);
		movement->SetValue("maxYawRate", 3.142f);
		movement->SetValue("maxFwdSpeed", 85.0f);
		movement->SetValue("maxLeftSpeed", 50.0f);
		movement->SetValue("maxSpeed", 14.0f);
		movement->SetValue("maxUpSpeed", 17.0f);
		movement->SetValue("basicSpeedFraction", 0.7f);
		movement->SetValue("yawDecreaseWithSpeed", 40.0f);
		movement->SetValue("tiltPerVelDifference", 0.04f);
		movement->SetValue("maxTiltAngle", 0.15f);
		movement->SetValue("extraRollForTurn", 0.01f);
		movement->SetValue("yawPerRoll", -0.45f);
		movement->SetValue("pitchActionPerTilt", 5.0f);
		movement->SetValue("powerInputConst", 0.4f);
		movement->SetValue("powerInputDamping", -0.5f);
		movement->SetValue("yawInputConst", 3.0f);
		movement->SetValue("yawInputDamping", -0.5f);
		movement->SetValue("impulseHelper", "impulsePos");
		movement->SetValue("WingComponentLeft", "WingLeft");
		movement->SetValue("WingComponentRight", "WingRight");
		movement->SetValue("engineWarmupDelay", 1.0f);
		movement->SetValue("horizFwdForce", 24.0f);
		movement->SetValue("horizLeftForce", 16.0f);
		movement->SetValue("boostForce", 0.2f);
		movement->SetValue("strafeForce", 1.0f);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "rotorSmokeOut");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("rpmPitchSpeed", 1.2f);
		movement->SetValue("SoundParams", movement_soundparams);

		if (!this->SetMovement("VTOL", movement))
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
		seat->SetValue("sitHelper", "passenger01_pos");
		seat->SetValue("useBoundsForEntering", true);
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("remotelyUseActionsFromSeat", "gunner");
		seat->SetValue("blurMult", 0.0f);
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("agVehicleName", "US_vtol");
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 0, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0.01, 0.025, 0.001));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* driver = new VehicleSeat();
		driver->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "driver", driver });
	}

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetValue("Seats", seats);
	}

	// SeatTransitions
	this->InitSeatTransitions();

	// Inventory
	this->SetAmmoCapacity("a2ahomingmissile", 30);

	// Actions
	{
		SmartScriptTable actions(gEnv->pScriptSystem);
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "LandingGears");
			SmartScriptTable action_activations(gEnv->pScriptSystem);
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				activation->SetValue("type", "OnGroundCollision");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_landinggears(gEnv->pScriptSystem);
			action_landinggears->SetValue("altitudeToRetractGears", 4.0f);
			action->SetValue("LandingGears", action_landinggears);
			actions->PushBack(action);
		}
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "AutomaticDoor");
			SmartScriptTable action_activations(gEnv->pScriptSystem);
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				activation->SetValue("type", "OnGroundCollision");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_automaticdoor(gEnv->pScriptSystem);
			action_automaticdoor->SetValue("animation", "passengerDoors");
			action_automaticdoor->SetValue("timeMax", 1.0f);
			action->SetValue("AutomaticDoor", action_automaticdoor);
			actions->PushBack(action);
		}
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "EntityAttachment");
			SmartScriptTable action_entityattachment(gEnv->pScriptSystem);
			action_entityattachment->SetValue("class", "US_tank");
			action_entityattachment->SetValue("helper", "tank_pos");
			action->SetValue("EntityAttachment", action_entityattachment);
			actions->PushBack(action);
		}

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Actions", actions);
		this->InitActions(table);
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("submergedRatioMax", 0.5f);
		damages->SetValue("submergedDamageMult", 1.5f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.0099999998f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.0099999998f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.125f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.5f);
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
			damagemultiplier->SetValue("multiplier", 4.0f);
			damagemultiplier->SetValue("splash", 2.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		damages->SetValue("DamageMultipliers", damages_damagemultipliers);
		SmartScriptTable damages_damagesgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable Destroy_damagesgroup(gEnv->pScriptSystem);
			Destroy_damagesgroup->SetValue("name", "Destroy");
			Destroy_damagesgroup->SetValue("useTemplate", "VehicleDestroy1");
			SmartScriptTable Destroy_damagesgroup_damagessubgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.35f);
				damagessubgroup->SetValue("randomness", 0.1f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Explosion");
					SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
					damagebehavior_explosion->SetValue("damage", 500.0f);
					damagebehavior_explosion->SetValue("helper", "impulsePos");
					damagebehavior_explosion->SetValue("pressure", 1000.0f);
					damagebehavior_explosion->SetValue("radius", 15.0f);
					damagebehavior_explosion->SetValue("minRadius", 5.0f);
					damagebehavior_explosion->SetValue("physRadius", 3.0f);
					damagebehavior_explosion->SetValue("minPhysRadius", 2.2f);
					damagebehavior->SetValue("Explosion", damagebehavior_explosion);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
				Destroy_damagesgroup_damagessubgroups->PushBack(damagessubgroup);
			}
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 3.0f);
				damagessubgroup->SetValue("randomness", 0.25f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Burn");
					SmartScriptTable damagebehavior_burn(gEnv->pScriptSystem);
					damagebehavior_burn->SetValue("helper", "impulsePos");
					damagebehavior_burn->SetValue("damage", 5.0f);
					damagebehavior_burn->SetValue("selfDamage", 10.0f);
					damagebehavior_burn->SetValue("radius", 2.0f);
					damagebehavior_burn->SetValue("interval", 0.5f);
					damagebehavior->SetValue("Burn", damagebehavior_burn);
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
