#include "Asian_tank.h"

Asian_tank::Asian_tank()
{
	// enable Asian_tank::Init function
	this->Vehicle::PatchVTable();
}

Asian_tank::~Asian_tank()
{
}

bool Asian_tank::Init(IGameObject* pGameObject)
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
						else if (m_modName == "TACCannon")
						{
							cannon_part->SetValue("useOption", 1);
						}
						else if (m_modName == "MOAC")
						{
							cannon_part->SetValue("useOption", 1);
						}
						else if (m_modName == "MOAR")
						{
							cannon_part->SetValue("useOption", 1);
						}
						else if (m_modName == "Singularity")
						{
							cannon_part->SetValue("useOption", 1);
						}
						else
						{
							cannon_part->SetValue("useOption", 0);
						}
						SmartScriptTable cannon_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable cannonViewPos_helper(gEnv->pScriptSystem);
							cannonViewPos_helper->SetValue("name", "cannonViewPos");
							cannonViewPos_helper->SetValue("position", Vec3(-0.56099999, 0.93040001, 2.5137));
							cannonViewPos_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonViewPos_helper);
						}
						{
							SmartScriptTable cannonOut_helper(gEnv->pScriptSystem);
							cannonOut_helper->SetValue("name", "cannonOut");
							cannonOut_helper->SetValue("position", Vec3(0, 6.4447999, 2.233));
							cannonOut_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOut_helper);
						}
						{
							SmartScriptTable cannonBase_helper(gEnv->pScriptSystem);
							cannonBase_helper->SetValue("name", "cannonBase");
							cannonBase_helper->SetValue("position", Vec3(0, 0, 2.233));
							cannonBase_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonBase_helper);
						}
						{
							SmartScriptTable coaxOut_helper(gEnv->pScriptSystem);
							coaxOut_helper->SetValue("name", "coaxOut");
							coaxOut_helper->SetValue("position", Vec3(0.45190001, 0.99510002, 2.233));
							coaxOut_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(coaxOut_helper);
						}
						{
							SmartScriptTable cannonOutMOAC_helper(gEnv->pScriptSystem);
							cannonOutMOAC_helper->SetValue("name", "cannonOutMOAC");
							cannonOutMOAC_helper->SetValue("position", Vec3(0.0055999998, 4.8102999, 2.2304001));
							cannonOutMOAC_helper->SetValue("direction", Vec3(0, 1, 0));
							cannon_part_helpers->PushBack(cannonOutMOAC_helper);
						}
						cannon_part->SetValue("Helpers", cannon_part_helpers);
						SmartScriptTable cannon_part_animatedjoint(gEnv->pScriptSystem);
						if (m_modName == "GaussCannon")
						{
							cannon_part_animatedjoint->SetValue("filename", "objects/vehicles/tank_cannon_options/gausscannon.cgf");
						}
						else if (m_modName == "TACCannon")
						{
							cannon_part_animatedjoint->SetValue("filename", "objects/vehicles/tank_cannon_options/taccannon.cgf");
						}
						else if (m_modName == "MOAC")
						{
							cannon_part_animatedjoint->SetValue("filename", "objects/vehicles/tank_cannon_options/moac.cgf");
						}
						else if (m_modName == "MOAR")
						{
							cannon_part_animatedjoint->SetValue("filename", "objects/vehicles/tank_cannon_options/moar.cgf");
						}
						else if (m_modName == "Singularity")
						{
							cannon_part_animatedjoint->SetValue("filename", "objects/vehicles/tank_cannon_options/singularity.cgf");
						}
						else
						{
							cannon_part_animatedjoint->SetValue("filename", "");
						}
						if (m_modName == "GaussCannon")
						{
							cannon_part_animatedjoint->SetValue("filenameDestroyed", "objects/vehicles/tank_cannon_options/gausscannon_damaged.cgf");
						}
						else if (m_modName == "TACCannon")
						{
							cannon_part_animatedjoint->SetValue("filenameDestroyed", "objects/vehicles/tank_cannon_options/taccannon_damaged.cgf");
						}
						else if (m_modName == "MOAC")
						{
							cannon_part_animatedjoint->SetValue("filenameDestroyed", "objects/vehicles/tank_cannon_options/moac_damaged.cgf");
						}
						else if (m_modName == "MOAR")
						{
							cannon_part_animatedjoint->SetValue("filenameDestroyed", "objects/vehicles/tank_cannon_options/moar_damaged.cgf");
						}
						else if (m_modName == "Singularity")
						{
							cannon_part_animatedjoint->SetValue("filenameDestroyed", "objects/vehicles/tank_cannon_options/singularity_damaged.cgf");
						}
						else
						{
							cannon_part_animatedjoint->SetValue("filenameDestroyed", "");
						}
						cannon_part->SetValue("AnimatedJoint", cannon_part_animatedjoint);
						cannon_base_part_parts->PushBack(cannon_part);
					}
					cannon_base_part->SetValue("Parts", cannon_base_part_parts);
					SmartScriptTable cannon_base_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable cannon_base_part_animatedjoint_rotation(gEnv->pScriptSystem);
					cannon_base_part_animatedjoint_rotation->SetValue("pitchSpeed", 31.0f);
					cannon_base_part_animatedjoint_rotation->SetValue("pitchAccel", 85.0f);
					SmartScriptTable cannon_base_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					cannon_base_part_animatedjoint_rotation_pitchlimits->PushBack(-5.0f);
					cannon_base_part_animatedjoint_rotation_pitchlimits->PushBack(35.0f);
					cannon_base_part_animatedjoint_rotation->SetValue("pitchLimits", cannon_base_part_animatedjoint_rotation_pitchlimits);
					SmartScriptTable cannon_base_part_animatedjoint_rotation_rotationtest(gEnv->pScriptSystem);
					cannon_base_part_animatedjoint_rotation_rotationtest->SetValue("helper1", "cannonBase");
					cannon_base_part_animatedjoint_rotation_rotationtest->SetValue("helper2", "cannonOut");
					cannon_base_part_animatedjoint_rotation_rotationtest->SetValue("radius", 0.25f);
					cannon_base_part_animatedjoint_rotation->SetValue("RotationTest", cannon_base_part_animatedjoint_rotation_rotationtest);
					SmartScriptTable cannon_base_part_animatedjoint_rotation_sound(gEnv->pScriptSystem);
					cannon_base_part_animatedjoint_rotation_sound->SetValue("event", "cannon");
					cannon_base_part_animatedjoint_rotation_sound->SetValue("helper", "turretPos");
					cannon_base_part_animatedjoint_rotation->SetValue("Sound", cannon_base_part_animatedjoint_rotation_sound);
					cannon_base_part_animatedjoint->SetValue("Rotation", cannon_base_part_animatedjoint_rotation);
					cannon_base_part->SetValue("AnimatedJoint", cannon_base_part_animatedjoint);
					turret_part_parts->PushBack(cannon_base_part);
				}
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
							gunnerEyePos_helper->SetValue("position", Vec3(0.75319999, -1.0319999, 3.3840001));
							gunnerEyePos_helper->SetValue("direction", Vec3(0, 1, 0));
							shiten_part_helpers->PushBack(gunnerEyePos_helper);
						}
						{
							SmartScriptTable searchLightPos_helper(gEnv->pScriptSystem);
							searchLightPos_helper->SetValue("name", "searchLightPos");
							searchLightPos_helper->SetValue("position", Vec3(0.93000001, -0.47, 3.0799999));
							searchLightPos_helper->SetValue("direction", Vec3(0, 1, 0));
							shiten_part_helpers->PushBack(searchLightPos_helper);
						}
						shiten_part->SetValue("Helpers", shiten_part_helpers);
						SmartScriptTable shiten_part_animatedjoint(gEnv->pScriptSystem);
						SmartScriptTable shiten_part_animatedjoint_rotation(gEnv->pScriptSystem);
						shiten_part_animatedjoint_rotation->SetValue("pitchSpeed", 70.0f);
						shiten_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
						SmartScriptTable shiten_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
						shiten_part_animatedjoint_rotation_pitchlimits->PushBack(-17.0f);
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
						SmartScriptTable gunnerPos_helper(gEnv->pScriptSystem);
						gunnerPos_helper->SetValue("name", "gunnerPos");
						gunnerPos_helper->SetValue("position", Vec3(0, 0, 0));
						gunnerPos_helper->SetValue("direction", Vec3(0, 1, 0));
						shitenTurret_part_helpers->PushBack(gunnerPos_helper);
					}
					shitenTurret_part->SetValue("Helpers", shitenTurret_part_helpers);
					SmartScriptTable shitenTurret_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable shitenTurret_part_animatedjoint_rotation(gEnv->pScriptSystem);
					shitenTurret_part_animatedjoint_rotation->SetValue("yawSpeed", 135.0f);
					shitenTurret_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
					SmartScriptTable shitenTurret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
					shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(0.0f);
					shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(0.0f);
					shitenTurret_part_animatedjoint_rotation->SetValue("yawLimits", shitenTurret_part_animatedjoint_rotation_yawlimits);
					shitenTurret_part_animatedjoint->SetValue("Rotation", shitenTurret_part_animatedjoint_rotation);
					shitenTurret_part->SetValue("AnimatedJoint", shitenTurret_part_animatedjoint);
					turret_part_parts->PushBack(shitenTurret_part);
				}
				turret_part->SetValue("Parts", turret_part_parts);
				SmartScriptTable turret_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable driverSitPos_helper(gEnv->pScriptSystem);
					driverSitPos_helper->SetValue("name", "driverSitPos");
					driverSitPos_helper->SetValue("position", Vec3(0.74169999, -1.2349, 2.0105));
					driverSitPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(driverSitPos_helper);
				}
				{
					SmartScriptTable turretPos_helper(gEnv->pScriptSystem);
					turretPos_helper->SetValue("name", "turretPos");
					turretPos_helper->SetValue("position", Vec3(-0.001, 0.1201, 1.584));
					turretPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(turretPos_helper);
				}
				turret_part->SetValue("Helpers", turret_part_helpers);
				SmartScriptTable turret_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable turret_part_animatedjoint_rotation(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation->SetValue("yawSpeed", 37.0f);
				turret_part_animatedjoint_rotation->SetValue("yawAccel", 115.0f);
				SmartScriptTable turret_part_animatedjoint_rotation_sound(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation_sound->SetValue("event", "turret");
				turret_part_animatedjoint_rotation_sound->SetValue("eventDamage", "sounds/vehicles:vehicle_accessories:turret_turn_damaged");
				turret_part_animatedjoint_rotation_sound->SetValue("helper", "turretPos");
				turret_part_animatedjoint_rotation->SetValue("Sound", turret_part_animatedjoint_rotation_sound);
				SmartScriptTable turret_part_animatedjoint_rotation_rotationtest(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation_rotationtest->SetValue("helper1", "cannonBase");
				turret_part_animatedjoint_rotation_rotationtest->SetValue("helper2", "cannonOut");
				turret_part_animatedjoint_rotation_rotationtest->SetValue("radius", 0.25f);
				turret_part_animatedjoint_rotation->SetValue("RotationTest", turret_part_animatedjoint_rotation_rotationtest);
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
				wheel2_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel2_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel2_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel2_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel3_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel3_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel3_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel3_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel3_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel4_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel4_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel4_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel4_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel5_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel5_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel5_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel5_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel6_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel6_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel6_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel6_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel6_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel7_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel7_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel7_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel7_part_subpartwheel->SetValue("suspLength", 0.44999999f);
				wheel7_part_subpartwheel->SetValue("rayCast", true);
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
				wheel8_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel8_part_subpartwheel->SetValue("driving", false);
				wheel8_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel8_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel8_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel8_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel8_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel9_part_subpartwheel->SetValue("axle", 8);
				wheel9_part_subpartwheel->SetValue("density", 3000.0f);
				wheel9_part_subpartwheel->SetValue("damping", 0.0f);
				wheel9_part_subpartwheel->SetValue("driving", true);
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
				wheel10_part_subpartwheel->SetValue("axle", 0);
				wheel10_part_subpartwheel->SetValue("density", 3000.0f);
				wheel10_part_subpartwheel->SetValue("damping", 0.0f);
				wheel10_part_subpartwheel->SetValue("driving", false);
				wheel10_part_subpartwheel->SetValue("lenMax", 0.0f);
				wheel10_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel10_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel10_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel10_part_subpartwheel->SetValue("suspLength", 0.0f);
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
				wheel11_part_subpartwheel->SetValue("axle", 1);
				wheel11_part_subpartwheel->SetValue("density", 3000.0f);
				wheel11_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel11_part_subpartwheel->SetValue("driving", false);
				wheel11_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel11_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel11_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel11_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel11_part_subpartwheel->SetValue("suspLength", 0.44999999f);
				wheel11_part_subpartwheel->SetValue("rayCast", false);
				wheel11_part->SetValue("SubPartWheel", wheel11_part_subpartwheel);
				tank_part_parts->PushBack(wheel11_part);
			}
			{
				SmartScriptTable wheel12_part(gEnv->pScriptSystem);
				wheel12_part->SetValue("name", "wheel12");
				wheel12_part->SetValue("class", "SubPartWheel");
				wheel12_part->SetValue("isHidden", false);
				SmartScriptTable wheel12_part_subpartwheel(gEnv->pScriptSystem);
				wheel12_part_subpartwheel->SetValue("axle", 2);
				wheel12_part_subpartwheel->SetValue("density", 3000.0f);
				wheel12_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel12_part_subpartwheel->SetValue("driving", false);
				wheel12_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel12_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel12_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel12_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel12_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel13_part_subpartwheel->SetValue("axle", 3);
				wheel13_part_subpartwheel->SetValue("density", 3000.0f);
				wheel13_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel13_part_subpartwheel->SetValue("driving", false);
				wheel13_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel13_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel13_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel13_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel13_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel14_part_subpartwheel->SetValue("axle", 4);
				wheel14_part_subpartwheel->SetValue("density", 3000.0f);
				wheel14_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel14_part_subpartwheel->SetValue("driving", false);
				wheel14_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel14_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel14_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel14_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel14_part_subpartwheel->SetValue("suspLength", 0.44999999f);
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
				wheel15_part_subpartwheel->SetValue("axle", 5);
				wheel15_part_subpartwheel->SetValue("density", 3000.0f);
				wheel15_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel15_part_subpartwheel->SetValue("driving", false);
				wheel15_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel15_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel15_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel15_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel15_part_subpartwheel->SetValue("suspLength", 0.44999999f);
				wheel15_part_subpartwheel->SetValue("rayCast", true);
				wheel15_part->SetValue("SubPartWheel", wheel15_part_subpartwheel);
				tank_part_parts->PushBack(wheel15_part);
			}
			{
				SmartScriptTable wheel16_part(gEnv->pScriptSystem);
				wheel16_part->SetValue("name", "wheel16");
				wheel16_part->SetValue("class", "SubPartWheel");
				wheel16_part->SetValue("isHidden", false);
				SmartScriptTable wheel16_part_subpartwheel(gEnv->pScriptSystem);
				wheel16_part_subpartwheel->SetValue("axle", 6);
				wheel16_part_subpartwheel->SetValue("density", 3000.0f);
				wheel16_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel16_part_subpartwheel->SetValue("driving", false);
				wheel16_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel16_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel16_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel16_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel16_part_subpartwheel->SetValue("suspLength", 0.44999999f);
				wheel16_part_subpartwheel->SetValue("rayCast", true);
				wheel16_part->SetValue("SubPartWheel", wheel16_part_subpartwheel);
				tank_part_parts->PushBack(wheel16_part);
			}
			{
				SmartScriptTable wheel17_part(gEnv->pScriptSystem);
				wheel17_part->SetValue("name", "wheel17");
				wheel17_part->SetValue("class", "SubPartWheel");
				wheel17_part->SetValue("isHidden", false);
				SmartScriptTable wheel17_part_subpartwheel(gEnv->pScriptSystem);
				wheel17_part_subpartwheel->SetValue("axle", 7);
				wheel17_part_subpartwheel->SetValue("density", 3000.0f);
				wheel17_part_subpartwheel->SetValue("damping", -0.30000001f);
				wheel17_part_subpartwheel->SetValue("driving", false);
				wheel17_part_subpartwheel->SetValue("lenMax", 0.69999999f);
				wheel17_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel17_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel17_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel17_part_subpartwheel->SetValue("suspLength", 0.44999999f);
				wheel17_part_subpartwheel->SetValue("rayCast", false);
				wheel17_part->SetValue("SubPartWheel", wheel17_part_subpartwheel);
				tank_part_parts->PushBack(wheel17_part);
			}
			{
				SmartScriptTable wheel18_part(gEnv->pScriptSystem);
				wheel18_part->SetValue("name", "wheel18");
				wheel18_part->SetValue("class", "SubPartWheel");
				wheel18_part->SetValue("isHidden", false);
				SmartScriptTable wheel18_part_subpartwheel(gEnv->pScriptSystem);
				wheel18_part_subpartwheel->SetValue("axle", 8);
				wheel18_part_subpartwheel->SetValue("density", 3000.0f);
				wheel18_part_subpartwheel->SetValue("damping", 0.0f);
				wheel18_part_subpartwheel->SetValue("driving", true);
				wheel18_part_subpartwheel->SetValue("lenMax", 0.0f);
				wheel18_part_subpartwheel->SetValue("maxFriction", 1.1f);
				wheel18_part_subpartwheel->SetValue("minFriction", 1.1f);
				wheel18_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel18_part_subpartwheel->SetValue("suspLength", 0.0f);
				wheel18_part_subpartwheel->SetValue("rayCast", false);
				wheel18_part->SetValue("SubPartWheel", wheel18_part_subpartwheel);
				tank_part_parts->PushBack(wheel18_part);
			}
			tank_part->SetValue("Parts", tank_part_parts);
			SmartScriptTable tank_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable driverEnterPos_helper(gEnv->pScriptSystem);
				driverEnterPos_helper->SetValue("name", "driverEnterPos");
				if (m_modName == "MP")
				{
					driverEnterPos_helper->SetValue("position", Vec3(-3.3520999, -0.38240001, 0.50300002));
				}
				else if (m_modName == "GaussCannon")
				{
					driverEnterPos_helper->SetValue("position", Vec3(-3.3520999, -0.38240001, 0.50300002));
				}
				else if (m_modName == "TACCannon")
				{
					driverEnterPos_helper->SetValue("position", Vec3(-3.3520999, -0.38240001, 0.50300002));
				}
				else if (m_modName == "MOAC")
				{
					driverEnterPos_helper->SetValue("position", Vec3(-3.3520999, -0.38240001, 0.50300002));
				}
				else if (m_modName == "MOAR")
				{
					driverEnterPos_helper->SetValue("position", Vec3(-3.3520999, -0.38240001, 0.50300002));
				}
				else if (m_modName == "Singularity")
				{
					driverEnterPos_helper->SetValue("position", Vec3(-3.3520999, -0.38240001, 0.50300002));
				}
				else
				{
					driverEnterPos_helper->SetValue("position", Vec3(3.3520999, -0.38229999, 0.50300002));
				}
				driverEnterPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(driverEnterPos_helper);
			}
			{
				SmartScriptTable gunnerEnterPos_helper(gEnv->pScriptSystem);
				gunnerEnterPos_helper->SetValue("name", "gunnerEnterPos");
				gunnerEnterPos_helper->SetValue("position", Vec3(3.3452001, -1.3589, 0.49900001));
				gunnerEnterPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(gunnerEnterPos_helper);
			}
			{
				SmartScriptTable engineSmokeOut_helper(gEnv->pScriptSystem);
				engineSmokeOut_helper->SetValue("name", "engineSmokeOut");
				engineSmokeOut_helper->SetValue("position", Vec3(-0.2119, -3.5741999, 1.8890001));
				engineSmokeOut_helper->SetValue("direction", Vec3(0, 0, 1));
				tank_part_helpers->PushBack(engineSmokeOut_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, -1.335, 1.699));
				burningPos_helper->SetValue("direction", Vec3(0, 0, 1));
				tank_part_helpers->PushBack(burningPos_helper);
			}
			{
				SmartScriptTable ai_anchor1_helper(gEnv->pScriptSystem);
				ai_anchor1_helper->SetValue("name", "ai_anchor1");
				ai_anchor1_helper->SetValue("position", Vec3(-0.034200002, -6.9189, 0.64899999));
				ai_anchor1_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(ai_anchor1_helper);
			}
			{
				SmartScriptTable ai_anchor2_helper(gEnv->pScriptSystem);
				ai_anchor2_helper->SetValue("name", "ai_anchor2");
				ai_anchor2_helper->SetValue("position", Vec3(-2.925, -3.582, 0.77499998));
				ai_anchor2_helper->SetValue("direction", Vec3(0.9659, -0.259, 0));
				tank_part_helpers->PushBack(ai_anchor2_helper);
			}
			{
				SmartScriptTable ai_anchor3_helper(gEnv->pScriptSystem);
				ai_anchor3_helper->SetValue("name", "ai_anchor3");
				ai_anchor3_helper->SetValue("position", Vec3(3.302, -3.4298999, 0.79299998));
				ai_anchor3_helper->SetValue("direction", Vec3(-0.93970001, -0.34189999, 0));
				tank_part_helpers->PushBack(ai_anchor3_helper);
			}
			{
				SmartScriptTable ai_anchor4_helper(gEnv->pScriptSystem);
				ai_anchor4_helper->SetValue("name", "ai_anchor4");
				ai_anchor4_helper->SetValue("position", Vec3(-3.2051001, 2.0039001, 0.61000001));
				ai_anchor4_helper->SetValue("direction", Vec3(0.70709997, 0.70709997, 0));
				tank_part_helpers->PushBack(ai_anchor4_helper);
			}
			{
				SmartScriptTable ai_anchor5_helper(gEnv->pScriptSystem);
				ai_anchor5_helper->SetValue("name", "ai_anchor5");
				ai_anchor5_helper->SetValue("position", Vec3(3.3159001, 1.6709, 0.68300003));
				ai_anchor5_helper->SetValue("direction", Vec3(-0.81889999, 0.57389998, 0));
				tank_part_helpers->PushBack(ai_anchor5_helper);
			}
			{
				SmartScriptTable exhaust_helper(gEnv->pScriptSystem);
				exhaust_helper->SetValue("name", "exhaust");
				exhaust_helper->SetValue("position", Vec3(1.8921, -3.7390001, 1.622));
				exhaust_helper->SetValue("direction", Vec3(0.93970001, 0, -0.34189999));
				tank_part_helpers->PushBack(exhaust_helper);
			}
			{
				SmartScriptTable hull_burning_helper(gEnv->pScriptSystem);
				hull_burning_helper->SetValue("name", "hull_burning");
				hull_burning_helper->SetValue("position", Vec3(0.12819999, -1.0007, 1.439));
				hull_burning_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(hull_burning_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(1.05, 2.4000001, 1.6));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				tank_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(-1.05, 2.4000001, 1.6));
				headLightRight_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				tank_part_helpers->PushBack(headLightRight_helper);
			}
			{
				SmartScriptTable headLightCenter_helper(gEnv->pScriptSystem);
				headLightCenter_helper->SetValue("name", "headLightCenter");
				headLightCenter_helper->SetValue("position", Vec3(0.0178, 3.2937, 1.476));
				headLightCenter_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				tank_part_helpers->PushBack(headLightCenter_helper);
			}
			{
				SmartScriptTable rearLightLeft_helper(gEnv->pScriptSystem);
				rearLightLeft_helper->SetValue("name", "rearLightLeft");
				rearLightLeft_helper->SetValue("position", Vec3(-1.7141, -5.0988002, 1.6416));
				rearLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				tank_part_helpers->PushBack(rearLightLeft_helper);
			}
			{
				SmartScriptTable rearLightRight_helper(gEnv->pScriptSystem);
				rearLightRight_helper->SetValue("name", "rearLightRight");
				rearLightRight_helper->SetValue("position", Vec3(1.719, -5.0991001, 1.6398));
				rearLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				tank_part_helpers->PushBack(rearLightRight_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(0, -0.99290001, 1.1682));
				centerPos_helper->SetValue("direction", Vec3(0, 1, 0));
				tank_part_helpers->PushBack(centerPos_helper);
			}
			tank_part->SetValue("Helpers", tank_part_helpers);
			SmartScriptTable tank_part_animated(gEnv->pScriptSystem);
			tank_part_animated->SetValue("filename", "Objects/Vehicles/asian_tank/asian_tank.cga");
			tank_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/asian_tank/asian_tank_damaged.cga");
			tank_part->SetValue("Animated", tank_part_animated);
			parts->PushBack(tank_part);
		}
		{
			SmartScriptTable leftTread_part(gEnv->pScriptSystem);
			leftTread_part->SetValue("name", "leftTread");
			leftTread_part->SetValue("class", "Tread");
			leftTread_part->SetValue("component", "leftTread");
			SmartScriptTable leftTread_part_tread(gEnv->pScriptSystem);
			leftTread_part_tread->SetValue("filename", "Objects/Vehicles/asian_tank/tread_left.chr");
			leftTread_part_tread->SetValue("materialName", "asian_tank_treads");
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
			rightTread_part_tread->SetValue("filename", "Objects/Vehicles/asian_tank/tread_right.chr");
			rightTread_part_tread->SetValue("materialName", "asian_tank_treads");
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
			massBox_part->SetValue("mass", 30000.0f);
			massBox_part->SetValue("position", Vec3(0, -0.87180001, 0.92000002));
			massBox_part->SetValue("disablePhysics", false);
			massBox_part->SetValue("disableCollision", false);
			massBox_part->SetValue("isHidden", false);
			SmartScriptTable massBox_part_helpers(gEnv->pScriptSystem);
			massBox_part->SetValue("Helpers", massBox_part_helpers);
			SmartScriptTable massBox_part_massbox(gEnv->pScriptSystem);
			massBox_part_massbox->SetValue("size", Vec3(2, 3, 1));
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
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 1200.0f);
		}
		else if (m_modName == "GaussCannon")
		{
			component->SetValue("damageMax", 1200.0f);
		}
		else if (m_modName == "TACCannon")
		{
			component->SetValue("damageMax", 1200.0f);
		}
		else if (m_modName == "MOAC")
		{
			component->SetValue("damageMax", 1200.0f);
		}
		else if (m_modName == "MOAR")
		{
			component->SetValue("damageMax", 1200.0f);
		}
		else if (m_modName == "Singularity")
		{
			component->SetValue("damageMax", 1200.0f);
		}
		else
		{
			component->SetValue("damageMax", 1200.0f);
		}
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-2, -5.1051998, 0.69999999));
		component->SetValue("maxBound", Vec3(2, 3.2268, 1.95));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			if (m_modName == "MP")
			{
				damagemultiplier->SetValue("multiplier", 1.5f);
			}
			else if (m_modName == "GaussCannon")
			{
				damagemultiplier->SetValue("multiplier", 1.5f);
			}
			else if (m_modName == "TACCannon")
			{
				damagemultiplier->SetValue("multiplier", 1.5f);
			}
			else if (m_modName == "MOAC")
			{
				damagemultiplier->SetValue("multiplier", 1.5f);
			}
			else if (m_modName == "MOAR")
			{
				damagemultiplier->SetValue("multiplier", 1.5f);
			}
			else if (m_modName == "Singularity")
			{
				damagemultiplier->SetValue("multiplier", 1.5f);
			}
			else
			{
				damagemultiplier->SetValue("multiplier", 1.5f);
			}
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 3.0f);
			damagemultiplier->SetValue("splash", 2.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.025f);
			damagemultiplier->SetValue("splash", 0.015f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 2.25f);
			damagemultiplier->SetValue("splash", 0.5f);
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
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "HullDamaged25");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.75f);
			damagebehavior->SetValue("damageRatioMax", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "HullDamaged75");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
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
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* hull = new VehicleComponent();
		hull->Init(this, component);
		m_components.push_back(hull);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "engine");
		component->SetValue("damageMax", 700.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.5168, -5.2178001, 0.5));
		component->SetValue("maxBound", Vec3(1.5178, -3.1889999, 2));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.2f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.4f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.66f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "EngineDamaged");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.75f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "EngineDamaged75");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
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
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			damagebehavior->SetValue("damageRatioMax", 0.75f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "EngineDamaged25");
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
		component->SetValue("damageMax", 800.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-3.5, -4, 1.95));
		component->SetValue("maxBound", Vec3(3.5, 2.5, 3));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.75f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
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
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 0.1f);
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
		component->SetValue("name", "leftTread");
		component->SetValue("damageMax", 490.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.9400001, -4.8759999, -0.219));
		component->SetValue("maxBound", Vec3(-1.0862, 3.0342, 1.33));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.75f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
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
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.05f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.5f);
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
		component->SetValue("damageMax", 490.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(1.135, -4.7961001, -0.22));
		component->SetValue("maxBound", Vec3(1.9400001, 3.0978999, 1.329));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.75f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
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
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.05f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.5f);
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
		component->SetValue("name", "FuelTankLeft");
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.3372, -5.8615999, 1.2402));
		component->SetValue("maxBound", Vec3(-0.1257, -5.0237002, 2.1672001));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		component->SetValue("isOnlyDamageableByPlayer", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 2.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "FuelTankBurningLeft");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Explosion");
			SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
			damagebehavior_explosion->SetValue("helper", "");
			if (m_modName == "MP")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "GaussCannon")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "TACCannon")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "MOAC")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "MOAR")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "Singularity")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else
			{
				damagebehavior_explosion->SetValue("damage", 100.0f);
			}
			damagebehavior_explosion->SetValue("radius", 10.0f);
			damagebehavior_explosion->SetValue("minRadius", 5.0f);
			damagebehavior_explosion->SetValue("minPhysRadius", 2.2f);
			damagebehavior_explosion->SetValue("physRadius", 3.0f);
			damagebehavior_explosion->SetValue("pressure", 500.0f);
			damagebehavior->SetValue("Explosion", damagebehavior_explosion);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* fuelTankLeft = new VehicleComponent();
		fuelTankLeft->Init(this, component);
		m_components.push_back(fuelTankLeft);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "FuelTankRight");
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.19580001, -5.8615999, 1.2402));
		component->SetValue("maxBound", Vec3(1.3643, -5.0237002, 2.1672001));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		component->SetValue("isOnlyDamageableByPlayer", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.3f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 2.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "FuelTankBurningRight");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Explosion");
			SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
			damagebehavior_explosion->SetValue("helper", "");
			if (m_modName == "MP")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "GaussCannon")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "TACCannon")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "MOAC")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "MOAR")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else if (m_modName == "Singularity")
			{
				damagebehavior_explosion->SetValue("damage", 25.0f);
			}
			else
			{
				damagebehavior_explosion->SetValue("damage", 100.0f);
			}
			damagebehavior_explosion->SetValue("radius", 10.0f);
			damagebehavior_explosion->SetValue("minRadius", 5.0f);
			damagebehavior_explosion->SetValue("minPhysRadius", 2.2f);
			damagebehavior_explosion->SetValue("physRadius", 3.0f);
			damagebehavior_explosion->SetValue("pressure", 500.0f);
			damagebehavior->SetValue("Explosion", damagebehavior_explosion);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* fuelTankRight = new VehicleComponent();
		fuelTankRight->Init(this, component);
		m_components.push_back(fuelTankRight);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "mainEnterLeft");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-2.7177999, -3.9812, 0.0583));
		component->SetValue("maxBound", Vec3(-1.3433, 2.0685999, 1.766));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* mainEnterLeft = new VehicleComponent();
		mainEnterLeft->Init(this, component);
		m_components.push_back(mainEnterLeft);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "mainEnterRight");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(1.3123, -3.9788001, 0.0244));
		component->SetValue("maxBound", Vec3(2.6775, 2.0778999, 1.8647));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* mainEnterRight = new VehicleComponent();
		mainEnterRight->Init(this, component);
		m_components.push_back(mainEnterRight);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "gunnerEnterTop");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.8416, -2.5747001, 2.2190001));
		component->SetValue("maxBound", Vec3(1.1461999, -0.017100001, 3.7679999));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* gunnerEnterTop = new VehicleComponent();
		gunnerEnterTop->Init(this, component);
		m_components.push_back(gunnerEnterTop);
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
			damagebehavior_burn->SetValue("selfDamage", 125.0f);
			damagebehavior_burn->SetValue("radius", 6.0f);
			damagebehavior_burn->SetValue("interval", 0.75f);
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
		particles_exhaust_enginestart->SetValue("effect", "vehicle_fx.vehicle_exhaust.tank_starter");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "vehicle_fx.vehicle_exhaust.tank_engine_off");
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
			SmartScriptTable EngineDamaged25_damageeffect(gEnv->pScriptSystem);
			EngineDamaged25_damageeffect->SetValue("name", "EngineDamaged25");
			EngineDamaged25_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Tank_engine_smoke_a");
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged75_damageeffect(gEnv->pScriptSystem);
			EngineDamaged75_damageeffect->SetValue("name", "EngineDamaged75");
			EngineDamaged75_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Tank_engine_smoke_b");
			particles_damageeffects->PushBack(EngineDamaged75_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "hull_burning");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Tank_hull_smoke_a");
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "hull_burning");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Tank_hull_smoke_b");
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		{
			SmartScriptTable FuelTankBurningLeft_damageeffect(gEnv->pScriptSystem);
			FuelTankBurningLeft_damageeffect->SetValue("name", "FuelTankBurningLeft");
			FuelTankBurningLeft_damageeffect->SetValue("helper", "FuelTankLeft");
			FuelTankBurningLeft_damageeffect->SetValue("effect", "vehicle_fx.chinese_truck.fuel_tank_jeep");
			particles_damageeffects->PushBack(FuelTankBurningLeft_damageeffect);
		}
		{
			SmartScriptTable FuelTankBurningRight_damageeffect(gEnv->pScriptSystem);
			FuelTankBurningRight_damageeffect->SetValue("name", "FuelTankBurningRight");
			FuelTankBurningRight_damageeffect->SetValue("helper", "FuelTankRight");
			FuelTankBurningRight_damageeffect->SetValue("effect", "vehicle_fx.chinese_truck.fuel_tank_jeep");
			particles_damageeffects->PushBack(FuelTankBurningRight_damageeffect);
		}
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
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Tank_engine_smoke_a");
			EngineDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
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
					wheels->PushBack(8);
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
					wheels->PushBack(17);
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
			shoot_state->SetValue("animation", "tank_cannon_recoil");
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
		movement->SetValue("steerSpeed", 8.0f);
		movement->SetValue("steerSpeedRelax", 5.0f);
		movement->SetValue("steerLimit", 1.0f);
		movement->SetValue("latFricMin", 0.40000001f);
		movement->SetValue("latFricMinSteer", 0.1f);
		movement->SetValue("latFricMax", 1.2f);
		movement->SetValue("latSlipMin", 0.89999998f);
		movement->SetValue("latSlipMax", 2.5999999f);
		movement->SetValue("rpmInterpSpeed", 2.0f);
		movement->SetValue("rpmRelaxSpeed", 2.0f);
		movement->SetValue("rpmGearShiftSpeed", 2.0f);
		movement->SetValue("steeringImpulseMin", 1.0f);
		movement->SetValue("steeringImpulseMax", 1.8f);
		movement->SetValue("steeringImpulseRelaxMin", 4.0f);
		movement->SetValue("steeringImpulseRelaxMax", 6.0f);
		SmartScriptTable movement_wheeled(gEnv->pScriptSystem);
		movement_wheeled->SetValue("axleFriction", 500);
		movement_wheeled->SetValue("axleFrictionMax", 6000);
		movement_wheeled->SetValue("brakeTorque", 6000);
		movement_wheeled->SetValue("clutchSpeed", 8.0f);
		movement_wheeled->SetValue("damping", 0.02f);
		movement_wheeled->SetValue("engineIdleRPM", 700);
		movement_wheeled->SetValue("engineMaxRPM", 3000);
		movement_wheeled->SetValue("engineMinRPM", 200);
		movement_wheeled->SetValue("enginePower", 800);
		movement_wheeled->SetValue("engineShiftDownRPM", 1300);
		movement_wheeled->SetValue("engineShiftUpRPM", 1800);
		movement_wheeled->SetValue("engineStartRPM", 700);
		movement_wheeled->SetValue("stabilizer", 0.1f);
		movement_wheeled->SetValue("stabiMin", 0.0f);
		movement_wheeled->SetValue("stabiMax", 0.25f);
		movement_wheeled->SetValue("minBrakingFriction", 1.2f);
		movement_wheeled->SetValue("maxSteer", 0.78500003f);
		movement_wheeled->SetValue("maxSpeed", 15.0f);
		movement_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_wheeled->SetValue("minEnergy", 0.01f);
		movement_wheeled->SetValue("slipThreshold", 0.050000001f);
		movement_wheeled->SetValue("gearDirSwitchRPM", 2500);
		movement_wheeled->SetValue("dynFriction", 1.0f);
		movement_wheeled->SetValue("steerTrackNeutralTurn", 0.78500003f);
		movement_wheeled->SetValue("pullTilt", 8.0f);
		movement_wheeled->SetValue("latFriction", 0.5f);
		SmartScriptTable movement_wheeled_gearratios(gEnv->pScriptSystem);
		movement_wheeled_gearratios->PushBack(-12.0f);
		movement_wheeled_gearratios->PushBack(0.0f);
		movement_wheeled_gearratios->PushBack(13.0f);
		movement_wheeled_gearratios->PushBack(10.5f);
		movement_wheeled_gearratios->PushBack(8.5f);
		movement_wheeled_gearratios->PushBack(7.0f);
		movement_wheeled->SetValue("gearRatios", movement_wheeled_gearratios);
		movement->SetValue("Wheeled", movement_wheeled);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSmokeOut");
		movement_soundparams->SetValue("runSoundDelay", 1.3f);
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
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		{
			SmartScriptTable ai_tweakgroup(gEnv->pScriptSystem);
			ai_tweakgroup->SetValue("name", "ai");
			SmartScriptTable ai_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable latFricMin_tweak(gEnv->pScriptSystem);
				latFricMin_tweak->SetValue("name", "latFricMin");
				latFricMin_tweak->SetValue("value", 0.15000001f);
				latFricMin_tweak->SetValue("op", 0);
				ai_tweakgroup_tweaks->PushBack(latFricMin_tweak);
			}
			ai_tweakgroup->SetValue("Tweaks", ai_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(ai_tweakgroup);
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
		seat->SetValue("transitionAnim", true);
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
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -7, 0.5));
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
				else if (m_modName == "TACCannon")
				{
					weapon->SetValue("class", "TACCannon");
				}
				else if (m_modName == "MOAC")
				{
					weapon->SetValue("class", "VehicleMOAC");
				}
				else if (m_modName == "MOAR")
				{
					weapon->SetValue("class", "VehicleMOAR");
				}
				else if (m_modName == "Singularity")
				{
					weapon->SetValue("class", "VehicleSingularity");
				}
				else
				{
					weapon->SetValue("class", "TankCannon");
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
						else if (m_modName == "MOAR")
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
				weapon->SetValue("class", "AsianCoaxialGun");
				weapon->SetValue("part", "cannon");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("coaxOut");
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
		seat->SetValue("name", "gunner");
		seat->SetValue("part", "shitenTurret");
		seat->SetValue("enterHelper", "gunnerEnterPos");
		seat->SetValue("sitHelper", "gunnerPos");
		seat->SetValue("useBoundsForEntering", false);
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "shiten");
		seat->SetValue("isPassengerExposed", true);
		seat->SetValue("seatGroupIndex", 2);
		seat->SetValue("transitionAnim", true);
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
				weapon->SetValue("class", "VehicleShiTenV2");
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

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetValue("Seats", seats);
	}

	// SeatTransitions
	this->InitSeatTransitions();

	// SeatGroups
	{
		SmartScriptTable seatGroup(gEnv->pScriptSystem);
		seatGroup->SetValue("name", "Tank");
		SmartScriptTable seatGroup_seats(gEnv->pScriptSystem);
		seatGroup_seats->PushBack("driver");
		seatGroup_seats->PushBack("gunner");
		seatGroup->SetValue("Seats", seatGroup_seats);

		VehicleSeatGroup* tank = new VehicleSeatGroup();
		tank->Init(this, seatGroup);
		m_seatGroups.push_back(tank);
	}

	// Inventory
	this->SetAmmoCapacity("tank125", 30);
	this->SetAmmoCapacity("tacprojectile", 2);
	this->SetAmmoCapacity("Tank_singularityprojectile", 4);

	// Paints
	if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/asian_tank/asian_tank_nk.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/asian_tank/asian_tank_us.mtl");
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
				activation->SetValue("param2", "mainEnterLeft");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("driver");
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
				activation->SetValue("param2", "mainEnterRight");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("driver");
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
				activation->SetValue("param2", "gunnerEnterTop");
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

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Actions", actions);
		this->InitActions(table);
	}

	// Damages
	{
		SmartScriptTable damages(gEnv->pScriptSystem);
		damages->SetValue("submergedRatioMax", 0.89999998f);
		damages->SetValue("submergedDamageMult", 0.5f);
		damages->SetValue("collDamageThreshold", 600.0f);
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
			damagemultiplier->SetValue("splash", 0.1f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.2f);
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
			damagemultiplier->SetValue("multiplier", 0.0f);
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
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 1.0f);
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
