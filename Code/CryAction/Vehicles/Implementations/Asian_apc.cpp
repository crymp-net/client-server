#include "Asian_apc.h"

Asian_apc::Asian_apc()
{
	// enable Asian_apc::Init function
	this->Vehicle::PatchVTable();
}

Asian_apc::~Asian_apc()
{
}

bool Asian_apc::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 650.0f;
	m_buoyancy.waterResistance = 2800.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.0020000001f;
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
							cannonViewPos_helper->SetValue("position", Vec3(-0.34889999, 0.68040001, 2.8571));
							cannonViewPos_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonViewPos_helper);
						}
						{
							SmartScriptTable cannonOut_helper(gEnv->pScriptSystem);
							cannonOut_helper->SetValue("name", "cannonOut");
							cannonOut_helper->SetValue("position", Vec3(0, 2.3979001, 2.756));
							cannonOut_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOut_helper);
						}
						{
							SmartScriptTable cannonOutLong_helper(gEnv->pScriptSystem);
							cannonOutLong_helper->SetValue("name", "cannonOutLong");
							cannonOutLong_helper->SetValue("position", Vec3(0, 4.6374998, 2.5114999));
							cannonOutLong_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOutLong_helper);
						}
						{
							SmartScriptTable rocketOut1_helper(gEnv->pScriptSystem);
							rocketOut1_helper->SetValue("name", "rocketOut1");
							rocketOut1_helper->SetValue("position", Vec3(1.0579, -0.0405, 3.1084001));
							rocketOut1_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(rocketOut1_helper);
						}
						{
							SmartScriptTable rocketOut2_helper(gEnv->pScriptSystem);
							rocketOut2_helper->SetValue("name", "rocketOut2");
							rocketOut2_helper->SetValue("position", Vec3(1.0585999, -0.052499998, 3.369));
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
					driverSitPos_helper->SetValue("position", Vec3(0, 0.66649997, 0.95410001));
					driverSitPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(driverSitPos_helper);
				}
				{
					SmartScriptTable turretPos_helper(gEnv->pScriptSystem);
					turretPos_helper->SetValue("name", "turretPos");
					turretPos_helper->SetValue("position", Vec3(0.00069999998, -0.1104, 2.0906999));
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
				wheel1_part->SetValue("component", "wheel1");
				wheel1_part->SetValue("mass", 80.0f);
				wheel1_part->SetValue("isHidden", false);
				SmartScriptTable wheel1_part_subpartwheel(gEnv->pScriptSystem);
				wheel1_part_subpartwheel->SetValue("axle", 0);
				wheel1_part_subpartwheel->SetValue("density", 0.0f);
				wheel1_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel1_part_subpartwheel->SetValue("driving", true);
				wheel1_part_subpartwheel->SetValue("lenMax", 0.55000001f);
				wheel1_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel1_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel1_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel1_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel1_part_subpartwheel->SetValue("rayCast", false);
				wheel1_part->SetValue("SubPartWheel", wheel1_part_subpartwheel);
				tank_part_parts->PushBack(wheel1_part);
			}
			{
				SmartScriptTable wheel2_part(gEnv->pScriptSystem);
				wheel2_part->SetValue("name", "wheel2");
				wheel2_part->SetValue("class", "SubPartWheel");
				wheel2_part->SetValue("component", "wheel2");
				wheel2_part->SetValue("mass", 80.0f);
				wheel2_part->SetValue("isHidden", false);
				SmartScriptTable wheel2_part_subpartwheel(gEnv->pScriptSystem);
				wheel2_part_subpartwheel->SetValue("axle", 1);
				wheel2_part_subpartwheel->SetValue("density", 0.0f);
				wheel2_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel2_part_subpartwheel->SetValue("driving", true);
				wheel2_part_subpartwheel->SetValue("lenMax", 0.55000001f);
				wheel2_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel2_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel2_part_subpartwheel->SetValue("rayCast", true);
				wheel2_part->SetValue("SubPartWheel", wheel2_part_subpartwheel);
				tank_part_parts->PushBack(wheel2_part);
			}
			{
				SmartScriptTable wheel3_part(gEnv->pScriptSystem);
				wheel3_part->SetValue("name", "wheel3");
				wheel3_part->SetValue("class", "SubPartWheel");
				wheel3_part->SetValue("component", "wheel3");
				wheel3_part->SetValue("mass", 80.0f);
				wheel3_part->SetValue("isHidden", false);
				SmartScriptTable wheel3_part_subpartwheel(gEnv->pScriptSystem);
				wheel3_part_subpartwheel->SetValue("axle", 2);
				wheel3_part_subpartwheel->SetValue("density", 0.0f);
				wheel3_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel3_part_subpartwheel->SetValue("driving", true);
				wheel3_part_subpartwheel->SetValue("lenMax", 0.55000001f);
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
				wheel4_part->SetValue("component", "wheel4");
				wheel4_part->SetValue("mass", 80.0f);
				wheel4_part->SetValue("isHidden", false);
				SmartScriptTable wheel4_part_subpartwheel(gEnv->pScriptSystem);
				wheel4_part_subpartwheel->SetValue("axle", 3);
				wheel4_part_subpartwheel->SetValue("density", 0.0f);
				wheel4_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel4_part_subpartwheel->SetValue("driving", true);
				wheel4_part_subpartwheel->SetValue("lenMax", 0.55000001f);
				wheel4_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel4_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel4_part_subpartwheel->SetValue("rayCast", false);
				wheel4_part->SetValue("SubPartWheel", wheel4_part_subpartwheel);
				tank_part_parts->PushBack(wheel4_part);
			}
			{
				SmartScriptTable wheel5_part(gEnv->pScriptSystem);
				wheel5_part->SetValue("name", "wheel5");
				wheel5_part->SetValue("class", "SubPartWheel");
				wheel5_part->SetValue("component", "wheel5");
				wheel5_part->SetValue("mass", 80.0f);
				wheel5_part->SetValue("isHidden", false);
				SmartScriptTable wheel5_part_subpartwheel(gEnv->pScriptSystem);
				wheel5_part_subpartwheel->SetValue("axle", 0);
				wheel5_part_subpartwheel->SetValue("density", 0.0f);
				wheel5_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel5_part_subpartwheel->SetValue("driving", true);
				wheel5_part_subpartwheel->SetValue("lenMax", 0.55000001f);
				wheel5_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel5_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel5_part_subpartwheel->SetValue("rayCast", false);
				wheel5_part->SetValue("SubPartWheel", wheel5_part_subpartwheel);
				tank_part_parts->PushBack(wheel5_part);
			}
			{
				SmartScriptTable wheel6_part(gEnv->pScriptSystem);
				wheel6_part->SetValue("name", "wheel6");
				wheel6_part->SetValue("class", "SubPartWheel");
				wheel6_part->SetValue("component", "wheel6");
				wheel6_part->SetValue("mass", 80.0f);
				wheel6_part->SetValue("isHidden", false);
				SmartScriptTable wheel6_part_subpartwheel(gEnv->pScriptSystem);
				wheel6_part_subpartwheel->SetValue("axle", 1);
				wheel6_part_subpartwheel->SetValue("density", 0.0f);
				wheel6_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel6_part_subpartwheel->SetValue("driving", true);
				wheel6_part_subpartwheel->SetValue("lenMax", 0.55000001f);
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
				wheel7_part->SetValue("component", "wheel7");
				wheel7_part->SetValue("mass", 80.0f);
				wheel7_part->SetValue("isHidden", false);
				SmartScriptTable wheel7_part_subpartwheel(gEnv->pScriptSystem);
				wheel7_part_subpartwheel->SetValue("axle", 2);
				wheel7_part_subpartwheel->SetValue("density", 0.0f);
				wheel7_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel7_part_subpartwheel->SetValue("driving", true);
				wheel7_part_subpartwheel->SetValue("lenMax", 0.55000001f);
				wheel7_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel7_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel7_part_subpartwheel->SetValue("rayCast", true);
				wheel7_part->SetValue("SubPartWheel", wheel7_part_subpartwheel);
				tank_part_parts->PushBack(wheel7_part);
			}
			{
				SmartScriptTable wheel8_part(gEnv->pScriptSystem);
				wheel8_part->SetValue("name", "wheel8");
				wheel8_part->SetValue("class", "SubPartWheel");
				wheel8_part->SetValue("component", "wheel8");
				wheel8_part->SetValue("mass", 80.0f);
				wheel8_part->SetValue("isHidden", false);
				SmartScriptTable wheel8_part_subpartwheel(gEnv->pScriptSystem);
				wheel8_part_subpartwheel->SetValue("axle", 3);
				wheel8_part_subpartwheel->SetValue("density", 0.0f);
				wheel8_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel8_part_subpartwheel->SetValue("driving", true);
				wheel8_part_subpartwheel->SetValue("lenMax", 0.55000001f);
				wheel8_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel8_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel8_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel8_part_subpartwheel->SetValue("suspLength", 0.34999999f);
				wheel8_part_subpartwheel->SetValue("rayCast", false);
				wheel8_part->SetValue("SubPartWheel", wheel8_part_subpartwheel);
				tank_part_parts->PushBack(wheel8_part);
			}
			tank_part->SetValue("Parts", tank_part_parts);
			SmartScriptTable tank_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable driverEnterPos_helper(gEnv->pScriptSystem);
				driverEnterPos_helper->SetValue("name", "driverEnterPos");
				driverEnterPos_helper->SetValue("position", Vec3(2.9897001, 0, 0.50330001));
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
				passenger1_sit_pos_helper->SetValue("position", Vec3(-0.76730001, -1.4531, 1.2194));
				passenger1_sit_pos_helper->SetValue("direction", Vec3(1, 0, 0));
				tank_part_helpers->PushBack(passenger1_sit_pos_helper);
			}
			{
				SmartScriptTable passenger2_sit_pos_helper(gEnv->pScriptSystem);
				passenger2_sit_pos_helper->SetValue("name", "passenger2_sit_pos");
				passenger2_sit_pos_helper->SetValue("position", Vec3(0.75629997, -1.4365, 1.219));
				passenger2_sit_pos_helper->SetValue("direction", Vec3(-1, 0, 0));
				tank_part_helpers->PushBack(passenger2_sit_pos_helper);
			}
			{
				SmartScriptTable passenger3_sit_pos_helper(gEnv->pScriptSystem);
				passenger3_sit_pos_helper->SetValue("name", "passenger3_sit_pos");
				passenger3_sit_pos_helper->SetValue("position", Vec3(-0.74779999, -2.2653999, 1.2175));
				passenger3_sit_pos_helper->SetValue("direction", Vec3(1, 0, 0));
				tank_part_helpers->PushBack(passenger3_sit_pos_helper);
			}
			{
				SmartScriptTable passenger4_sit_pos_helper(gEnv->pScriptSystem);
				passenger4_sit_pos_helper->SetValue("name", "passenger4_sit_pos");
				passenger4_sit_pos_helper->SetValue("position", Vec3(0.76419997, -2.2514999, 1.22));
				passenger4_sit_pos_helper->SetValue("direction", Vec3(-1, 0, 0));
				tank_part_helpers->PushBack(passenger4_sit_pos_helper);
			}
			{
				SmartScriptTable passenger5_sit_pos_helper(gEnv->pScriptSystem);
				passenger5_sit_pos_helper->SetValue("name", "passenger5_sit_pos");
				passenger5_sit_pos_helper->SetValue("position", Vec3(-0.76440001, -3.0741999, 1.2208));
				passenger5_sit_pos_helper->SetValue("direction", Vec3(1, 0, 0));
				tank_part_helpers->PushBack(passenger5_sit_pos_helper);
			}
			{
				SmartScriptTable passenger6_sit_pos_helper(gEnv->pScriptSystem);
				passenger6_sit_pos_helper->SetValue("name", "passenger6_sit_pos");
				passenger6_sit_pos_helper->SetValue("position", Vec3(0.7554, -3.0637, 1.2078));
				passenger6_sit_pos_helper->SetValue("direction", Vec3(-1, 0, 0));
				tank_part_helpers->PushBack(passenger6_sit_pos_helper);
			}
			{
				SmartScriptTable engineSmokeOut_helper(gEnv->pScriptSystem);
				engineSmokeOut_helper->SetValue("name", "engineSmokeOut");
				engineSmokeOut_helper->SetValue("position", Vec3(0.52340001, 1.2908, 1.9173));
				engineSmokeOut_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(engineSmokeOut_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, -1.3008, 0.0034));
				burningPos_helper->SetValue("direction", Vec3(0, 0, 1));
				tank_part_helpers->PushBack(burningPos_helper);
			}
			{
				SmartScriptTable engineSoundPos_helper(gEnv->pScriptSystem);
				engineSoundPos_helper->SetValue("name", "engineSoundPos");
				engineSoundPos_helper->SetValue("position", Vec3(-0.0076000001, 2.6159999, 1.4867001));
				engineSoundPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(engineSoundPos_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(1.1001, 3.95, 1.8));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				tank_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(-1.1001, 3.95, 1.8));
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
				rearLightLeft_helper->SetValue("position", Vec3(-1.2, -3.8999, 1.5));
				rearLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				tank_part_helpers->PushBack(rearLightLeft_helper);
			}
			{
				SmartScriptTable rearLightRight_helper(gEnv->pScriptSystem);
				rearLightRight_helper->SetValue("name", "rearLightRight");
				rearLightRight_helper->SetValue("position", Vec3(1.2, -3.8999, 1.5));
				rearLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				tank_part_helpers->PushBack(rearLightRight_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(0, -0.73189998, 1.1338));
				centerPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(centerPos_helper);
			}
			{
				SmartScriptTable cornerForcePos_helper(gEnv->pScriptSystem);
				cornerForcePos_helper->SetValue("name", "cornerForcePos");
				cornerForcePos_helper->SetValue("position", Vec3(0, 0.31029999, 1.2749));
				cornerForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(cornerForcePos_helper);
			}
			{
				SmartScriptTable pushForcePos_helper(gEnv->pScriptSystem);
				pushForcePos_helper->SetValue("name", "pushForcePos");
				pushForcePos_helper->SetValue("position", Vec3(0, -2.1315999, 1.2756));
				pushForcePos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(pushForcePos_helper);
			}
			{
				SmartScriptTable wakePos_helper(gEnv->pScriptSystem);
				wakePos_helper->SetValue("name", "wakePos");
				wakePos_helper->SetValue("position", Vec3(0, 0.019300001, 1.0354));
				wakePos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(wakePos_helper);
			}
			{
				SmartScriptTable hullDamage_helper(gEnv->pScriptSystem);
				hullDamage_helper->SetValue("name", "hullDamage");
				hullDamage_helper->SetValue("position", Vec3(0, -0.0557, 1.7661));
				hullDamage_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(hullDamage_helper);
			}
			tank_part->SetValue("Helpers", tank_part_helpers);
			SmartScriptTable tank_part_animated(gEnv->pScriptSystem);
			tank_part_animated->SetValue("filename", "Objects/Vehicles/asian_apc/asian_apc.cga");
			tank_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/asian_apc/asian_apc_damaged.cga");
			tank_part->SetValue("Animated", tank_part_animated);
			parts->PushBack(tank_part);
		}
		{
			SmartScriptTable mass_part(gEnv->pScriptSystem);
			mass_part->SetValue("name", "mass");
			mass_part->SetValue("class", "MassBox");
			mass_part->SetValue("component", "");
			mass_part->SetValue("mass", 15000.0f);
			mass_part->SetValue("position", Vec3(-0.0127, 0.095899999, 1.2469));
			mass_part->SetValue("disablePhysics", false);
			mass_part->SetValue("disableCollision", false);
			mass_part->SetValue("isHidden", false);
			SmartScriptTable mass_part_helpers(gEnv->pScriptSystem);
			mass_part->SetValue("Helpers", mass_part_helpers);
			SmartScriptTable mass_part_massbox(gEnv->pScriptSystem);
			mass_part_massbox->SetValue("size", Vec3(1.6274, 3.3371999, 0.83310002));
			mass_part_massbox->SetValue("drivingOffset", -0.5f);
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
			component->SetValue("damageMax", 750.0f);
		}
		else
		{
			component->SetValue("damageMax", 750.0f);
		}
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-2, -4, 0));
		component->SetValue("maxBound", Vec3(2, 4.3, 2.44));
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
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 4.0f);
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
		component->SetValue("minBound", Vec3(-1.5569, 1.3235, 1.0145));
		component->SetValue("maxBound", Vec3(1.636, 3.3278999, 2.3575001));
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
			damagemultiplier->SetValue("multiplier", 2.5f);
			damagemultiplier->SetValue("splash", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.5f);
			damagemultiplier->SetValue("splash", 1.0f);
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
		component->SetValue("minBound", Vec3(-3, -3.2, 2.44));
		component->SetValue("maxBound", Vec3(3, 2, 3.5));
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
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 4.0f);
			damagemultiplier->SetValue("splash", 0.2f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.1f);
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
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel1");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel1 = new VehicleComponent();
		wheel1->Init(this, component);
		m_components.push_back(wheel1);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel2");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel2 = new VehicleComponent();
		wheel2->Init(this, component);
		m_components.push_back(wheel2);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel3");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel3 = new VehicleComponent();
		wheel3->Init(this, component);
		m_components.push_back(wheel3);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel4");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel4 = new VehicleComponent();
		wheel4->Init(this, component);
		m_components.push_back(wheel4);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel5");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel5 = new VehicleComponent();
		wheel5->Init(this, component);
		m_components.push_back(wheel5);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel6");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel6 = new VehicleComponent();
		wheel6->Init(this, component);
		m_components.push_back(wheel6);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel7");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel7 = new VehicleComponent();
		wheel7->Init(this, component);
		m_components.push_back(wheel7);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel8");
		component->SetValue("damageMax", 50.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
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
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "BlowTire");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_blowtire(gEnv->pScriptSystem);
			damagebehavior->SetValue("BlowTire", damagebehavior_blowtire);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_IndicatorWheels_group(gEnv->pScriptSystem);
			damagebehavior_IndicatorWheels_group->SetValue("name", "IndicatorWheels");
			damagebehavior->SetValue("Group", damagebehavior_IndicatorWheels_group);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* wheel8 = new VehicleComponent();
		wheel8->Init(this, component);
		m_components.push_back(wheel8);
	}

	this->RegisterComponentParts(componentParts);

	// Particles
	{
		SmartScriptTable particles(gEnv->pScriptSystem);
		SmartScriptTable particles_exhaust(gEnv->pScriptSystem);
		particles_exhaust->SetValue("insideWater", false);
		particles_exhaust->SetValue("outsideWater", true);
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
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.NKapc_engine_smoke_a");
			EngineDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged75_damageeffect(gEnv->pScriptSystem);
			EngineDamaged75_damageeffect->SetValue("name", "EngineDamaged75");
			EngineDamaged75_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.NKapc_engine_smoke_b");
			EngineDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged75_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "hullDamage");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.NKapc_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "hullDamage");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.NKapc_hull_smoke_b");
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
					wheels->PushBack(1);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(4);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(5);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(8);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			environmentlayer_wheels->SetValue("WheelGroups", environmentlayer_wheels_wheelgroups);
			environmentlayer->SetValue("Wheels", environmentlayer_wheels);
			particles_environmentlayers->PushBack(environmentlayer);
		}
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 1.0f);
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
		SmartScriptTable movement_stdwheeled(gEnv->pScriptSystem);
		movement_stdwheeled->SetValue("vMaxSteerMax", 15.0f);
		movement_stdwheeled->SetValue("steerSpeed", 120.0f);
		movement_stdwheeled->SetValue("steerSpeedMin", 130.0f);
		movement_stdwheeled->SetValue("steerSpeedScale", 1.0f);
		movement_stdwheeled->SetValue("steerSpeedScaleMin", 1.0f);
		movement_stdwheeled->SetValue("v0SteerMax", 28.0f);
		movement_stdwheeled->SetValue("kvSteerMax", 3.0f);
		movement_stdwheeled->SetValue("steerRelaxation", 500.0f);
		movement_stdwheeled->SetValue("pedalLimitMax", 0.050000001f);
		movement_stdwheeled->SetValue("rpmInterpSpeed", 4.0f);
		movement_stdwheeled->SetValue("rpmRelaxSpeed", 2.0f);
		movement_stdwheeled->SetValue("rpmGearShiftSpeed", 4.0f);
		SmartScriptTable movement_stdwheeled_wheeled(gEnv->pScriptSystem);
		movement_stdwheeled_wheeled->SetValue("axleFriction", 250);
		movement_stdwheeled_wheeled->SetValue("axleFrictionMax", 7000);
		movement_stdwheeled_wheeled->SetValue("brakeTorque", 5000);
		movement_stdwheeled_wheeled->SetValue("brakeImpulse", 0.40000001f);
		movement_stdwheeled_wheeled->SetValue("clutchSpeed", 5.0f);
		movement_stdwheeled_wheeled->SetValue("damping", 0.0099999998f);
		movement_stdwheeled_wheeled->SetValue("engineIdleRPM", 600);
		movement_stdwheeled_wheeled->SetValue("engineMaxRPM", 2500);
		movement_stdwheeled_wheeled->SetValue("engineMinRPM", 200);
		movement_stdwheeled_wheeled->SetValue("enginePower", 1200);
		movement_stdwheeled_wheeled->SetValue("engineShiftDownRPM", 1200);
		movement_stdwheeled_wheeled->SetValue("engineShiftUpRPM", 2000);
		movement_stdwheeled_wheeled->SetValue("engineStartRPM", 600);
		movement_stdwheeled_wheeled->SetValue("stabilizer", 0.1f);
		movement_stdwheeled_wheeled->SetValue("maxSteer", 0.51999998f);
		movement_stdwheeled_wheeled->SetValue("maxSpeed", 17.5f);
		movement_stdwheeled_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_stdwheeled_wheeled->SetValue("minEnergy", 0.012f);
		movement_stdwheeled_wheeled->SetValue("slipThreshold", 0.02f);
		movement_stdwheeled_wheeled->SetValue("suspDampingMin", -0.15000001f);
		movement_stdwheeled_wheeled->SetValue("suspDampingMax", -0.64999998f);
		movement_stdwheeled_wheeled->SetValue("gearDirSwitchRPM", 1500);
		movement_stdwheeled_wheeled->SetValue("dynFriction", 1.0f);
		movement_stdwheeled_wheeled->SetValue("latFriction", 1.1f);
		SmartScriptTable movement_stdwheeled_wheeled_gearratios(gEnv->pScriptSystem);
		movement_stdwheeled_wheeled_gearratios->PushBack(-10.0f);
		movement_stdwheeled_wheeled_gearratios->PushBack(0.0f);
		movement_stdwheeled_wheeled_gearratios->PushBack(15.0f);
		movement_stdwheeled_wheeled_gearratios->PushBack(13.0f);
		movement_stdwheeled_wheeled_gearratios->PushBack(10.0f);
		movement_stdwheeled_wheeled->SetValue("gearRatios", movement_stdwheeled_wheeled_gearratios);
		movement_stdwheeled->SetValue("Wheeled", movement_stdwheeled_wheeled);
		SmartScriptTable movement_stdwheeled_soundparams(gEnv->pScriptSystem);
		movement_stdwheeled_soundparams->SetValue("engineSoundPosition", "engineSmokeOut");
		movement_stdwheeled_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_stdwheeled_soundparams->SetValue("roadBumpMinSusp", 8.0f);
		movement_stdwheeled_soundparams->SetValue("roadBumpMinSpeed", 5.0f);
		movement_stdwheeled_soundparams->SetValue("roadBumpIntensity", 0.40000001f);
		movement_stdwheeled_soundparams->SetValue("maxSlipSpeed", 12.5f);
		movement_stdwheeled_soundparams->SetValue("airbrake", 0.5f);
		movement_stdwheeled->SetValue("SoundParams", movement_stdwheeled_soundparams);
		SmartScriptTable movement_stdwheeled_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable enginePower_tweak(gEnv->pScriptSystem);
				enginePower_tweak->SetValue("name", "enginePower");
				enginePower_tweak->SetValue("value", 1.2f);
				enginePower_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(enginePower_tweak);
			}
			{
				SmartScriptTable engineMaxRPM_tweak(gEnv->pScriptSystem);
				engineMaxRPM_tweak->SetValue("name", "engineMaxRPM");
				engineMaxRPM_tweak->SetValue("value", 1.2f);
				engineMaxRPM_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(engineMaxRPM_tweak);
			}
			{
				SmartScriptTable axleFriction_tweak(gEnv->pScriptSystem);
				axleFriction_tweak->SetValue("name", "axleFriction");
				axleFriction_tweak->SetValue("value", 0.89999998f);
				axleFriction_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(axleFriction_tweak);
			}
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_stdwheeled_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		movement_stdwheeled->SetValue("TweakGroups", movement_stdwheeled_tweakgroups);
		SmartScriptTable movement_stdwheeled_boost(gEnv->pScriptSystem);
		movement_stdwheeled_boost->SetValue("endurance", 7.5f);
		movement_stdwheeled_boost->SetValue("regeneration", 7.5f);
		movement_stdwheeled_boost->SetValue("strength", 3.0f);
		movement_stdwheeled->SetValue("Boost", movement_stdwheeled_boost);
		movement->SetValue("StdWheeled", movement_stdwheeled);
		SmartScriptTable movement_stdboat(gEnv->pScriptSystem);
		movement_stdboat->SetValue("velMax", 8.0f);
		movement_stdboat->SetValue("velMaxReverse", 5.0f);
		movement_stdboat->SetValue("pedalLimitReverse", 1.0f);
		movement_stdboat->SetValue("acceleration", 1.8f);
		movement_stdboat->SetValue("accelerationVelMax", 0.85000002f);
		movement_stdboat->SetValue("accelerationMultiplier", 0.75f);
		movement_stdboat->SetValue("pushHelper", "pushForcePos");
		movement_stdboat->SetValue("pushTilt", 0.0f);
		movement_stdboat->SetValue("turnRateMax", 0.69999999f);
		movement_stdboat->SetValue("turnAccel", 0.5f);
		movement_stdboat->SetValue("turnAccelMultiplier", 0.5f);
		movement_stdboat->SetValue("turnDamping", 0.02f);
		movement_stdboat->SetValue("cornerForce", 2.5f);
		movement_stdboat->SetValue("cornerHelper", "cornerForcePos");
		movement_stdboat->SetValue("cornerTilt", 0.0f);
		movement_stdboat->SetValue("waveIdleStrength", Vec3(0.050000001, 0.050000001, 0));
		SmartScriptTable movement_stdboat_soundparams(gEnv->pScriptSystem);
		movement_stdboat_soundparams->SetValue("eventGroup", "");
		movement_stdboat_soundparams->SetValue("engineSoundPosition", "engineSoundPos");
		movement_stdboat_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_stdboat_soundparams->SetValue("rpmPitchSpeed", 1.0f);
		movement_stdboat->SetValue("SoundParams", movement_stdboat_soundparams);
		SmartScriptTable movement_stdboat_animations(gEnv->pScriptSystem);
		movement_stdboat_animations->SetValue("engine", "");
		movement_stdboat->SetValue("Animations", movement_stdboat_animations);
		SmartScriptTable movement_stdboat_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable pushTilt_tweak(gEnv->pScriptSystem);
				pushTilt_tweak->SetValue("name", "pushTilt");
				pushTilt_tweak->SetValue("value", 10.0f);
				pushTilt_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(pushTilt_tweak);
			}
			{
				SmartScriptTable velMax_tweak(gEnv->pScriptSystem);
				velMax_tweak->SetValue("name", "velMax");
				velMax_tweak->SetValue("value", 12.0f);
				velMax_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(velMax_tweak);
			}
			{
				SmartScriptTable acceleration_tweak(gEnv->pScriptSystem);
				acceleration_tweak->SetValue("name", "acceleration");
				acceleration_tweak->SetValue("value", 2.4000001f);
				acceleration_tweak->SetValue("op", 0);
				player_boost_tweakgroup_tweaks->PushBack(acceleration_tweak);
			}
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_stdboat_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		movement_stdboat->SetValue("TweakGroups", movement_stdboat_tweakgroups);
		movement->SetValue("StdBoat", movement_stdboat);

		if (!this->SetMovement("Amphibious", movement))
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
		seat->SetValue("agVehicleName", "US_apc");
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
		seat->SetValue("agVehicleName", "US_apc");
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
		seat->SetValue("agVehicleName", "US_apc");
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
		seat->SetValue("agVehicleName", "US_apc");
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
		seat->SetValue("agVehicleName", "US_apc");
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
		seat->SetValue("agVehicleName", "US_apc");
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
		seat->SetValue("agVehicleName", "US_apc");
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
		this->SetPaintMaterial("objects/vehicles/asian_apc/asian_apc.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/asian_apc/asian_apc_us.mtl");
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("submergedRatioMax", 0.99000001f);
		damages->SetValue("submergedDamageMult", 0.0099999998f);
		damages->SetValue("collDamageThreshold", 300.0f);
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
