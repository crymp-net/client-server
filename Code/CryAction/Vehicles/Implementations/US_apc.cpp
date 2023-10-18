#include "US_apc.h"

US_apc::US_apc()
{
	// enable US_apc::Init function
	this->Vehicle::PatchVTable();
}

US_apc::~US_apc()
{
}

bool US_apc::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 150.0f;
	m_buoyancy.waterResistance = 2500.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.039999999f;
	m_physics.maxLoggedCollisions = 2;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable tank_part(gEnv->pScriptSystem);
			tank_part->SetValue("name", "tank");
			tank_part->SetValue("class", "Animated");
			tank_part->SetValue("mass", 0.0f);
			tank_part->SetValue("isHidden", false);
			SmartScriptTable tank_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable turret_part(gEnv->pScriptSystem);
				turret_part->SetValue("name", "turret");
				turret_part->SetValue("class", "AnimatedJoint");
				turret_part->SetValue("component", "turret");
				turret_part->SetValue("mass", 10000.0f);
				SmartScriptTable turret_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable cannon_base_part(gEnv->pScriptSystem);
					cannon_base_part->SetValue("name", "cannon_base");
					cannon_base_part->SetValue("class", "AnimatedJoint");
					cannon_base_part->SetValue("component", "turret");
					cannon_base_part->SetValue("mass", 0.0f);
					SmartScriptTable cannon_base_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable cannon_part(gEnv->pScriptSystem);
						cannon_part->SetValue("name", "cannon");
						cannon_part->SetValue("class", "AnimatedJoint");
						cannon_part->SetValue("component", "turret");
						cannon_part->SetValue("mass", 500.0f);
						if (m_modName == "GaussCannon")
						{
							cannon_part->SetValue("useOption", 1);
						}
						else if (m_modName == "MOAC")
						{
							cannon_part->SetValue("useOption", 2);
						}
						else
						{
							cannon_part->SetValue("useOption", 0);
						}
						SmartScriptTable cannon_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable cannonViewPos_helper(gEnv->pScriptSystem);
							cannonViewPos_helper->SetValue("name", "cannonViewPos");
							cannonViewPos_helper->SetValue("position", Vec3(-0.34889999, 0.84960002, 2.7153001));
							cannonViewPos_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonViewPos_helper);
						}
						{
							SmartScriptTable ghostViewPos_helper(gEnv->pScriptSystem);
							ghostViewPos_helper->SetValue("name", "ghostViewPos");
							ghostViewPos_helper->SetValue("position", Vec3(0.018999999, 2.4282, 2.8827));
							ghostViewPos_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(ghostViewPos_helper);
						}
						{
							SmartScriptTable cannonOut_helper(gEnv->pScriptSystem);
							cannonOut_helper->SetValue("name", "cannonOut");
							cannonOut_helper->SetValue("position", Vec3(0, 2.8515999, 2.5114999));
							cannonOut_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOut_helper);
						}
						{
							SmartScriptTable cannonOutLong_helper(gEnv->pScriptSystem);
							cannonOutLong_helper->SetValue("name", "cannonOutLong");
							cannonOutLong_helper->SetValue("position", Vec3(0, 4.6377001, 2.5114999));
							cannonOutLong_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOutLong_helper);
						}
						{
							SmartScriptTable rocketOut1_helper(gEnv->pScriptSystem);
							rocketOut1_helper->SetValue("name", "rocketOut1");
							rocketOut1_helper->SetValue("position", Vec3(-1.4326, 0.1067, 2.9349999));
							rocketOut1_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(rocketOut1_helper);
						}
						{
							SmartScriptTable rocketOut2_helper(gEnv->pScriptSystem);
							rocketOut2_helper->SetValue("name", "rocketOut2");
							rocketOut2_helper->SetValue("position", Vec3(-1.7107, 0.1016, 2.9421999));
							rocketOut2_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(rocketOut2_helper);
						}
						cannon_part->SetValue("Helpers", cannon_part_helpers);
						cannon_base_part_parts->PushBack(cannon_part);
					}
					cannon_base_part->SetValue("Parts", cannon_base_part_parts);
					SmartScriptTable cannon_base_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable cannon_base_part_animatedjoint_rotation(gEnv->pScriptSystem);
					cannon_base_part_animatedjoint_rotation->SetValue("pitchSpeed", 28.0f);
					cannon_base_part_animatedjoint_rotation->SetValue("pitchAccel", 80.0f);
					SmartScriptTable cannon_base_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					cannon_base_part_animatedjoint_rotation_pitchlimits->PushBack(-5.0f);
					cannon_base_part_animatedjoint_rotation_pitchlimits->PushBack(25.0f);
					cannon_base_part_animatedjoint_rotation->SetValue("pitchLimits", cannon_base_part_animatedjoint_rotation_pitchlimits);
					SmartScriptTable cannon_base_part_animatedjoint_rotation_sound(gEnv->pScriptSystem);
					cannon_base_part_animatedjoint_rotation_sound->SetValue("event", "cannon");
					cannon_base_part_animatedjoint_rotation_sound->SetValue("helper", "turretPos");
					cannon_base_part_animatedjoint_rotation->SetValue("Sound", cannon_base_part_animatedjoint_rotation_sound);
					cannon_base_part_animatedjoint->SetValue("Rotation", cannon_base_part_animatedjoint_rotation);
					cannon_base_part->SetValue("AnimatedJoint", cannon_base_part_animatedjoint);
					turret_part_parts->PushBack(cannon_base_part);
				}
				turret_part->SetValue("Parts", turret_part_parts);
				SmartScriptTable turret_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable driverSitPos_helper(gEnv->pScriptSystem);
					driverSitPos_helper->SetValue("name", "driverSitPos");
					driverSitPos_helper->SetValue("position", Vec3(0, 0.38789999, 0.93989998));
					driverSitPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(driverSitPos_helper);
				}
				{
					SmartScriptTable turretPos_helper(gEnv->pScriptSystem);
					turretPos_helper->SetValue("name", "turretPos");
					turretPos_helper->SetValue("position", Vec3(0.00050000002, -0.1104, 2.0906999));
					turretPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(turretPos_helper);
				}
				turret_part->SetValue("Helpers", turret_part_helpers);
				SmartScriptTable turret_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable turret_part_animatedjoint_rotation(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation->SetValue("yawSpeed", 28.0f);
				turret_part_animatedjoint_rotation->SetValue("yawAccel", 90.0f);
				SmartScriptTable turret_part_animatedjoint_rotation_sound(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation_sound->SetValue("event", "turret");
				turret_part_animatedjoint_rotation_sound->SetValue("eventDamage", "sounds/vehicles:vehicle_accessories:turret_turn_damaged");
				turret_part_animatedjoint_rotation_sound->SetValue("helper", "turretPos");
				turret_part_animatedjoint_rotation->SetValue("Sound", turret_part_animatedjoint_rotation_sound);
				turret_part_animatedjoint->SetValue("Rotation", turret_part_animatedjoint_rotation);
				turret_part->SetValue("AnimatedJoint", turret_part_animatedjoint);
				tank_part_parts->PushBack(turret_part);
			}
			{
				SmartScriptTable wheel1_part(gEnv->pScriptSystem);
				wheel1_part->SetValue("name", "wheel1");
				wheel1_part->SetValue("class", "SubPartWheel");
				wheel1_part->SetValue("isHidden", false);
				SmartScriptTable wheel1_part_subpartwheel(gEnv->pScriptSystem);
				wheel1_part_subpartwheel->SetValue("axle", 0);
				wheel1_part_subpartwheel->SetValue("density", 3000.0f);
				wheel1_part_subpartwheel->SetValue("damping", 0.0f);
				wheel1_part_subpartwheel->SetValue("driving", false);
				wheel1_part_subpartwheel->SetValue("lenMax", 0.0f);
				wheel1_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel1_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel1_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel1_part_subpartwheel->SetValue("suspLength", 0.0f);
				wheel1_part_subpartwheel->SetValue("rayCast", false);
				wheel1_part->SetValue("SubPartWheel", wheel1_part_subpartwheel);
				tank_part_parts->PushBack(wheel1_part);
			}
			{
				SmartScriptTable wheel2_part(gEnv->pScriptSystem);
				wheel2_part->SetValue("name", "wheel2");
				wheel2_part->SetValue("class", "SubPartWheel");
				wheel2_part->SetValue("isHidden", false);
				SmartScriptTable wheel2_part_subpartwheel(gEnv->pScriptSystem);
				wheel2_part_subpartwheel->SetValue("axle", 1);
				wheel2_part_subpartwheel->SetValue("density", 3000.0f);
				wheel2_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel2_part_subpartwheel->SetValue("driving", false);
				wheel2_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel2_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel2_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel2_part_subpartwheel->SetValue("rayCast", false);
				wheel2_part->SetValue("SubPartWheel", wheel2_part_subpartwheel);
				tank_part_parts->PushBack(wheel2_part);
			}
			{
				SmartScriptTable wheel3_part(gEnv->pScriptSystem);
				wheel3_part->SetValue("name", "wheel3");
				wheel3_part->SetValue("class", "SubPartWheel");
				wheel3_part->SetValue("isHidden", false);
				SmartScriptTable wheel3_part_subpartwheel(gEnv->pScriptSystem);
				wheel3_part_subpartwheel->SetValue("axle", 2);
				wheel3_part_subpartwheel->SetValue("density", 3000.0f);
				wheel3_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel3_part_subpartwheel->SetValue("driving", false);
				wheel3_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel3_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel3_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel3_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel3_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel3_part_subpartwheel->SetValue("rayCast", true);
				wheel3_part->SetValue("SubPartWheel", wheel3_part_subpartwheel);
				tank_part_parts->PushBack(wheel3_part);
			}
			{
				SmartScriptTable wheel4_part(gEnv->pScriptSystem);
				wheel4_part->SetValue("name", "wheel4");
				wheel4_part->SetValue("class", "SubPartWheel");
				wheel4_part->SetValue("isHidden", false);
				SmartScriptTable wheel4_part_subpartwheel(gEnv->pScriptSystem);
				wheel4_part_subpartwheel->SetValue("axle", 3);
				wheel4_part_subpartwheel->SetValue("density", 3000.0f);
				wheel4_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel4_part_subpartwheel->SetValue("driving", false);
				wheel4_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel4_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel4_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel4_part_subpartwheel->SetValue("rayCast", true);
				wheel4_part->SetValue("SubPartWheel", wheel4_part_subpartwheel);
				tank_part_parts->PushBack(wheel4_part);
			}
			{
				SmartScriptTable wheel5_part(gEnv->pScriptSystem);
				wheel5_part->SetValue("name", "wheel5");
				wheel5_part->SetValue("class", "SubPartWheel");
				wheel5_part->SetValue("isHidden", false);
				SmartScriptTable wheel5_part_subpartwheel(gEnv->pScriptSystem);
				wheel5_part_subpartwheel->SetValue("axle", 4);
				wheel5_part_subpartwheel->SetValue("density", 3000.0f);
				wheel5_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel5_part_subpartwheel->SetValue("driving", false);
				wheel5_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel5_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel5_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel5_part_subpartwheel->SetValue("rayCast", true);
				wheel5_part->SetValue("SubPartWheel", wheel5_part_subpartwheel);
				tank_part_parts->PushBack(wheel5_part);
			}
			{
				SmartScriptTable wheel6_part(gEnv->pScriptSystem);
				wheel6_part->SetValue("name", "wheel6");
				wheel6_part->SetValue("class", "SubPartWheel");
				wheel6_part->SetValue("isHidden", false);
				SmartScriptTable wheel6_part_subpartwheel(gEnv->pScriptSystem);
				wheel6_part_subpartwheel->SetValue("axle", 5);
				wheel6_part_subpartwheel->SetValue("density", 3000.0f);
				wheel6_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel6_part_subpartwheel->SetValue("driving", false);
				wheel6_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel6_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel6_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel6_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel6_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel6_part_subpartwheel->SetValue("rayCast", true);
				wheel6_part->SetValue("SubPartWheel", wheel6_part_subpartwheel);
				tank_part_parts->PushBack(wheel6_part);
			}
			{
				SmartScriptTable wheel7_part(gEnv->pScriptSystem);
				wheel7_part->SetValue("name", "wheel7");
				wheel7_part->SetValue("class", "SubPartWheel");
				wheel7_part->SetValue("isHidden", false);
				SmartScriptTable wheel7_part_subpartwheel(gEnv->pScriptSystem);
				wheel7_part_subpartwheel->SetValue("axle", 6);
				wheel7_part_subpartwheel->SetValue("density", 3000.0f);
				wheel7_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel7_part_subpartwheel->SetValue("driving", false);
				wheel7_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel7_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel7_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel7_part_subpartwheel->SetValue("rayCast", false);
				wheel7_part->SetValue("SubPartWheel", wheel7_part_subpartwheel);
				tank_part_parts->PushBack(wheel7_part);
			}
			{
				SmartScriptTable wheel8_part(gEnv->pScriptSystem);
				wheel8_part->SetValue("name", "wheel8");
				wheel8_part->SetValue("class", "SubPartWheel");
				wheel8_part->SetValue("isHidden", false);
				SmartScriptTable wheel8_part_subpartwheel(gEnv->pScriptSystem);
				wheel8_part_subpartwheel->SetValue("axle", 7);
				wheel8_part_subpartwheel->SetValue("density", 3000.0f);
				wheel8_part_subpartwheel->SetValue("damping", 0.0f);
				wheel8_part_subpartwheel->SetValue("driving", true);
				wheel8_part_subpartwheel->SetValue("lenMax", 0.0f);
				wheel8_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel8_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel8_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel8_part_subpartwheel->SetValue("suspLength", 0.0f);
				wheel8_part_subpartwheel->SetValue("rayCast", false);
				wheel8_part->SetValue("SubPartWheel", wheel8_part_subpartwheel);
				tank_part_parts->PushBack(wheel8_part);
			}
			{
				SmartScriptTable wheel9_part(gEnv->pScriptSystem);
				wheel9_part->SetValue("name", "wheel9");
				wheel9_part->SetValue("class", "SubPartWheel");
				wheel9_part->SetValue("isHidden", false);
				SmartScriptTable wheel9_part_subpartwheel(gEnv->pScriptSystem);
				wheel9_part_subpartwheel->SetValue("axle", 0);
				wheel9_part_subpartwheel->SetValue("density", 3000.0f);
				wheel9_part_subpartwheel->SetValue("damping", 0.0f);
				wheel9_part_subpartwheel->SetValue("driving", false);
				wheel9_part_subpartwheel->SetValue("lenMax", 0.0f);
				wheel9_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel9_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel9_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel9_part_subpartwheel->SetValue("suspLength", 0.0f);
				wheel9_part_subpartwheel->SetValue("rayCast", false);
				wheel9_part->SetValue("SubPartWheel", wheel9_part_subpartwheel);
				tank_part_parts->PushBack(wheel9_part);
			}
			{
				SmartScriptTable wheel10_part(gEnv->pScriptSystem);
				wheel10_part->SetValue("name", "wheel10");
				wheel10_part->SetValue("class", "SubPartWheel");
				wheel10_part->SetValue("isHidden", false);
				SmartScriptTable wheel10_part_subpartwheel(gEnv->pScriptSystem);
				wheel10_part_subpartwheel->SetValue("axle", 1);
				wheel10_part_subpartwheel->SetValue("density", 3000.0f);
				wheel10_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel10_part_subpartwheel->SetValue("driving", false);
				wheel10_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel10_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel10_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel10_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel10_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel10_part_subpartwheel->SetValue("rayCast", false);
				wheel10_part->SetValue("SubPartWheel", wheel10_part_subpartwheel);
				tank_part_parts->PushBack(wheel10_part);
			}
			{
				SmartScriptTable wheel11_part(gEnv->pScriptSystem);
				wheel11_part->SetValue("name", "wheel11");
				wheel11_part->SetValue("class", "SubPartWheel");
				wheel11_part->SetValue("isHidden", false);
				SmartScriptTable wheel11_part_subpartwheel(gEnv->pScriptSystem);
				wheel11_part_subpartwheel->SetValue("axle", 2);
				wheel11_part_subpartwheel->SetValue("density", 3000.0f);
				wheel11_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel11_part_subpartwheel->SetValue("driving", false);
				wheel11_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel11_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel11_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel11_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel11_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel11_part_subpartwheel->SetValue("rayCast", true);
				wheel11_part->SetValue("SubPartWheel", wheel11_part_subpartwheel);
				tank_part_parts->PushBack(wheel11_part);
			}
			{
				SmartScriptTable wheel12_part(gEnv->pScriptSystem);
				wheel12_part->SetValue("name", "wheel12");
				wheel12_part->SetValue("class", "SubPartWheel");
				wheel12_part->SetValue("isHidden", false);
				SmartScriptTable wheel12_part_subpartwheel(gEnv->pScriptSystem);
				wheel12_part_subpartwheel->SetValue("axle", 3);
				wheel12_part_subpartwheel->SetValue("density", 3000.0f);
				wheel12_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel12_part_subpartwheel->SetValue("driving", false);
				wheel12_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel12_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel12_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel12_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel12_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel12_part_subpartwheel->SetValue("rayCast", true);
				wheel12_part->SetValue("SubPartWheel", wheel12_part_subpartwheel);
				tank_part_parts->PushBack(wheel12_part);
			}
			{
				SmartScriptTable wheel13_part(gEnv->pScriptSystem);
				wheel13_part->SetValue("name", "wheel13");
				wheel13_part->SetValue("class", "SubPartWheel");
				wheel13_part->SetValue("isHidden", false);
				SmartScriptTable wheel13_part_subpartwheel(gEnv->pScriptSystem);
				wheel13_part_subpartwheel->SetValue("axle", 4);
				wheel13_part_subpartwheel->SetValue("density", 3000.0f);
				wheel13_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel13_part_subpartwheel->SetValue("driving", false);
				wheel13_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel13_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel13_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel13_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel13_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel13_part_subpartwheel->SetValue("rayCast", true);
				wheel13_part->SetValue("SubPartWheel", wheel13_part_subpartwheel);
				tank_part_parts->PushBack(wheel13_part);
			}
			{
				SmartScriptTable wheel14_part(gEnv->pScriptSystem);
				wheel14_part->SetValue("name", "wheel14");
				wheel14_part->SetValue("class", "SubPartWheel");
				wheel14_part->SetValue("isHidden", false);
				SmartScriptTable wheel14_part_subpartwheel(gEnv->pScriptSystem);
				wheel14_part_subpartwheel->SetValue("axle", 5);
				wheel14_part_subpartwheel->SetValue("density", 3000.0f);
				wheel14_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel14_part_subpartwheel->SetValue("driving", false);
				wheel14_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel14_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel14_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel14_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel14_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel14_part_subpartwheel->SetValue("rayCast", true);
				wheel14_part->SetValue("SubPartWheel", wheel14_part_subpartwheel);
				tank_part_parts->PushBack(wheel14_part);
			}
			{
				SmartScriptTable wheel15_part(gEnv->pScriptSystem);
				wheel15_part->SetValue("name", "wheel15");
				wheel15_part->SetValue("class", "SubPartWheel");
				wheel15_part->SetValue("isHidden", false);
				SmartScriptTable wheel15_part_subpartwheel(gEnv->pScriptSystem);
				wheel15_part_subpartwheel->SetValue("axle", 6);
				wheel15_part_subpartwheel->SetValue("density", 3000.0f);
				wheel15_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel15_part_subpartwheel->SetValue("driving", false);
				wheel15_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel15_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel15_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel15_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel15_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel15_part_subpartwheel->SetValue("rayCast", false);
				wheel15_part->SetValue("SubPartWheel", wheel15_part_subpartwheel);
				tank_part_parts->PushBack(wheel15_part);
			}
			{
				SmartScriptTable wheel16_part(gEnv->pScriptSystem);
				wheel16_part->SetValue("name", "wheel16");
				wheel16_part->SetValue("class", "SubPartWheel");
				wheel16_part->SetValue("isHidden", false);
				SmartScriptTable wheel16_part_subpartwheel(gEnv->pScriptSystem);
				wheel16_part_subpartwheel->SetValue("axle", 7);
				wheel16_part_subpartwheel->SetValue("density", 3000.0f);
				wheel16_part_subpartwheel->SetValue("damping", 0.0f);
				wheel16_part_subpartwheel->SetValue("driving", true);
				wheel16_part_subpartwheel->SetValue("lenMax", 0.0f);
				wheel16_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel16_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel16_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel16_part_subpartwheel->SetValue("suspLength", 0.0f);
				wheel16_part_subpartwheel->SetValue("rayCast", false);
				wheel16_part->SetValue("SubPartWheel", wheel16_part_subpartwheel);
				tank_part_parts->PushBack(wheel16_part);
			}
			tank_part->SetValue("Parts", tank_part_parts);
			SmartScriptTable tank_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable driverEnterPos_helper(gEnv->pScriptSystem);
				driverEnterPos_helper->SetValue("name", "driverEnterPos");
				driverEnterPos_helper->SetValue("position", Vec3(2.9902, 0, 0.50330001));
				driverEnterPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(driverEnterPos_helper);
			}
			{
				SmartScriptTable passenger_enter_pos_helper(gEnv->pScriptSystem);
				passenger_enter_pos_helper->SetValue("name", "passenger_enter_pos");
				passenger_enter_pos_helper->SetValue("position", Vec3(-0.145, -4.7157998, 0.5007));
				passenger_enter_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(passenger_enter_pos_helper);
			}
			{
				SmartScriptTable passenger1_sit_pos_helper(gEnv->pScriptSystem);
				passenger1_sit_pos_helper->SetValue("name", "passenger1_sit_pos");
				passenger1_sit_pos_helper->SetValue("position", Vec3(-1.0288, -1.2764, 1.1793));
				passenger1_sit_pos_helper->SetValue("direction", Vec3(1, 0, 0));
				tank_part_helpers->PushBack(passenger1_sit_pos_helper);
			}
			{
				SmartScriptTable passenger2_sit_pos_helper(gEnv->pScriptSystem);
				passenger2_sit_pos_helper->SetValue("name", "passenger2_sit_pos");
				passenger2_sit_pos_helper->SetValue("position", Vec3(0.96390003, -1.2832, 1.1811));
				passenger2_sit_pos_helper->SetValue("direction", Vec3(-1, 0, 0));
				tank_part_helpers->PushBack(passenger2_sit_pos_helper);
			}
			{
				SmartScriptTable passenger3_sit_pos_helper(gEnv->pScriptSystem);
				passenger3_sit_pos_helper->SetValue("name", "passenger3_sit_pos");
				passenger3_sit_pos_helper->SetValue("position", Vec3(-1.0063, -2.095, 1.1781));
				passenger3_sit_pos_helper->SetValue("direction", Vec3(1, 0, 0));
				tank_part_helpers->PushBack(passenger3_sit_pos_helper);
			}
			{
				SmartScriptTable passenger4_sit_pos_helper(gEnv->pScriptSystem);
				passenger4_sit_pos_helper->SetValue("name", "passenger4_sit_pos");
				passenger4_sit_pos_helper->SetValue("position", Vec3(0.95020002, -2.0762, 1.1923));
				passenger4_sit_pos_helper->SetValue("direction", Vec3(-1, 0, 0));
				tank_part_helpers->PushBack(passenger4_sit_pos_helper);
			}
			{
				SmartScriptTable passenger5_sit_pos_helper(gEnv->pScriptSystem);
				passenger5_sit_pos_helper->SetValue("name", "passenger5_sit_pos");
				passenger5_sit_pos_helper->SetValue("position", Vec3(-1.0063, -2.8183999, 1.1781));
				passenger5_sit_pos_helper->SetValue("direction", Vec3(1, 0, 0));
				tank_part_helpers->PushBack(passenger5_sit_pos_helper);
			}
			{
				SmartScriptTable passenger6_sit_pos_helper(gEnv->pScriptSystem);
				passenger6_sit_pos_helper->SetValue("name", "passenger6_sit_pos");
				passenger6_sit_pos_helper->SetValue("position", Vec3(0.93580002, -2.8039999, 1.1998));
				passenger6_sit_pos_helper->SetValue("direction", Vec3(-1, 0, 0));
				tank_part_helpers->PushBack(passenger6_sit_pos_helper);
			}
			{
				SmartScriptTable engineSmokeOut_helper(gEnv->pScriptSystem);
				engineSmokeOut_helper->SetValue("name", "engineSmokeOut");
				engineSmokeOut_helper->SetValue("position", Vec3(-0.0125, 2.6085999, 1.5766));
				engineSmokeOut_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(engineSmokeOut_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, -1.3347, 0.0034));
				burningPos_helper->SetValue("direction", Vec3(0, 0, 1));
				tank_part_helpers->PushBack(burningPos_helper);
			}
			{
				SmartScriptTable engineSoundPos_helper(gEnv->pScriptSystem);
				engineSoundPos_helper->SetValue("name", "engineSoundPos");
				engineSoundPos_helper->SetValue("position", Vec3(-0.0076000001, 2.6157, 1.4867001));
				engineSoundPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(engineSoundPos_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(1.4457999, 3.3799, 1.4088));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				tank_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(-1.4629, 3.3503001, 1.4081));
				headLightRight_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				tank_part_helpers->PushBack(headLightRight_helper);
			}
			{
				SmartScriptTable headLightCenter_helper(gEnv->pScriptSystem);
				headLightCenter_helper->SetValue("name", "headLightCenter");
				headLightCenter_helper->SetValue("position", Vec3(0.0178, 3.4784999, 1.3498));
				headLightCenter_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				tank_part_helpers->PushBack(headLightCenter_helper);
			}
			{
				SmartScriptTable rearLightLeft_helper(gEnv->pScriptSystem);
				rearLightLeft_helper->SetValue("name", "rearLightLeft");
				rearLightLeft_helper->SetValue("position", Vec3(-1.2488, -3.6496999, 1.9815));
				rearLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				tank_part_helpers->PushBack(rearLightLeft_helper);
			}
			{
				SmartScriptTable rearLightRight_helper(gEnv->pScriptSystem);
				rearLightRight_helper->SetValue("name", "rearLightRight");
				rearLightRight_helper->SetValue("position", Vec3(1.2437, -3.6399, 1.9766999));
				rearLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				tank_part_helpers->PushBack(rearLightRight_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(0, -0.92309999, 1.1338));
				centerPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(centerPos_helper);
			}
			{
				SmartScriptTable hullDamage_helper(gEnv->pScriptSystem);
				hullDamage_helper->SetValue("name", "hullDamage");
				hullDamage_helper->SetValue("position", Vec3(0, -0.28830001, 1.4329));
				hullDamage_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(hullDamage_helper);
			}
			tank_part->SetValue("Helpers", tank_part_helpers);
			SmartScriptTable tank_part_animated(gEnv->pScriptSystem);
			tank_part_animated->SetValue("filename", "Objects/Vehicles/us_apc/us_apc.cga");
			tank_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/us_apc/us_apc_damaged.cga");
			tank_part->SetValue("Animated", tank_part_animated);
			parts->PushBack(tank_part);
		}
		{
			SmartScriptTable leftTread_part(gEnv->pScriptSystem);
			leftTread_part->SetValue("name", "leftTread");
			leftTread_part->SetValue("class", "Tread");
			leftTread_part->SetValue("component", "leftTread");
			SmartScriptTable leftTread_part_tread(gEnv->pScriptSystem);
			leftTread_part_tread->SetValue("filename", "Objects/Vehicles/us_apc/tread_left.chr");
			leftTread_part_tread->SetValue("materialName", "us_apc_tread");
			leftTread_part_tread->SetValue("uvRateMax", 23.0f);
			leftTread_part_tread->SetValue("uvSliceCount", 12.0f);
			leftTread_part->SetValue("Tread", leftTread_part_tread);
			parts->PushBack(leftTread_part);
		}
		{
			SmartScriptTable rightTread_part(gEnv->pScriptSystem);
			rightTread_part->SetValue("name", "rightTread");
			rightTread_part->SetValue("class", "Tread");
			rightTread_part->SetValue("component", "rightTread");
			SmartScriptTable rightTread_part_tread(gEnv->pScriptSystem);
			rightTread_part_tread->SetValue("filename", "Objects/Vehicles/us_apc/tread_right.chr");
			rightTread_part_tread->SetValue("materialName", "us_apc_tread");
			rightTread_part_tread->SetValue("uvRateMax", 23.0f);
			rightTread_part_tread->SetValue("uvSliceCount", 12.0f);
			rightTread_part->SetValue("Tread", rightTread_part_tread);
			parts->PushBack(rightTread_part);
		}
		{
			SmartScriptTable massBox_part(gEnv->pScriptSystem);
			massBox_part->SetValue("name", "massBox");
			massBox_part->SetValue("class", "MassBox");
			massBox_part->SetValue("component", "");
			massBox_part->SetValue("mass", 20000.0f);
			massBox_part->SetValue("position", Vec3(0.0017, 0.024700001, 0.8775));
			massBox_part->SetValue("disablePhysics", false);
			massBox_part->SetValue("disableCollision", false);
			massBox_part->SetValue("isHidden", false);
			SmartScriptTable massBox_part_helpers(gEnv->pScriptSystem);
			massBox_part->SetValue("Helpers", massBox_part_helpers);
			SmartScriptTable massBox_part_massbox(gEnv->pScriptSystem);
			massBox_part_massbox->SetValue("size", Vec3(2, 3, 0.76999998));
			massBox_part->SetValue("MassBox", massBox_part_massbox);
			parts->PushBack(massBox_part);
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
		{
			SmartScriptTable rearLightLeft_part(gEnv->pScriptSystem);
			rearLightLeft_part->SetValue("name", "rearLightLeft");
			rearLightLeft_part->SetValue("class", "Light");
			rearLightLeft_part->SetValue("component", "rearLightsLeft");
			rearLightLeft_part->SetValue("helper", "rearLightLeft");
			SmartScriptTable rearLightLeft_part_light(gEnv->pScriptSystem);
			rearLightLeft_part_light->SetValue("type", "Rear");
			rearLightLeft_part->SetValue("Light", rearLightLeft_part_light);
			parts->PushBack(rearLightLeft_part);
		}
		{
			SmartScriptTable rearLightRight_part(gEnv->pScriptSystem);
			rearLightRight_part->SetValue("name", "rearLightRight");
			rearLightRight_part->SetValue("class", "Light");
			rearLightRight_part->SetValue("component", "rearLightsRight");
			rearLightRight_part->SetValue("helper", "rearLightRight");
			SmartScriptTable rearLightRight_part_light(gEnv->pScriptSystem);
			rearLightRight_part_light->SetValue("type", "Rear");
			rearLightRight_part->SetValue("Light", rearLightRight_part_light);
			parts->PushBack(rearLightRight_part);
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
		component->SetValue("damageMax", 700.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-2, -3.7, 0));
		component->SetValue("maxBound", Vec3(2, 3.5, 2.2));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			if (m_modName == "MP")
			{
				damagemultiplier->SetValue("multiplier", 2.0f);
			}
			else
			{
				damagemultiplier->SetValue("multiplier", 1.25f);
			}
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.5f);
			damagemultiplier->SetValue("splash", 2.0f);
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
			damagebehavior_indicator_light->SetValue("sound", "sounds/vehicles:vehicle_accessories:warning_beep_tank");
			damagebehavior_indicator_light->SetValue("soundRatioMin", 0.75f);
			damagebehavior_indicator_light->SetValue("helper", "turretPos");
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
		component->SetValue("name", "engine");
		component->SetValue("damageMax", 450.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.2998, 1.2156, 0.6476));
		component->SetValue("maxBound", Vec3(1.3789001, 3.4363, 1.9907));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.05f);
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
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.5f);
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
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			damagebehavior->SetValue("damageRatioMax", 0.75f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "EngineDamaged25");
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
			damagebehavior_effect->SetValue("effect", "EngineDamaged75");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* engine = new VehicleComponent();
		engine->Init(this, component);
		m_components.push_back(engine);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "turret");
		component->SetValue("damageMax", 650.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-2.5, -3.2, 2.2));
		component->SetValue("maxBound", Vec3(2.5, 2.5, 3.3));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.2f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* turret = new VehicleComponent();
		turret->Init(this, component);
		m_components.push_back(turret);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "leftTread");
		component->SetValue("damageMax", 300.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.75, -3.65, -0.2166));
		component->SetValue("maxBound", Vec3(-1.0854, 3.5, 1.3332));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.2f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* leftTread = new VehicleComponent();
		leftTread->Init(this, component);
		m_components.push_back(leftTread);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "rightTread");
		component->SetValue("damageMax", 300.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(1.1348, -3.65, -0.2191));
		component->SetValue("maxBound", Vec3(1.75, 3.5, 1.3308001));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.2f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* rightTread = new VehicleComponent();
		rightTread->Init(this, component);
		m_components.push_back(rightTread);
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
		particles_exhaust->SetValue("insideWater", false);
		particles_exhaust->SetValue("outsideWater", true);
		SmartScriptTable particles_exhaust_helpers(gEnv->pScriptSystem);
		particles_exhaust_helpers->PushBack("exhaust_left_pos");
		particles_exhaust_helpers->PushBack("exhaust_right_pos");
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
		particles_exhaust_enginerunning->SetValue("maxSpeed", 15.0f);
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
			VehicleDestroyed_damageeffect->SetValue("helper", "burningPos");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.tank_explosion.chinese_tank");
			VehicleDestroyed_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable VehicleFlipped_damageeffect(gEnv->pScriptSystem);
			VehicleFlipped_damageeffect->SetValue("name", "VehicleFlipped");
			VehicleFlipped_damageeffect->SetValue("helper", "centerPos");
			VehicleFlipped_damageeffect->SetValue("effect", "smoke_and_fire.Jeep.flipped_heavy");
			particles_damageeffects->PushBack(VehicleFlipped_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged25_damageeffect(gEnv->pScriptSystem);
			EngineDamaged25_damageeffect->SetValue("name", "EngineDamaged25");
			EngineDamaged25_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.USapc_engine_smoke_a");
			EngineDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged75_damageeffect(gEnv->pScriptSystem);
			EngineDamaged75_damageeffect->SetValue("name", "EngineDamaged75");
			EngineDamaged75_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.USapc_engine_smoke_b");
			EngineDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged75_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "hullDamage");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.USapc_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "hullDamage");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.USapc_hull_smoke_b");
			HullDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 0.5f);
			environmentlayer->SetValue("minSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			environmentlayer->SetValue("minSpeedSpeedScale", 1.0f);
			environmentlayer->SetValue("maxSpeed", 20.0f);
			environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			environmentlayer->SetValue("maxSpeedSpeedScale", 1.0f);
			SmartScriptTable environmentlayer_alignment(gEnv->pScriptSystem);
			environmentlayer_alignment->SetValue("alignGroundHeight", 0.0f);
			environmentlayer_alignment->SetValue("maxHeightSizeScale", 1.0f);
			environmentlayer_alignment->SetValue("maxHeightCountScale", 1.0f);
			environmentlayer_alignment->SetValue("alignToWater", false);
			environmentlayer->SetValue("Alignment", environmentlayer_alignment);
			SmartScriptTable environmentlayer_emitters(gEnv->pScriptSystem);
			environmentlayer->SetValue("Emitters", environmentlayer_emitters);
			SmartScriptTable environmentlayer_wheels(gEnv->pScriptSystem);
			SmartScriptTable environmentlayer_wheels_wheelgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(2);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(7);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(10);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(15);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			environmentlayer_wheels->SetValue("WheelGroups", environmentlayer_wheels_wheelgroups);
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
		animation->SetValue("name", "cannon_fire");
		animation->SetValue("part", "tank");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			shoot_state->SetValue("animation", "apc_cannon_recoil");
			shoot_state->SetValue("isLooped", false);
			shoot_state->SetValue("speedDefault", 1.0f);
			shoot_state->SetValue("speedMin", 1.0f);
			shoot_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(shoot_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* cannon_fire = new VehicleAnimation();
		cannon_fire->Init(this, animation);
		m_animations.push_back({ "cannon_fire", cannon_fire });
	}

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("pedalSpeed", 7.0f);
		movement->SetValue("pedalThreshold", 0.0f);
		movement->SetValue("steerSpeed", 10.0f);
		movement->SetValue("steerSpeedRelax", 7.0f);
		movement->SetValue("steerLimit", 1.0f);
		movement->SetValue("latFricMin", 0.60000002f);
		movement->SetValue("latFricMinSteer", 0.1f);
		movement->SetValue("latFricMax", 1.2f);
		movement->SetValue("latSlipMin", 0.89999998f);
		movement->SetValue("latSlipMax", 2.5999999f);
		movement->SetValue("rpmInterpSpeed", 2.0f);
		movement->SetValue("rpmRelaxSpeed", 2.0f);
		movement->SetValue("rpmGearShiftSpeed", 2.0f);
		movement->SetValue("steeringImpulseMin", 1.0f);
		movement->SetValue("steeringImpulseMax", 2.3f);
		movement->SetValue("steeringImpulseRelaxMin", 4.0f);
		movement->SetValue("steeringImpulseRelaxMax", 6.0f);
		SmartScriptTable movement_wheeled(gEnv->pScriptSystem);
		movement_wheeled->SetValue("axleFriction", 450);
		movement_wheeled->SetValue("axleFrictionMax", 4000);
		movement_wheeled->SetValue("brakeTorque", 5000);
		movement_wheeled->SetValue("clutchSpeed", 8.0f);
		movement_wheeled->SetValue("damping", 0.02f);
		movement_wheeled->SetValue("engineIdleRPM", 600);
		movement_wheeled->SetValue("engineMaxRPM", 2800);
		movement_wheeled->SetValue("engineMinRPM", 200);
		movement_wheeled->SetValue("enginePower", 650);
		movement_wheeled->SetValue("engineShiftDownRPM", 1200);
		movement_wheeled->SetValue("engineShiftUpRPM", 2300);
		movement_wheeled->SetValue("engineStartRPM", 600);
		movement_wheeled->SetValue("stabilizer", 0.25f);
		movement_wheeled->SetValue("minBrakingFriction", 1.1f);
		movement_wheeled->SetValue("maxSteer", 0.78500003f);
		movement_wheeled->SetValue("maxSpeed", 15.0f);
		movement_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_wheeled->SetValue("minEnergy", 0.039999999f);
		movement_wheeled->SetValue("slipThreshold", 0.050000001f);
		movement_wheeled->SetValue("gearDirSwitchRPM", 2500);
		movement_wheeled->SetValue("dynFriction", 1.0f);
		movement_wheeled->SetValue("steerTrackNeutralTurn", 0.78500003f);
		movement_wheeled->SetValue("pullTilt", 2.0f);
		SmartScriptTable movement_wheeled_gearratios(gEnv->pScriptSystem);
		movement_wheeled_gearratios->PushBack(-12.0f);
		movement_wheeled_gearratios->PushBack(0.0f);
		movement_wheeled_gearratios->PushBack(10.0f);
		movement_wheeled_gearratios->PushBack(8.0f);
		movement_wheeled_gearratios->PushBack(7.0f);
		movement_wheeled->SetValue("gearRatios", movement_wheeled_gearratios);
		movement->SetValue("Wheeled", movement_wheeled);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSoundPos");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("rpmPitchSpeed", 0.0f);
		movement_soundparams->SetValue("roadBumpMinSusp", 8.0f);
		movement_soundparams->SetValue("roadBumpMinSpeed", 8.0f);
		movement_soundparams->SetValue("roadBumpIntensity", 0.30000001f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable pullTilt_tweak(gEnv->pScriptSystem);
				pullTilt_tweak->SetValue("name", "pullTilt");
				pullTilt_tweak->SetValue("value", 2.0f);
				pullTilt_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(pullTilt_tweak);
			}
			{
				SmartScriptTable engineMaxRPM_tweak(gEnv->pScriptSystem);
				engineMaxRPM_tweak->SetValue("name", "engineMaxRPM");
				engineMaxRPM_tweak->SetValue("value", 1.2f);
				engineMaxRPM_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(engineMaxRPM_tweak);
			}
			{
				SmartScriptTable enginePower_tweak(gEnv->pScriptSystem);
				enginePower_tweak->SetValue("name", "enginePower");
				enginePower_tweak->SetValue("value", 1.1f);
				enginePower_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(enginePower_tweak);
			}
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);

		if (!this->SetMovement("Tank", movement))
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
		seat->SetValue("enterHelper", "driverEnterPos");
		seat->SetValue("sitHelper", "driverSitPos");
		seat->SetValue("isDriver", true);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("isPassengerHidden", true);
		seat->SetValue("AimPart", "cannon");
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("explosionShakeMult", 0.2f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "cannonViewPos");
			view_firstperson->SetValue("frameObject", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.75, 1));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0.5));
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
			seataction_rotateturret->SetValue("pitchPart", "cannon_base");
			seataction_rotateturret->SetValue("yawPart", "turret");
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
				if (m_modName == "GaussCannon")
				{
					weapon->SetValue("class", "GaussCannon");
				}
				else if (m_modName == "MOAC")
				{
					weapon->SetValue("class", "VehicleMOAC");
				}
				else
				{
					weapon->SetValue("class", "APCCannon");
				}
				weapon->SetValue("part", "cannon");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("cannonOut");
				weapon->SetValue("Helpers", weapon_helpers);
				SmartScriptTable weapon_actions(gEnv->pScriptSystem);
				{
					SmartScriptTable fire_action(gEnv->pScriptSystem);
					fire_action->SetValue("name", "fire");
					SmartScriptTable fire_action_animations(gEnv->pScriptSystem);
					{
						SmartScriptTable cannon_fire_animation(gEnv->pScriptSystem);
						if (m_modName == "MOAC")
						{
							cannon_fire_animation->SetValue("name", "");
						}
						else
						{
							cannon_fire_animation->SetValue("name", "cannon_fire");
						}
						fire_action_animations->PushBack(cannon_fire_animation);
					}
					fire_action->SetValue("Animations", fire_action_animations);
					weapon_actions->PushBack(fire_action);
				}
				weapon->SetValue("Actions", weapon_actions);
				seataction_weapons_weapons->PushBack(weapon);
			}
			seataction_weapons->SetValue("Weapons", seataction_weapons_weapons);
			seataction->SetValue("Weapons", seataction_weapons);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Weapons");
			SmartScriptTable seataction_weapons(gEnv->pScriptSystem);
			seataction_weapons->SetValue("isSecondary", true);
			SmartScriptTable seataction_weapons_weapons(gEnv->pScriptSystem);
			{
				SmartScriptTable weapon(gEnv->pScriptSystem);
				weapon->SetValue("class", "APCRocketLauncher");
				weapon->SetValue("part", "turret");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("rocketOut1");
				weapon_helpers->PushBack("rocketOut2");
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
			SmartScriptTable seataction_lights(gEnv->pScriptSystem);
			seataction_lights->SetValue("activation", "toggle");
			SmartScriptTable seataction_lights_lightparts(gEnv->pScriptSystem);
			seataction_lights_lightparts->PushBack("headLightLeft");
			seataction_lights_lightparts->PushBack("headLightLeftFlare");
			seataction_lights_lightparts->PushBack("headLightRight");
			seataction_lights_lightparts->PushBack("headLightRightFlare");
			seataction_lights_lightparts->PushBack("headLightCenter");
			seataction_lights_lightparts->PushBack("rearLightLeft");
			seataction_lights_lightparts->PushBack("rearLightRight");
			seataction_lights->SetValue("LightParts", seataction_lights_lightparts);
			seataction->SetValue("Lights", seataction_lights);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* driver = new VehicleSeat();
		driver->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "driver", driver });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat1");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger1_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-35, 0, -70));
			view->SetValue("rotationMax", Vec3(20, 0, 70));
			view->SetValue("hidePlayer", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_hideparts(gEnv->pScriptSystem);
			view_hideparts->PushBack("leftTread");
			view_hideparts->PushBack("rightTread");
			view->SetValue("HideParts", view_hideparts);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.5, 0.5));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat1 = new VehicleSeat();
		backseat1->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat1", backseat1 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat2");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger2_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-35, 0, -70));
			view->SetValue("rotationMax", Vec3(20, 0, 70));
			view->SetValue("hidePlayer", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_hideparts(gEnv->pScriptSystem);
			view_hideparts->PushBack("leftTread");
			view_hideparts->PushBack("rightTread");
			view->SetValue("HideParts", view_hideparts);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.5, 0.5));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat2 = new VehicleSeat();
		backseat2->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat2", backseat2 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat3");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger3_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-35, 0, -70));
			view->SetValue("rotationMax", Vec3(20, 0, 70));
			view->SetValue("hidePlayer", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_hideparts(gEnv->pScriptSystem);
			view_hideparts->PushBack("leftTread");
			view_hideparts->PushBack("rightTread");
			view->SetValue("HideParts", view_hideparts);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.5, 0.5));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat3 = new VehicleSeat();
		backseat3->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat3", backseat3 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat4");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger4_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-35, 0, -70));
			view->SetValue("rotationMax", Vec3(20, 0, 70));
			view->SetValue("hidePlayer", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_hideparts(gEnv->pScriptSystem);
			view_hideparts->PushBack("leftTread");
			view_hideparts->PushBack("rightTread");
			view->SetValue("HideParts", view_hideparts);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.5, 0.5));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat4 = new VehicleSeat();
		backseat4->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat4", backseat4 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat5");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger5_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-35, 0, -70));
			view->SetValue("rotationMax", Vec3(20, 0, 70));
			view->SetValue("hidePlayer", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_hideparts(gEnv->pScriptSystem);
			view_hideparts->PushBack("leftTread");
			view_hideparts->PushBack("rightTread");
			view->SetValue("HideParts", view_hideparts);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.5, 0.5));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat5 = new VehicleSeat();
		backseat5->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat5", backseat5 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat6");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger6_sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-35, 0, -70));
			view->SetValue("rotationMax", Vec3(20, 0, 70));
			view->SetValue("hidePlayer", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_hideparts(gEnv->pScriptSystem);
			view_hideparts->PushBack("leftTread");
			view_hideparts->PushBack("rightTread");
			view->SetValue("HideParts", view_hideparts);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -4.5, 0.5));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 3, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0, 0.050000001, 1));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat6 = new VehicleSeat();
		backseat6->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat6", backseat6 });
	}

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetValue("Seats", seats);
	}

	// SeatTransitions
	this->InitSeatTransitions();

	// Inventory
	this->SetAmmoCapacity("tank30", 300);
	this->SetAmmoCapacity("towmissile", 6);

	// Paints
	if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/us_apc/us_apc_nk.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/us_apc/us_apc.mtl");
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("submergedRatioMax", 0.89999998f);
		damages->SetValue("submergedDamageMult", 0.5f);
		damages->SetValue("collDamageThreshold", 400.0f);
		damages->SetValue("groundCollisionMinMult", 1.0f);
		damages->SetValue("groundCollisionMaxMult", 1.0f);
		damages->SetValue("groundCollisionMinSpeed", 18.0f);
		damages->SetValue("groundCollisionMaxSpeed", 18.0f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.0f);
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
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.05f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.025f);
			damagemultiplier->SetValue("splash", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "c4");
			damagemultiplier->SetValue("multiplier", 1.5f);
			damagemultiplier->SetValue("splash", 0.5f);
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
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		damages->SetValue("DamageMultipliers", damages_damagemultipliers);
		SmartScriptTable damages_damagesgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable Destroy_damagesgroup(gEnv->pScriptSystem);
			Destroy_damagesgroup->SetValue("name", "Destroy");
			Destroy_damagesgroup->SetValue("useTemplate", "TankDestroy");
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
					damagebehavior->SetValue("ignoreVehicleDestruction", true);
					SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
					damagebehavior_impulse->SetValue("forceMin", 3.0f);
					damagebehavior_impulse->SetValue("forceMax", 4.0f);
					damagebehavior_impulse->SetValue("direction", Vec3(0, 0, 1));
					damagebehavior_impulse->SetValue("momentum", Vec3(0.5, 0.5, 0));
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
