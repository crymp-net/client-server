#include "Asian_aaa.h"

Asian_aaa::Asian_aaa()
{
	// enable Asian_aaa::Init function
	this->Vehicle::PatchVTable();
}

Asian_aaa::~Asian_aaa()
{
}

bool Asian_aaa::Init(IGameObject* pGameObject)
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
	m_physics.minEnergy = 0.01f;
	m_physics.maxLoggedCollisions = 2;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable body_part(gEnv->pScriptSystem);
			body_part->SetValue("name", "body");
			body_part->SetValue("class", "Animated");
			body_part->SetValue("mass", 0.0f);
			SmartScriptTable body_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable hull_part(gEnv->pScriptSystem);
				hull_part->SetValue("name", "hull");
				hull_part->SetValue("class", "AnimatedJoint");
				hull_part->SetValue("component", "hull");
				hull_part->SetValue("mass", 0.0f);
				SmartScriptTable hull_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable turret_part(gEnv->pScriptSystem);
					turret_part->SetValue("name", "turret");
					turret_part->SetValue("class", "AnimatedJoint");
					turret_part->SetValue("component", "turret");
					turret_part->SetValue("mass", 0.0f);
					SmartScriptTable turret_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable radar_part(gEnv->pScriptSystem);
						radar_part->SetValue("name", "radar");
						radar_part->SetValue("class", "AnimatedJoint");
						radar_part->SetValue("component", "radar");
						radar_part->SetValue("mass", 100.0f);
						SmartScriptTable radar_part_animatedjoint(gEnv->pScriptSystem);
						radar_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 0.1));
						radar_part->SetValue("AnimatedJoint", radar_part_animatedjoint);
						turret_part_parts->PushBack(radar_part);
					}
					{
						SmartScriptTable cannon_part(gEnv->pScriptSystem);
						cannon_part->SetValue("name", "cannon");
						cannon_part->SetValue("class", "AnimatedJoint");
						cannon_part->SetValue("component", "turret");
						cannon_part->SetValue("mass", 0.0f);
						SmartScriptTable cannon_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable cannon01_part(gEnv->pScriptSystem);
							cannon01_part->SetValue("name", "cannon01");
							cannon01_part->SetValue("class", "AnimatedJoint");
							cannon01_part->SetValue("component", "turret");
							cannon01_part->SetValue("mass", 0.0f);
							if (m_modName == "GaussCannon")
							{
								cannon01_part->SetValue("useOption", 1);
							}
							else
							{
								cannon01_part->SetValue("useOption", 0);
							}
							cannon_part_parts->PushBack(cannon01_part);
						}
						{
							SmartScriptTable cannon02_part(gEnv->pScriptSystem);
							cannon02_part->SetValue("name", "cannon02");
							cannon02_part->SetValue("class", "AnimatedJoint");
							cannon02_part->SetValue("component", "turret");
							cannon02_part->SetValue("mass", 0.0f);
							if (m_modName == "GaussCannon")
							{
								cannon02_part->SetValue("useOption", 1);
							}
							else
							{
								cannon02_part->SetValue("useOption", 0);
							}
							cannon_part_parts->PushBack(cannon02_part);
						}
						{
							SmartScriptTable cannon03_part(gEnv->pScriptSystem);
							cannon03_part->SetValue("name", "cannon03");
							cannon03_part->SetValue("class", "AnimatedJoint");
							cannon03_part->SetValue("component", "turret");
							cannon03_part->SetValue("mass", 0.0f);
							if (m_modName == "GaussCannon")
							{
								cannon03_part->SetValue("useOption", 1);
							}
							else
							{
								cannon03_part->SetValue("useOption", 0);
							}
							cannon_part_parts->PushBack(cannon03_part);
						}
						{
							SmartScriptTable cannon04_part(gEnv->pScriptSystem);
							cannon04_part->SetValue("name", "cannon04");
							cannon04_part->SetValue("class", "AnimatedJoint");
							cannon04_part->SetValue("component", "turret");
							cannon04_part->SetValue("mass", 0.0f);
							if (m_modName == "GaussCannon")
							{
								cannon04_part->SetValue("useOption", 1);
							}
							else
							{
								cannon04_part->SetValue("useOption", 0);
							}
							cannon_part_parts->PushBack(cannon04_part);
						}
						cannon_part->SetValue("Parts", cannon_part_parts);
						SmartScriptTable cannon_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable cameraPos_helper(gEnv->pScriptSystem);
							cameraPos_helper->SetValue("name", "cameraPos");
							cameraPos_helper->SetValue("position", Vec3(-0.0076000001, 0.8, 3.2));
							cameraPos_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cameraPos_helper);
						}
						{
							SmartScriptTable ghostViewPos_helper(gEnv->pScriptSystem);
							ghostViewPos_helper->SetValue("name", "ghostViewPos");
							ghostViewPos_helper->SetValue("position", Vec3(-0.0076000001, 1.3862, 3.4537001));
							ghostViewPos_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(ghostViewPos_helper);
						}
						{
							SmartScriptTable cannonOut1_helper(gEnv->pScriptSystem);
							cannonOut1_helper->SetValue("name", "cannonOut1");
							cannonOut1_helper->SetValue("position", Vec3(-1, 2.5918, 2.7567999));
							cannonOut1_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOut1_helper);
						}
						{
							SmartScriptTable cannonOut2_helper(gEnv->pScriptSystem);
							cannonOut2_helper->SetValue("name", "cannonOut2");
							cannonOut2_helper->SetValue("position", Vec3(-1, 2.5901, 2.441));
							cannonOut2_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOut2_helper);
						}
						{
							SmartScriptTable cannonOut3_helper(gEnv->pScriptSystem);
							cannonOut3_helper->SetValue("name", "cannonOut3");
							cannonOut3_helper->SetValue("position", Vec3(1, 2.5940001, 2.7465999));
							cannonOut3_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOut3_helper);
						}
						{
							SmartScriptTable cannonOut4_helper(gEnv->pScriptSystem);
							cannonOut4_helper->SetValue("name", "cannonOut4");
							cannonOut4_helper->SetValue("position", Vec3(1, 2.5883999, 2.4431));
							cannonOut4_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOut4_helper);
						}
						{
							SmartScriptTable rocketOut1_helper(gEnv->pScriptSystem);
							rocketOut1_helper->SetValue("name", "rocketOut1");
							rocketOut1_helper->SetValue("position", Vec3(-1.4226, 0.98610002, 2.7565999));
							rocketOut1_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(rocketOut1_helper);
						}
						{
							SmartScriptTable rocketOut2_helper(gEnv->pScriptSystem);
							rocketOut2_helper->SetValue("name", "rocketOut2");
							rocketOut2_helper->SetValue("position", Vec3(1.4182, 1.0305001, 2.7513001));
							rocketOut2_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(rocketOut2_helper);
						}
						cannon_part->SetValue("Helpers", cannon_part_helpers);
						SmartScriptTable cannon_part_animatedjoint(gEnv->pScriptSystem);
						SmartScriptTable cannon_part_animatedjoint_rotation(gEnv->pScriptSystem);
						cannon_part_animatedjoint_rotation->SetValue("pitchSpeed", 34.0f);
						cannon_part_animatedjoint_rotation->SetValue("pitchAccel", 140.0f);
						SmartScriptTable cannon_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
						cannon_part_animatedjoint_rotation_pitchlimits->PushBack(0.0f);
						cannon_part_animatedjoint_rotation_pitchlimits->PushBack(85.0f);
						cannon_part_animatedjoint_rotation->SetValue("pitchLimits", cannon_part_animatedjoint_rotation_pitchlimits);
						SmartScriptTable cannon_part_animatedjoint_rotation_sound(gEnv->pScriptSystem);
						cannon_part_animatedjoint_rotation_sound->SetValue("event", "cannon");
						cannon_part_animatedjoint_rotation_sound->SetValue("helper", "turret_pos");
						cannon_part_animatedjoint_rotation->SetValue("Sound", cannon_part_animatedjoint_rotation_sound);
						cannon_part_animatedjoint->SetValue("Rotation", cannon_part_animatedjoint_rotation);
						cannon_part->SetValue("AnimatedJoint", cannon_part_animatedjoint);
						turret_part_parts->PushBack(cannon_part);
					}
					turret_part->SetValue("Parts", turret_part_parts);
					SmartScriptTable turret_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable turret_pos_helper(gEnv->pScriptSystem);
						turret_pos_helper->SetValue("name", "turret_pos");
						turret_pos_helper->SetValue("position", Vec3(-0.0063, -0.50660002, 1.9087));
						turret_pos_helper->SetValue("direction", Vec3(0, 1, 0));
						turret_part_helpers->PushBack(turret_pos_helper);
					}
					turret_part->SetValue("Helpers", turret_part_helpers);
					SmartScriptTable turret_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable turret_part_animatedjoint_rotation(gEnv->pScriptSystem);
					turret_part_animatedjoint_rotation->SetValue("yawSpeed", 38.0f);
					turret_part_animatedjoint_rotation->SetValue("yawAccel", 140.0f);
					SmartScriptTable turret_part_animatedjoint_rotation_sound(gEnv->pScriptSystem);
					turret_part_animatedjoint_rotation_sound->SetValue("event", "turret");
					turret_part_animatedjoint_rotation_sound->SetValue("eventDamage", "sounds/vehicles:vehicle_accessories:turret_turn_damaged");
					turret_part_animatedjoint_rotation_sound->SetValue("helper", "turret_pos");
					turret_part_animatedjoint_rotation->SetValue("Sound", turret_part_animatedjoint_rotation_sound);
					turret_part_animatedjoint->SetValue("Rotation", turret_part_animatedjoint_rotation);
					turret_part->SetValue("AnimatedJoint", turret_part_animatedjoint);
					hull_part_parts->PushBack(turret_part);
				}
				hull_part->SetValue("Parts", hull_part_parts);
				SmartScriptTable hull_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable hullDamage_helper(gEnv->pScriptSystem);
					hullDamage_helper->SetValue("name", "hullDamage");
					hullDamage_helper->SetValue("position", Vec3(0, 0.031300001, 1.5518));
					hullDamage_helper->SetValue("direction", Vec3(0, 1, 0));
					hull_part_helpers->PushBack(hullDamage_helper);
				}
				hull_part->SetValue("Helpers", hull_part_helpers);
				SmartScriptTable hull_part_animatedjoint(gEnv->pScriptSystem);
				hull_part->SetValue("AnimatedJoint", hull_part_animatedjoint);
				body_part_parts->PushBack(hull_part);
			}
			{
				SmartScriptTable wheel1_part(gEnv->pScriptSystem);
				wheel1_part->SetValue("name", "wheel1");
				wheel1_part->SetValue("class", "SubPartWheel");
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
				body_part_parts->PushBack(wheel1_part);
			}
			{
				SmartScriptTable wheel2_part(gEnv->pScriptSystem);
				wheel2_part->SetValue("name", "wheel2");
				wheel2_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel2_part_subpartwheel(gEnv->pScriptSystem);
				wheel2_part_subpartwheel->SetValue("axle", 1);
				wheel2_part_subpartwheel->SetValue("density", 3000.0f);
				wheel2_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel2_part_subpartwheel->SetValue("driving", false);
				wheel2_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel2_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel2_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel2_part_subpartwheel->SetValue("rayCast", false);
				wheel2_part->SetValue("SubPartWheel", wheel2_part_subpartwheel);
				body_part_parts->PushBack(wheel2_part);
			}
			{
				SmartScriptTable wheel3_part(gEnv->pScriptSystem);
				wheel3_part->SetValue("name", "wheel3");
				wheel3_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel3_part_subpartwheel(gEnv->pScriptSystem);
				wheel3_part_subpartwheel->SetValue("axle", 2);
				wheel3_part_subpartwheel->SetValue("density", 3000.0f);
				wheel3_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel3_part_subpartwheel->SetValue("driving", false);
				wheel3_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel3_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel3_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel3_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel3_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel3_part_subpartwheel->SetValue("rayCast", true);
				wheel3_part->SetValue("SubPartWheel", wheel3_part_subpartwheel);
				body_part_parts->PushBack(wheel3_part);
			}
			{
				SmartScriptTable wheel4_part(gEnv->pScriptSystem);
				wheel4_part->SetValue("name", "wheel4");
				wheel4_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel4_part_subpartwheel(gEnv->pScriptSystem);
				wheel4_part_subpartwheel->SetValue("axle", 3);
				wheel4_part_subpartwheel->SetValue("density", 3000.0f);
				wheel4_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel4_part_subpartwheel->SetValue("driving", false);
				wheel4_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel4_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel4_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel4_part_subpartwheel->SetValue("rayCast", true);
				wheel4_part->SetValue("SubPartWheel", wheel4_part_subpartwheel);
				body_part_parts->PushBack(wheel4_part);
			}
			{
				SmartScriptTable wheel5_part(gEnv->pScriptSystem);
				wheel5_part->SetValue("name", "wheel5");
				wheel5_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel5_part_subpartwheel(gEnv->pScriptSystem);
				wheel5_part_subpartwheel->SetValue("axle", 4);
				wheel5_part_subpartwheel->SetValue("density", 3000.0f);
				wheel5_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel5_part_subpartwheel->SetValue("driving", false);
				wheel5_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel5_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel5_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel5_part_subpartwheel->SetValue("rayCast", true);
				wheel5_part->SetValue("SubPartWheel", wheel5_part_subpartwheel);
				body_part_parts->PushBack(wheel5_part);
			}
			{
				SmartScriptTable wheel6_part(gEnv->pScriptSystem);
				wheel6_part->SetValue("name", "wheel6");
				wheel6_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel6_part_subpartwheel(gEnv->pScriptSystem);
				wheel6_part_subpartwheel->SetValue("axle", 5);
				wheel6_part_subpartwheel->SetValue("density", 3000.0f);
				wheel6_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel6_part_subpartwheel->SetValue("driving", false);
				wheel6_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel6_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel6_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel6_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel6_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel6_part_subpartwheel->SetValue("rayCast", true);
				wheel6_part->SetValue("SubPartWheel", wheel6_part_subpartwheel);
				body_part_parts->PushBack(wheel6_part);
			}
			{
				SmartScriptTable wheel7_part(gEnv->pScriptSystem);
				wheel7_part->SetValue("name", "wheel7");
				wheel7_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel7_part_subpartwheel(gEnv->pScriptSystem);
				wheel7_part_subpartwheel->SetValue("axle", 6);
				wheel7_part_subpartwheel->SetValue("density", 3000.0f);
				wheel7_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel7_part_subpartwheel->SetValue("driving", false);
				wheel7_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel7_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel7_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel7_part_subpartwheel->SetValue("rayCast", false);
				wheel7_part->SetValue("SubPartWheel", wheel7_part_subpartwheel);
				body_part_parts->PushBack(wheel7_part);
			}
			{
				SmartScriptTable wheel8_part(gEnv->pScriptSystem);
				wheel8_part->SetValue("name", "wheel8");
				wheel8_part->SetValue("class", "SubPartWheel");
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
				body_part_parts->PushBack(wheel8_part);
			}
			{
				SmartScriptTable wheel9_part(gEnv->pScriptSystem);
				wheel9_part->SetValue("name", "wheel9");
				wheel9_part->SetValue("class", "SubPartWheel");
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
				body_part_parts->PushBack(wheel9_part);
			}
			{
				SmartScriptTable wheel10_part(gEnv->pScriptSystem);
				wheel10_part->SetValue("name", "wheel10");
				wheel10_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel10_part_subpartwheel(gEnv->pScriptSystem);
				wheel10_part_subpartwheel->SetValue("axle", 1);
				wheel10_part_subpartwheel->SetValue("density", 3000.0f);
				wheel10_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel10_part_subpartwheel->SetValue("driving", false);
				wheel10_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel10_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel10_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel10_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel10_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel10_part_subpartwheel->SetValue("rayCast", false);
				wheel10_part->SetValue("SubPartWheel", wheel10_part_subpartwheel);
				body_part_parts->PushBack(wheel10_part);
			}
			{
				SmartScriptTable wheel11_part(gEnv->pScriptSystem);
				wheel11_part->SetValue("name", "wheel11");
				wheel11_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel11_part_subpartwheel(gEnv->pScriptSystem);
				wheel11_part_subpartwheel->SetValue("axle", 2);
				wheel11_part_subpartwheel->SetValue("density", 3000.0f);
				wheel11_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel11_part_subpartwheel->SetValue("driving", false);
				wheel11_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel11_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel11_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel11_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel11_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel11_part_subpartwheel->SetValue("rayCast", true);
				wheel11_part->SetValue("SubPartWheel", wheel11_part_subpartwheel);
				body_part_parts->PushBack(wheel11_part);
			}
			{
				SmartScriptTable wheel12_part(gEnv->pScriptSystem);
				wheel12_part->SetValue("name", "wheel12");
				wheel12_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel12_part_subpartwheel(gEnv->pScriptSystem);
				wheel12_part_subpartwheel->SetValue("axle", 3);
				wheel12_part_subpartwheel->SetValue("density", 3000.0f);
				wheel12_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel12_part_subpartwheel->SetValue("driving", false);
				wheel12_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel12_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel12_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel12_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel12_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel12_part_subpartwheel->SetValue("rayCast", true);
				wheel12_part->SetValue("SubPartWheel", wheel12_part_subpartwheel);
				body_part_parts->PushBack(wheel12_part);
			}
			{
				SmartScriptTable wheel13_part(gEnv->pScriptSystem);
				wheel13_part->SetValue("name", "wheel13");
				wheel13_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel13_part_subpartwheel(gEnv->pScriptSystem);
				wheel13_part_subpartwheel->SetValue("axle", 4);
				wheel13_part_subpartwheel->SetValue("density", 3000.0f);
				wheel13_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel13_part_subpartwheel->SetValue("driving", false);
				wheel13_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel13_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel13_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel13_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel13_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel13_part_subpartwheel->SetValue("rayCast", true);
				wheel13_part->SetValue("SubPartWheel", wheel13_part_subpartwheel);
				body_part_parts->PushBack(wheel13_part);
			}
			{
				SmartScriptTable wheel14_part(gEnv->pScriptSystem);
				wheel14_part->SetValue("name", "wheel14");
				wheel14_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel14_part_subpartwheel(gEnv->pScriptSystem);
				wheel14_part_subpartwheel->SetValue("axle", 5);
				wheel14_part_subpartwheel->SetValue("density", 3000.0f);
				wheel14_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel14_part_subpartwheel->SetValue("driving", false);
				wheel14_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel14_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel14_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel14_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel14_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel14_part_subpartwheel->SetValue("rayCast", true);
				wheel14_part->SetValue("SubPartWheel", wheel14_part_subpartwheel);
				body_part_parts->PushBack(wheel14_part);
			}
			{
				SmartScriptTable wheel15_part(gEnv->pScriptSystem);
				wheel15_part->SetValue("name", "wheel15");
				wheel15_part->SetValue("class", "SubPartWheel");
				SmartScriptTable wheel15_part_subpartwheel(gEnv->pScriptSystem);
				wheel15_part_subpartwheel->SetValue("axle", 6);
				wheel15_part_subpartwheel->SetValue("density", 3000.0f);
				wheel15_part_subpartwheel->SetValue("damping", -0.41999999f);
				wheel15_part_subpartwheel->SetValue("driving", false);
				wheel15_part_subpartwheel->SetValue("lenMax", 0.47f);
				wheel15_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel15_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel15_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel15_part_subpartwheel->SetValue("suspLength", 0.33000001f);
				wheel15_part_subpartwheel->SetValue("rayCast", false);
				wheel15_part->SetValue("SubPartWheel", wheel15_part_subpartwheel);
				body_part_parts->PushBack(wheel15_part);
			}
			{
				SmartScriptTable wheel16_part(gEnv->pScriptSystem);
				wheel16_part->SetValue("name", "wheel16");
				wheel16_part->SetValue("class", "SubPartWheel");
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
				body_part_parts->PushBack(wheel16_part);
			}
			body_part->SetValue("Parts", body_part_parts);
			SmartScriptTable body_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable enterPos_helper(gEnv->pScriptSystem);
				enterPos_helper->SetValue("name", "enterPos");
				enterPos_helper->SetValue("position", Vec3(2.9131, 1.2522, 0.68879998));
				enterPos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(enterPos_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, 0, 0.039299998));
				burningPos_helper->SetValue("direction", Vec3(0, 0, 1));
				body_part_helpers->PushBack(burningPos_helper);
			}
			{
				SmartScriptTable engineSoundPos_helper(gEnv->pScriptSystem);
				engineSoundPos_helper->SetValue("name", "engineSoundPos");
				engineSoundPos_helper->SetValue("position", Vec3(0.022, 2.8640001, 1.4286));
				engineSoundPos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(engineSoundPos_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(1.4214, 3.1066999, 1.4797));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(-1.4170001, 3.1543, 1.4883));
				headLightRight_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightRight_helper);
			}
			{
				SmartScriptTable headLightCenter_helper(gEnv->pScriptSystem);
				headLightCenter_helper->SetValue("name", "headLightCenter");
				headLightCenter_helper->SetValue("position", Vec3(0.0178, 3.1565001, 1.476));
				headLightCenter_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightCenter_helper);
			}
			{
				SmartScriptTable rearLightLeft_helper(gEnv->pScriptSystem);
				rearLightLeft_helper->SetValue("name", "rearLightLeft");
				rearLightLeft_helper->SetValue("position", Vec3(-1.25, -3.6020999, 1.6813999));
				rearLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightLeft_helper);
			}
			{
				SmartScriptTable rearLightRight_helper(gEnv->pScriptSystem);
				rearLightRight_helper->SetValue("name", "rearLightRight");
				rearLightRight_helper->SetValue("position", Vec3(1.2695, -3.6145, 1.6847));
				rearLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightRight_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(0, -0.3845, 1.3942));
				centerPos_helper->SetValue("direction", Vec3(0, 0, 1));
				body_part_helpers->PushBack(centerPos_helper);
			}
			{
				SmartScriptTable exhaust_helper(gEnv->pScriptSystem);
				exhaust_helper->SetValue("name", "exhaust");
				exhaust_helper->SetValue("position", Vec3(1.0125, -3.6863, 1.1955));
				exhaust_helper->SetValue("direction", Vec3(-0.0118, -0.3127, -0.94980001));
				body_part_helpers->PushBack(exhaust_helper);
			}
			{
				SmartScriptTable engineDamage_helper(gEnv->pScriptSystem);
				engineDamage_helper->SetValue("name", "engineDamage");
				engineDamage_helper->SetValue("position", Vec3(0, 0, 4.4394999));
				engineDamage_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(engineDamage_helper);
			}
			body_part->SetValue("Helpers", body_part_helpers);
			SmartScriptTable body_part_animated(gEnv->pScriptSystem);
			body_part_animated->SetValue("filename", "Objects/Vehicles/Asian_AAA/asian_aaa.cga");
			body_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/Asian_AAA/asian_aaa_damaged.cga");
			body_part->SetValue("Animated", body_part_animated);
			parts->PushBack(body_part);
		}
		{
			SmartScriptTable massBox_part(gEnv->pScriptSystem);
			massBox_part->SetValue("name", "massBox");
			massBox_part->SetValue("class", "MassBox");
			massBox_part->SetValue("component", "");
			massBox_part->SetValue("mass", 25000.0f);
			massBox_part->SetValue("position", Vec3(-0.0017, -0.5557, 0.8344));
			massBox_part->SetValue("disablePhysics", false);
			massBox_part->SetValue("disableCollision", false);
			massBox_part->SetValue("isHidden", false);
			SmartScriptTable massBox_part_helpers(gEnv->pScriptSystem);
			massBox_part->SetValue("Helpers", massBox_part_helpers);
			SmartScriptTable massBox_part_massbox(gEnv->pScriptSystem);
			massBox_part_massbox->SetValue("size", Vec3(1.6464, 2.5882001, 0.75700003));
			massBox_part->SetValue("MassBox", massBox_part_massbox);
			parts->PushBack(massBox_part);
		}
		{
			SmartScriptTable leftTread_part(gEnv->pScriptSystem);
			leftTread_part->SetValue("name", "leftTread");
			leftTread_part->SetValue("class", "Tread");
			leftTread_part->SetValue("component", "leftTread");
			SmartScriptTable leftTread_part_tread(gEnv->pScriptSystem);
			leftTread_part_tread->SetValue("filename", "Objects/Vehicles/asian_aaa/tread_left.chr");
			leftTread_part_tread->SetValue("materialName", "tread");
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
			rightTread_part_tread->SetValue("filename", "Objects/Vehicles/asian_aaa/tread_right.chr");
			rightTread_part_tread->SetValue("materialName", "tread");
			rightTread_part_tread->SetValue("uvRateMax", 23.0f);
			rightTread_part_tread->SetValue("uvSliceCount", 12.0f);
			rightTread_part->SetValue("Tread", rightTread_part_tread);
			parts->PushBack(rightTread_part);
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
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 650.0f);
		}
		else
		{
			component->SetValue("damageMax", 500.0f);
		}
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-2, -4, 0));
		component->SetValue("maxBound", Vec3(2, 3.5, 4));
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
				damagemultiplier->SetValue("multiplier", 2.0f);
			}
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
		component->SetValue("name", "turret");
		component->SetValue("damageMax", 300.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-2, -2.5, 2));
		component->SetValue("maxBound", Vec3(2, 2, 4));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.3f);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* turret = new VehicleComponent();
		turret->Init(this, component);
		m_components.push_back(turret);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "radar");
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);

		VehicleComponent* radar = new VehicleComponent();
		radar->Init(this, component);
		m_components.push_back(radar);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "back");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.71310002, -3.4939001, 0.6401));
		component->SetValue("maxBound", Vec3(0.84299999, -2.8292999, 1.6917));
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
			damagemultiplier->SetValue("splash", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.25f);
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

		VehicleComponent* back = new VehicleComponent();
		back->Init(this, component);
		m_components.push_back(back);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "leftTread");
		component->SetValue("damageMax", 200.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.63, -3.7, -0.2166));
		component->SetValue("maxBound", Vec3(-1.0854, 3.2, 1.2));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.75f);
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
		component->SetValue("damageMax", 200.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(1.08, -3.7, -0.2191));
		component->SetValue("maxBound", Vec3(1.63, 3.2, 1.2));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.75f);
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
			damagebehavior_burn->SetValue("selfDamage", 50.0f);
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
		particles_exhaust_helpers->PushBack("exhaust");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "vehicle_fx.vehicle_exhaust.tank_exhaust");
		particles_exhaust_enginerunning->SetValue("boostEffect", "vehicle_fx.vehicle_exhaust.heavy_boost");
		particles_exhaust_enginerunning->SetValue("baseSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 0.80000001f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeed", 15.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedSizeScale", 1.3f);
		particles_exhaust_enginerunning->SetValue("maxSpeedCountScale", 1.3f);
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
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "hullDamage");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.NKaaa_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "hullDamage");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.NKaaa_hull_smoke_b");
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
		animation->SetValue("name", "radarOn");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable on_state(gEnv->pScriptSystem);
			on_state->SetValue("name", "on");
			on_state->SetValue("animation", "AAA_radar_rotate");
			on_state->SetValue("isLooped", true);
			on_state->SetValue("speedDefault", 1.0f);
			on_state->SetValue("speedMin", 1.0f);
			on_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(on_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* radarOn = new VehicleAnimation();
		radarOn->Init(this, animation);
		m_animations.push_back({ "radarOn", radarOn });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "cannon1_fire");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			shoot_state->SetValue("animation", "AAA_cannon01_firing");
			shoot_state->SetValue("isLooped", false);
			shoot_state->SetValue("speedDefault", 1.0f);
			shoot_state->SetValue("speedMin", 1.0f);
			shoot_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(shoot_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* cannon1_fire = new VehicleAnimation();
		cannon1_fire->Init(this, animation);
		m_animations.push_back({ "cannon1_fire", cannon1_fire });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "cannon2_fire");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			shoot_state->SetValue("animation", "AAA_cannon02_firing");
			shoot_state->SetValue("isLooped", false);
			shoot_state->SetValue("speedDefault", 1.0f);
			shoot_state->SetValue("speedMin", 1.0f);
			shoot_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(shoot_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* cannon2_fire = new VehicleAnimation();
		cannon2_fire->Init(this, animation);
		m_animations.push_back({ "cannon2_fire", cannon2_fire });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "cannon3_fire");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			shoot_state->SetValue("animation", "AAA_cannon03_firing");
			shoot_state->SetValue("isLooped", false);
			shoot_state->SetValue("speedDefault", 1.0f);
			shoot_state->SetValue("speedMin", 1.0f);
			shoot_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(shoot_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* cannon3_fire = new VehicleAnimation();
		cannon3_fire->Init(this, animation);
		m_animations.push_back({ "cannon3_fire", cannon3_fire });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "cannon4_fire");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			shoot_state->SetValue("animation", "AAA_cannon04_firing");
			shoot_state->SetValue("isLooped", false);
			shoot_state->SetValue("speedDefault", 1.0f);
			shoot_state->SetValue("speedMin", 1.0f);
			shoot_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(shoot_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* cannon4_fire = new VehicleAnimation();
		cannon4_fire->Init(this, animation);
		m_animations.push_back({ "cannon4_fire", cannon4_fire });
	}

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("pedalSpeed", 5.0f);
		movement->SetValue("pedalThreshold", 0.0f);
		movement->SetValue("steerSpeed", 10.0f);
		movement->SetValue("steerSpeedRelax", 7.0f);
		movement->SetValue("latFricMin", 0.40000001f);
		movement->SetValue("latFricMinSteer", 0.1f);
		movement->SetValue("latFricMax", 1.3f);
		movement->SetValue("latSlipMin", 1.1f);
		movement->SetValue("latSlipMax", 2.4000001f);
		movement->SetValue("rpmInterpSpeed", 2.0f);
		movement->SetValue("rpmRelaxSpeed", 2.0f);
		movement->SetValue("rpmGearShiftSpeed", 2.0f);
		movement->SetValue("steeringImpulseMin", 1.0f);
		movement->SetValue("steeringImpulseMax", 2.0f);
		movement->SetValue("steeringImpulseRelaxMin", 4.0f);
		movement->SetValue("steeringImpulseRelaxMax", 6.0f);
		SmartScriptTable movement_wheeled(gEnv->pScriptSystem);
		movement_wheeled->SetValue("axleFriction", 650);
		movement_wheeled->SetValue("axleFrictionMax", 4000);
		movement_wheeled->SetValue("brakeTorque", 10000);
		movement_wheeled->SetValue("clutchSpeed", 8.0f);
		movement_wheeled->SetValue("damping", 0.02f);
		movement_wheeled->SetValue("engineIdleRPM", 500);
		movement_wheeled->SetValue("engineMaxRPM", 2400);
		movement_wheeled->SetValue("engineMinRPM", 100);
		movement_wheeled->SetValue("enginePower", 750);
		movement_wheeled->SetValue("engineShiftDownRPM", 1000);
		movement_wheeled->SetValue("engineShiftUpRPM", 1900);
		movement_wheeled->SetValue("engineStartRPM", 500);
		movement_wheeled->SetValue("stabilizer", 0.25f);
		movement_wheeled->SetValue("stabiMin", 0.0f);
		movement_wheeled->SetValue("stabiMax", 0.25f);
		movement_wheeled->SetValue("maxSteer", 0.78500003f);
		movement_wheeled->SetValue("maxSpeed", 12.0f);
		movement_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_wheeled->SetValue("minEnergy", 0.01f);
		movement_wheeled->SetValue("slipThreshold", 0.050000001f);
		movement_wheeled->SetValue("gearDirSwitchRPM", 2500);
		movement_wheeled->SetValue("dynFriction", 1.0f);
		movement_wheeled->SetValue("steerTrackNeutralTurn", 0.78500003f);
		movement_wheeled->SetValue("pullTilt", 5.0f);
		SmartScriptTable movement_wheeled_gearratios(gEnv->pScriptSystem);
		movement_wheeled_gearratios->PushBack(-15.0f);
		movement_wheeled_gearratios->PushBack(0.0f);
		movement_wheeled_gearratios->PushBack(10.0f);
		movement_wheeled_gearratios->PushBack(8.0f);
		movement_wheeled_gearratios->PushBack(7.0f);
		movement_wheeled->SetValue("gearRatios", movement_wheeled_gearratios);
		movement->SetValue("Wheeled", movement_wheeled);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSoundPos");
		movement_soundparams->SetValue("runSoundDelay", 0.80000001f);
		movement_soundparams->SetValue("rpmPitchSpeed", 0.0f);
		movement_soundparams->SetValue("roadBumpMinSusp", 8.0f);
		movement_soundparams->SetValue("roadBumpMinSpeed", 8.0f);
		movement_soundparams->SetValue("roadBumpIntensity", 0.25f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable pullTilt_tweak(gEnv->pScriptSystem);
				pullTilt_tweak->SetValue("name", "pullTilt");
				pullTilt_tweak->SetValue("value", 1.5f);
				pullTilt_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(pullTilt_tweak);
			}
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);
		SmartScriptTable movement_animations(gEnv->pScriptSystem);
		movement_animations->SetValue("engine", "radarOn");
		movement->SetValue("Animations", movement_animations);

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
		seat->SetValue("enterHelper", "enterPos");
		seat->SetValue("sitHelper", "turret_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("isPassengerHidden", true);
		seat->SetValue("AimPart", "cannon");
		seat->SetValue("explosionShakeMult", 1.0f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "cameraPos");
			view_firstperson->SetValue("frameObject", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -2.5, 0.5));
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
			SmartScriptTable seataction_rotateturret(gEnv->pScriptSystem);
			seataction_rotateturret->SetValue("pitchPart", "cannon");
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
					weapon->SetValue("class", "GaussAAA");
				}
				else
				{
					weapon->SetValue("class", "AACannon");
				}
				weapon->SetValue("part", "cannon");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("cannonOut1");
				weapon_helpers->PushBack("cannonOut3");
				weapon_helpers->PushBack("cannonOut2");
				weapon_helpers->PushBack("cannonOut4");
				weapon->SetValue("Helpers", weapon_helpers);
				SmartScriptTable weapon_actions(gEnv->pScriptSystem);
				{
					SmartScriptTable fire_action(gEnv->pScriptSystem);
					fire_action->SetValue("name", "fire");
					SmartScriptTable fire_action_animations(gEnv->pScriptSystem);
					{
						SmartScriptTable cannon1_fire_animation(gEnv->pScriptSystem);
						cannon1_fire_animation->SetValue("name", "cannon1_fire");
						fire_action_animations->PushBack(cannon1_fire_animation);
					}
					{
						SmartScriptTable cannon3_fire_animation(gEnv->pScriptSystem);
						cannon3_fire_animation->SetValue("name", "cannon3_fire");
						fire_action_animations->PushBack(cannon3_fire_animation);
					}
					{
						SmartScriptTable cannon2_fire_animation(gEnv->pScriptSystem);
						cannon2_fire_animation->SetValue("name", "cannon2_fire");
						fire_action_animations->PushBack(cannon2_fire_animation);
					}
					{
						SmartScriptTable cannon4_fire_animation(gEnv->pScriptSystem);
						cannon4_fire_animation->SetValue("name", "cannon4_fire");
						fire_action_animations->PushBack(cannon4_fire_animation);
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
				if (m_modName == "SAM")
				{
					weapon->SetValue("class", "SideWinder");
				}
				else
				{
					weapon->SetValue("class", "AARocketLauncher");
				}
				weapon->SetValue("part", "cannon");
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

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetValue("Seats", seats);
	}

	// SeatTransitions
	this->InitSeatTransitions();

	// Inventory
	this->SetAmmoCapacity("tankaa", 1000);
	this->SetAmmoCapacity("dumbaamissile", 8);
	this->SetAmmoCapacity("a2ahomingmissile", 8);

	// Paints
	if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/asian_aaa/asian_aaa.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/asian_aaa/asian_aaa_us.mtl");
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("submergedRatioMax", 0.89999998f);
		damages->SetValue("submergedDamageMult", 0.5f);
		damages->SetValue("collDamageThreshold", 450.0f);
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.3f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		damages->SetValue("DamageMultipliers", damages_damagemultipliers);
		SmartScriptTable damages_damagesgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable Destroy_damagesgroup(gEnv->pScriptSystem);
			Destroy_damagesgroup->SetValue("name", "Destroy");
			Destroy_damagesgroup->SetValue("useTemplate", "TankDestroy");
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
