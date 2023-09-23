#include "Asian_truck.h"

Asian_truck::Asian_truck()
{
	// enable Asian_truck::Init function
	this->Vehicle::PatchVTable();
}

Asian_truck::~Asian_truck()
{
}

bool Asian_truck::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 100.0f;
	m_buoyancy.waterResistance = 50.0f;
	m_buoyancy.waterDamping = 0.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.039999999f;
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
			body_part->SetValue("disablePhysics", false);
			SmartScriptTable body_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable hull_part(gEnv->pScriptSystem);
				hull_part->SetValue("name", "hull");
				hull_part->SetValue("class", "AnimatedJoint");
				hull_part->SetValue("mass", 0.0f);
				SmartScriptTable hull_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable shitenTurret_part(gEnv->pScriptSystem);
					shitenTurret_part->SetValue("name", "shitenTurret");
					shitenTurret_part->SetValue("class", "AnimatedJoint");
					shitenTurret_part->SetValue("mass", 0.0f);
					shitenTurret_part->SetValue("disablePhysics", true);
					shitenTurret_part->SetValue("isHidden", false);
					SmartScriptTable shitenTurret_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable shiten_part(gEnv->pScriptSystem);
						shiten_part->SetValue("name", "shiten");
						shiten_part->SetValue("class", "AnimatedJoint");
						shiten_part->SetValue("component", "Cabin");
						shiten_part->SetValue("mass", 0.0f);
						SmartScriptTable shiten_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable shitenOut_helper(gEnv->pScriptSystem);
							shitenOut_helper->SetValue("name", "shitenOut");
							shitenOut_helper->SetValue("position", Vec3(0.0107, 4.3197999, 3.3543));
							shitenOut_helper->SetValue("direction", Vec3(0, 1, 0));
							shiten_part_helpers->PushBack(shitenOut_helper);
						}
						{
							SmartScriptTable gunnerEyePos_helper(gEnv->pScriptSystem);
							gunnerEyePos_helper->SetValue("name", "gunnerEyePos");
							gunnerEyePos_helper->SetValue("position", Vec3(0.0073000002, 2.5662, 3.6809001));
							gunnerEyePos_helper->SetValue("direction", Vec3(0, 1, 0));
							shiten_part_helpers->PushBack(gunnerEyePos_helper);
						}
						{
							SmartScriptTable gunnerLeftArm_helper(gEnv->pScriptSystem);
							gunnerLeftArm_helper->SetValue("name", "gunnerLeftArm");
							gunnerLeftArm_helper->SetValue("position", Vec3(-0.0425, 2.8096001, 3.3464));
							gunnerLeftArm_helper->SetValue("direction", Vec3(0, 1, 0));
							shiten_part_helpers->PushBack(gunnerLeftArm_helper);
						}
						{
							SmartScriptTable gunnerRightArm_helper(gEnv->pScriptSystem);
							gunnerRightArm_helper->SetValue("name", "gunnerRightArm");
							gunnerRightArm_helper->SetValue("position", Vec3(0.069300003, 2.8079, 3.3439));
							gunnerRightArm_helper->SetValue("direction", Vec3(0, 1, 0));
							shiten_part_helpers->PushBack(gunnerRightArm_helper);
						}
						shiten_part->SetValue("Helpers", shiten_part_helpers);
						SmartScriptTable shiten_part_animatedjoint(gEnv->pScriptSystem);
						SmartScriptTable shiten_part_animatedjoint_rotation(gEnv->pScriptSystem);
						shiten_part_animatedjoint_rotation->SetValue("pitchSpeed", 70.0f);
						shiten_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
						SmartScriptTable shiten_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
						shiten_part_animatedjoint_rotation_pitchlimits->PushBack(-13.0f);
						shiten_part_animatedjoint_rotation_pitchlimits->PushBack(60.0f);
						shiten_part_animatedjoint_rotation->SetValue("pitchLimits", shiten_part_animatedjoint_rotation_pitchlimits);
						shiten_part_animatedjoint->SetValue("Rotation", shiten_part_animatedjoint_rotation);
						shiten_part->SetValue("AnimatedJoint", shiten_part_animatedjoint);
						shitenTurret_part_parts->PushBack(shiten_part);
					}
					shitenTurret_part->SetValue("Parts", shitenTurret_part_parts);
					SmartScriptTable shitenTurret_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable gunner_pos_helper(gEnv->pScriptSystem);
						gunner_pos_helper->SetValue("name", "gunner_pos");
						shitenTurret_part_helpers->PushBack(gunner_pos_helper);
					}
					shitenTurret_part->SetValue("Helpers", shitenTurret_part_helpers);
					SmartScriptTable shitenTurret_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable shitenTurret_part_animatedjoint_rotation(gEnv->pScriptSystem);
					shitenTurret_part_animatedjoint_rotation->SetValue("yawSpeed", 75.0f);
					shitenTurret_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
					SmartScriptTable shitenTurret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
					if (m_modName == "Hardtop")
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(-138.0f);
					}
					else if (m_modName == "Hardtop_MP")
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(-138.0f);
					}
					else if (m_modName == "Spawntruck")
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(-135.0f);
					}
					else
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(0.0f);
					}
					if (m_modName == "Hardtop")
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(138.0f);
					}
					else if (m_modName == "Hardtop_MP")
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(138.0f);
					}
					else if (m_modName == "Spawntruck")
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(135.0f);
					}
					else
					{
						shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(0.0f);
					}
					shitenTurret_part_animatedjoint_rotation->SetValue("yawLimits", shitenTurret_part_animatedjoint_rotation_yawlimits);
					shitenTurret_part_animatedjoint->SetValue("Rotation", shitenTurret_part_animatedjoint_rotation);
					shitenTurret_part->SetValue("AnimatedJoint", shitenTurret_part_animatedjoint);
					hull_part_parts->PushBack(shitenTurret_part);
				}
				{
					SmartScriptTable steeringWheel_part(gEnv->pScriptSystem);
					steeringWheel_part->SetValue("name", "steeringWheel");
					steeringWheel_part->SetValue("class", "AnimatedJoint");
					steeringWheel_part->SetValue("mass", 0.0f);
					SmartScriptTable steeringWheel_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable steeringLeftArm_helper(gEnv->pScriptSystem);
						steeringLeftArm_helper->SetValue("name", "steeringLeftArm");
						steeringLeftArm_helper->SetValue("position", Vec3(-0.83890003, 3.1935999, 2.2235999));
						steeringLeftArm_helper->SetValue("direction", Vec3(0, 1, 0));
						steeringWheel_part_helpers->PushBack(steeringLeftArm_helper);
					}
					{
						SmartScriptTable steeringRightArm_helper(gEnv->pScriptSystem);
						steeringRightArm_helper->SetValue("name", "steeringRightArm");
						steeringRightArm_helper->SetValue("position", Vec3(-0.4438, 3.2258, 2.2188001));
						steeringRightArm_helper->SetValue("direction", Vec3(0, 1, 0));
						steeringWheel_part_helpers->PushBack(steeringRightArm_helper);
					}
					steeringWheel_part->SetValue("Helpers", steeringWheel_part_helpers);
					hull_part_parts->PushBack(steeringWheel_part);
				}
				{
					SmartScriptTable bumper_front_part(gEnv->pScriptSystem);
					bumper_front_part->SetValue("name", "bumper_front");
					bumper_front_part->SetValue("class", "AnimatedJoint");
					bumper_front_part->SetValue("component", "cabin");
					bumper_front_part->SetValue("mass", 65.0f);
					SmartScriptTable bumper_front_part_animatedjoint(gEnv->pScriptSystem);
					bumper_front_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 1, 0.34999999));
					bumper_front_part->SetValue("AnimatedJoint", bumper_front_part_animatedjoint);
					hull_part_parts->PushBack(bumper_front_part);
				}
				hull_part->SetValue("Parts", hull_part_parts);
				SmartScriptTable hull_part_helpers(gEnv->pScriptSystem);
				hull_part->SetValue("Helpers", hull_part_helpers);
				body_part_parts->PushBack(hull_part);
			}
			{
				SmartScriptTable spare_wheel_part(gEnv->pScriptSystem);
				spare_wheel_part->SetValue("name", "spare_wheel");
				spare_wheel_part->SetValue("class", "AnimatedJoint");
				spare_wheel_part->SetValue("component", "sparewheel");
				spare_wheel_part->SetValue("mass", 65.0f);
				SmartScriptTable spare_wheel_part_animatedjoint(gEnv->pScriptSystem);
				spare_wheel_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 1));
				spare_wheel_part_animatedjoint->SetValue("detachProbability", 1.0f);
				spare_wheel_part->SetValue("AnimatedJoint", spare_wheel_part_animatedjoint);
				body_part_parts->PushBack(spare_wheel_part);
			}
			{
				SmartScriptTable door1_part(gEnv->pScriptSystem);
				door1_part->SetValue("name", "door1");
				door1_part->SetValue("class", "AnimatedJoint");
				door1_part->SetValue("component", "Cabin");
				door1_part->SetValue("mass", 100.0f);
				SmartScriptTable door1_part_animatedjoint(gEnv->pScriptSystem);
				door1_part_animatedjoint->SetValue("detachBaseForce", Vec3(-1, 0, 0));
				door1_part->SetValue("AnimatedJoint", door1_part_animatedjoint);
				body_part_parts->PushBack(door1_part);
			}
			{
				SmartScriptTable door2_part(gEnv->pScriptSystem);
				door2_part->SetValue("name", "door2");
				door2_part->SetValue("class", "AnimatedJoint");
				door2_part->SetValue("component", "Cabin");
				door2_part->SetValue("mass", 100.0f);
				SmartScriptTable door2_part_animatedjoint(gEnv->pScriptSystem);
				door2_part_animatedjoint->SetValue("detachBaseForce", Vec3(1, 0, 0));
				door2_part->SetValue("AnimatedJoint", door2_part_animatedjoint);
				body_part_parts->PushBack(door2_part);
			}
			{
				SmartScriptTable backdoor_part(gEnv->pScriptSystem);
				backdoor_part->SetValue("name", "backdoor");
				backdoor_part->SetValue("class", "AnimatedJoint");
				backdoor_part->SetValue("component", "Cabin");
				backdoor_part->SetValue("mass", 100.0f);
				SmartScriptTable backdoor_part_animatedjoint(gEnv->pScriptSystem);
				backdoor_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, -1, 0.34999999));
				backdoor_part->SetValue("AnimatedJoint", backdoor_part_animatedjoint);
				body_part_parts->PushBack(backdoor_part);
			}
			{
				SmartScriptTable dials_speedometer_part(gEnv->pScriptSystem);
				dials_speedometer_part->SetValue("name", "dials_speedometer");
				dials_speedometer_part->SetValue("class", "AnimatedJoint");
				dials_speedometer_part->SetValue("component", "cabin");
				dials_speedometer_part->SetValue("mass", 0.0f);
				SmartScriptTable dials_speedometer_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable dials_speedometer_part_animatedjoint_dials(gEnv->pScriptSystem);
				dials_speedometer_part_animatedjoint_dials->SetValue("rotationMax", 70.0f);
				dials_speedometer_part_animatedjoint_dials->SetValue("ofs", -10.0f);
				dials_speedometer_part_animatedjoint->SetValue("Dials", dials_speedometer_part_animatedjoint_dials);
				dials_speedometer_part->SetValue("AnimatedJoint", dials_speedometer_part_animatedjoint);
				body_part_parts->PushBack(dials_speedometer_part);
			}
			{
				SmartScriptTable dials_revometer_part(gEnv->pScriptSystem);
				dials_revometer_part->SetValue("name", "dials_revometer");
				dials_revometer_part->SetValue("class", "AnimatedJoint");
				dials_revometer_part->SetValue("component", "cabin");
				dials_revometer_part->SetValue("mass", 0.0f);
				SmartScriptTable dials_revometer_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable dials_revometer_part_animatedjoint_dials(gEnv->pScriptSystem);
				dials_revometer_part_animatedjoint_dials->SetValue("rotationMax", 200.0f);
				dials_revometer_part_animatedjoint->SetValue("Dials", dials_revometer_part_animatedjoint_dials);
				dials_revometer_part->SetValue("AnimatedJoint", dials_revometer_part_animatedjoint);
				body_part_parts->PushBack(dials_revometer_part);
			}
			{
				SmartScriptTable bumper_back_part(gEnv->pScriptSystem);
				bumper_back_part->SetValue("name", "bumper_back");
				bumper_back_part->SetValue("class", "AnimatedJoint");
				bumper_back_part->SetValue("component", "rear");
				bumper_back_part->SetValue("mass", 65.0f);
				SmartScriptTable bumper_back_part_animatedjoint(gEnv->pScriptSystem);
				bumper_back_part->SetValue("AnimatedJoint", bumper_back_part_animatedjoint);
				body_part_parts->PushBack(bumper_back_part);
			}
			{
				SmartScriptTable frame_part(gEnv->pScriptSystem);
				frame_part->SetValue("name", "frame");
				frame_part->SetValue("class", "AnimatedJoint");
				frame_part->SetValue("component", "");
				frame_part->SetValue("mass", 0.0f);
				if (m_modName == "Hardtop")
				{
					frame_part->SetValue("useOption", 2);
				}
				else if (m_modName == "Hardtop_MP")
				{
					frame_part->SetValue("useOption", 2);
				}
				else if (m_modName == "Spawntruck")
				{
					frame_part->SetValue("useOption", 3);
				}
				else
				{
					frame_part->SetValue("useOption", 1);
				}
				SmartScriptTable frame_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable radar_part(gEnv->pScriptSystem);
					radar_part->SetValue("name", "radar");
					radar_part->SetValue("class", "AnimatedJoint");
					radar_part->SetValue("component", "rear");
					radar_part->SetValue("mass", 80.0f);
					if (m_modName == "Spawntruck")
					{
						radar_part->SetValue("useOption", 0);
					}
					else
					{
						radar_part->SetValue("useOption", 1);
					}
					SmartScriptTable radar_part_animatedjoint(gEnv->pScriptSystem);
					radar_part->SetValue("AnimatedJoint", radar_part_animatedjoint);
					frame_part_parts->PushBack(radar_part);
				}
				frame_part->SetValue("Parts", frame_part_parts);
				SmartScriptTable frame_part_animatedjoint(gEnv->pScriptSystem);
				frame_part->SetValue("AnimatedJoint", frame_part_animatedjoint);
				body_part_parts->PushBack(frame_part);
			}
			{
				SmartScriptTable wheel_1_part(gEnv->pScriptSystem);
				wheel_1_part->SetValue("name", "wheel_1");
				wheel_1_part->SetValue("class", "SubPartWheel");
				wheel_1_part->SetValue("component", "wheel1");
				wheel_1_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel_1_part_subpartwheel(gEnv->pScriptSystem);
				wheel_1_part_subpartwheel->SetValue("axle", 0);
				wheel_1_part_subpartwheel->SetValue("density", 0.0f);
				wheel_1_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel_1_part_subpartwheel->SetValue("driving", true);
				wheel_1_part_subpartwheel->SetValue("lenMax", 0.44999999f);
				wheel_1_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_1_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_1_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_1_part_subpartwheel->SetValue("suspLength", 0.30000001f);
				wheel_1_part_subpartwheel->SetValue("rimRadius", 0.31f);
				wheel_1_part->SetValue("SubPartWheel", wheel_1_part_subpartwheel);
				body_part_parts->PushBack(wheel_1_part);
			}
			{
				SmartScriptTable wheel_2_part(gEnv->pScriptSystem);
				wheel_2_part->SetValue("name", "wheel_2");
				wheel_2_part->SetValue("class", "SubPartWheel");
				wheel_2_part->SetValue("component", "wheel2");
				wheel_2_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel_2_part_subpartwheel(gEnv->pScriptSystem);
				wheel_2_part_subpartwheel->SetValue("axle", 0);
				wheel_2_part_subpartwheel->SetValue("density", 0.0f);
				wheel_2_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel_2_part_subpartwheel->SetValue("driving", true);
				wheel_2_part_subpartwheel->SetValue("lenMax", 0.44999999f);
				wheel_2_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_2_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_2_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_2_part_subpartwheel->SetValue("suspLength", 0.30000001f);
				wheel_2_part_subpartwheel->SetValue("rimRadius", 0.31f);
				wheel_2_part->SetValue("SubPartWheel", wheel_2_part_subpartwheel);
				body_part_parts->PushBack(wheel_2_part);
			}
			{
				SmartScriptTable wheel_3_part(gEnv->pScriptSystem);
				wheel_3_part->SetValue("name", "wheel_3");
				wheel_3_part->SetValue("class", "SubPartWheel");
				wheel_3_part->SetValue("component", "wheel3");
				wheel_3_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel_3_part_subpartwheel(gEnv->pScriptSystem);
				wheel_3_part_subpartwheel->SetValue("axle", 1);
				wheel_3_part_subpartwheel->SetValue("density", 0.0f);
				wheel_3_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel_3_part_subpartwheel->SetValue("driving", true);
				wheel_3_part_subpartwheel->SetValue("lenMax", 0.44999999f);
				wheel_3_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_3_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_3_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_3_part_subpartwheel->SetValue("suspLength", 0.30000001f);
				wheel_3_part_subpartwheel->SetValue("rimRadius", 0.31f);
				wheel_3_part->SetValue("SubPartWheel", wheel_3_part_subpartwheel);
				body_part_parts->PushBack(wheel_3_part);
			}
			{
				SmartScriptTable wheel_4_part(gEnv->pScriptSystem);
				wheel_4_part->SetValue("name", "wheel_4");
				wheel_4_part->SetValue("class", "SubPartWheel");
				wheel_4_part->SetValue("component", "wheel4");
				wheel_4_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel_4_part_subpartwheel(gEnv->pScriptSystem);
				wheel_4_part_subpartwheel->SetValue("axle", 1);
				wheel_4_part_subpartwheel->SetValue("density", 0.0f);
				wheel_4_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel_4_part_subpartwheel->SetValue("driving", true);
				wheel_4_part_subpartwheel->SetValue("lenMax", 0.44999999f);
				wheel_4_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_4_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_4_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_4_part_subpartwheel->SetValue("suspLength", 0.30000001f);
				wheel_4_part_subpartwheel->SetValue("rimRadius", 0.31f);
				wheel_4_part->SetValue("SubPartWheel", wheel_4_part_subpartwheel);
				body_part_parts->PushBack(wheel_4_part);
			}
			{
				SmartScriptTable wheel_5_part(gEnv->pScriptSystem);
				wheel_5_part->SetValue("name", "wheel_5");
				wheel_5_part->SetValue("class", "SubPartWheel");
				wheel_5_part->SetValue("component", "wheel5");
				wheel_5_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel_5_part_subpartwheel(gEnv->pScriptSystem);
				wheel_5_part_subpartwheel->SetValue("axle", 2);
				wheel_5_part_subpartwheel->SetValue("density", 0.0f);
				wheel_5_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel_5_part_subpartwheel->SetValue("driving", true);
				wheel_5_part_subpartwheel->SetValue("lenMax", 0.44999999f);
				wheel_5_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_5_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_5_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_5_part_subpartwheel->SetValue("suspLength", 0.30000001f);
				wheel_5_part_subpartwheel->SetValue("rimRadius", 0.31f);
				wheel_5_part->SetValue("SubPartWheel", wheel_5_part_subpartwheel);
				body_part_parts->PushBack(wheel_5_part);
			}
			{
				SmartScriptTable wheel_6_part(gEnv->pScriptSystem);
				wheel_6_part->SetValue("name", "wheel_6");
				wheel_6_part->SetValue("class", "SubPartWheel");
				wheel_6_part->SetValue("component", "wheel6");
				wheel_6_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel_6_part_subpartwheel(gEnv->pScriptSystem);
				wheel_6_part_subpartwheel->SetValue("axle", 2);
				wheel_6_part_subpartwheel->SetValue("density", 0.0f);
				wheel_6_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel_6_part_subpartwheel->SetValue("driving", true);
				wheel_6_part_subpartwheel->SetValue("lenMax", 0.44999999f);
				wheel_6_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_6_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_6_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_6_part_subpartwheel->SetValue("suspLength", 0.30000001f);
				wheel_6_part_subpartwheel->SetValue("rimRadius", 0.31f);
				wheel_6_part->SetValue("SubPartWheel", wheel_6_part_subpartwheel);
				body_part_parts->PushBack(wheel_6_part);
			}
			body_part->SetValue("Parts", body_part_parts);
			SmartScriptTable body_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable driverEyePos_helper(gEnv->pScriptSystem);
				driverEyePos_helper->SetValue("name", "driverEyePos");
				driverEyePos_helper->SetValue("position", Vec3(-0.56840003, 2.6752999, 2.6043));
				driverEyePos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(driverEyePos_helper);
			}
			{
				SmartScriptTable passengerEyePos_helper(gEnv->pScriptSystem);
				passengerEyePos_helper->SetValue("name", "passengerEyePos");
				passengerEyePos_helper->SetValue("position", Vec3(0.6189, 2.7012, 2.5996001));
				passengerEyePos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(passengerEyePos_helper);
			}
			{
				SmartScriptTable passenger01_enter_pos_helper(gEnv->pScriptSystem);
				passenger01_enter_pos_helper->SetValue("name", "passenger01_enter_pos");
				passenger01_enter_pos_helper->SetValue("position", Vec3(-2.2914, 3.0153999, 0.2969));
				passenger01_enter_pos_helper->SetValue("direction", Vec3(-1, 0, 0));
				body_part_helpers->PushBack(passenger01_enter_pos_helper);
			}
			{
				SmartScriptTable passenger02_enter_pos_helper(gEnv->pScriptSystem);
				passenger02_enter_pos_helper->SetValue("name", "passenger02_enter_pos");
				passenger02_enter_pos_helper->SetValue("position", Vec3(2.3529999, 2.6842999, 0.3594));
				passenger02_enter_pos_helper->SetValue("direction", Vec3(1, 0, 0));
				body_part_helpers->PushBack(passenger02_enter_pos_helper);
			}
			{
				SmartScriptTable engineSmokeOut_helper(gEnv->pScriptSystem);
				engineSmokeOut_helper->SetValue("name", "engineSmokeOut");
				engineSmokeOut_helper->SetValue("position", Vec3(0.0088, 3.4145999, 1.4936));
				engineSmokeOut_helper->SetValue("direction", Vec3(0, 0.90630001, 0.4226));
				body_part_helpers->PushBack(engineSmokeOut_helper);
			}
			{
				SmartScriptTable exhaustPos_helper(gEnv->pScriptSystem);
				exhaustPos_helper->SetValue("name", "exhaustPos");
				exhaustPos_helper->SetValue("position", Vec3(-0.60210001, 1.752, 3.2491));
				exhaustPos_helper->SetValue("direction", Vec3(0, 0.035, 0.99940002));
				body_part_helpers->PushBack(exhaustPos_helper);
			}
			{
				SmartScriptTable vehicleBurnPos_helper(gEnv->pScriptSystem);
				vehicleBurnPos_helper->SetValue("name", "vehicleBurnPos");
				vehicleBurnPos_helper->SetValue("position", Vec3(0.0063, 1.7764, 0.96350002));
				vehicleBurnPos_helper->SetValue("direction", Vec3(0, 0, 1));
				body_part_helpers->PushBack(vehicleBurnPos_helper);
			}
			{
				SmartScriptTable carriage_0_helper(gEnv->pScriptSystem);
				carriage_0_helper->SetValue("name", "carriage_0");
				carriage_0_helper->SetValue("position", Vec3(-0.076200001, -1.9385, 1.6847));
				carriage_0_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(carriage_0_helper);
			}
			{
				SmartScriptTable burningBack_helper(gEnv->pScriptSystem);
				burningBack_helper->SetValue("name", "burningBack");
				burningBack_helper->SetValue("position", Vec3(-1.2934999, -3.6896999, 1.4819));
				burningBack_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(burningBack_helper);
			}
			{
				SmartScriptTable LeftFuelTankBurning_helper(gEnv->pScriptSystem);
				LeftFuelTankBurning_helper->SetValue("name", "LeftFuelTankBurning");
				LeftFuelTankBurning_helper->SetValue("position", Vec3(-1.3430001, 0.47659999, 1.2359999));
				LeftFuelTankBurning_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(LeftFuelTankBurning_helper);
			}
			{
				SmartScriptTable RightFuelTankBurning_helper(gEnv->pScriptSystem);
				RightFuelTankBurning_helper->SetValue("name", "RightFuelTankBurning");
				RightFuelTankBurning_helper->SetValue("position", Vec3(1.1917, 0.026900001, 1.2297));
				RightFuelTankBurning_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(RightFuelTankBurning_helper);
			}
			{
				SmartScriptTable debugView_helper(gEnv->pScriptSystem);
				debugView_helper->SetValue("name", "debugView");
				debugView_helper->SetValue("position", Vec3(0.0132, -2.9784999, 3.0718999));
				debugView_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(debugView_helper);
			}
			{
				SmartScriptTable damageLightEnginePos_helper(gEnv->pScriptSystem);
				damageLightEnginePos_helper->SetValue("name", "damageLightEnginePos");
				damageLightEnginePos_helper->SetValue("position", Vec3(-0.74830002, 3.5745001, 2.1945));
				damageLightEnginePos_helper->SetValue("direction", Vec3(0, 0.70709997, 0.70709997));
				body_part_helpers->PushBack(damageLightEnginePos_helper);
			}
			{
				SmartScriptTable damageLightVehiclePos_helper(gEnv->pScriptSystem);
				damageLightVehiclePos_helper->SetValue("name", "damageLightVehiclePos");
				damageLightVehiclePos_helper->SetValue("position", Vec3(-0.74900001, 3.5278001, 2.1157999));
				damageLightVehiclePos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(damageLightVehiclePos_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(-0.91769999, 3.885, 1.5517));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(0.94999999, 3.8879001, 1.5403));
				headLightRight_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightRight_helper);
			}
			{
				SmartScriptTable headLightCenter_helper(gEnv->pScriptSystem);
				headLightCenter_helper->SetValue("name", "headLightCenter");
				headLightCenter_helper->SetValue("position", Vec3(-0.0077999998, 3.9663, 1.4867001));
				headLightCenter_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightCenter_helper);
			}
			{
				SmartScriptTable rearLightLeft_helper(gEnv->pScriptSystem);
				rearLightLeft_helper->SetValue("name", "rearLightLeft");
				rearLightLeft_helper->SetValue("position", Vec3(-0.92919999, -4.8933001, 1.2187001));
				rearLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightLeft_helper);
			}
			{
				SmartScriptTable rearLightRight_helper(gEnv->pScriptSystem);
				rearLightRight_helper->SetValue("name", "rearLightRight");
				rearLightRight_helper->SetValue("position", Vec3(0.94700003, -4.8655, 1.2079));
				rearLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightRight_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(0.1067, 0.33539999, 1.1059));
				centerPos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(centerPos_helper);
			}
			body_part->SetValue("Helpers", body_part_helpers);
			SmartScriptTable body_part_animated(gEnv->pScriptSystem);
			body_part_animated->SetValue("filename", "Objects/Vehicles/Asian_Truck_B/Asian_Truck_b.cga");
			body_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/Asian_Truck_B/Asian_Truck_b_damaged.cga");
			body_part->SetValue("Animated", body_part_animated);
			parts->PushBack(body_part);
		}
		{
			SmartScriptTable frontLightLeft_part(gEnv->pScriptSystem);
			frontLightLeft_part->SetValue("name", "frontLightLeft");
			frontLightLeft_part->SetValue("class", "Light");
			frontLightLeft_part->SetValue("component", "frontLightLeft");
			frontLightLeft_part->SetValue("helper", "frontLightLeft");
			parts->PushBack(frontLightLeft_part);
		}
		{
			SmartScriptTable frontLightRight_part(gEnv->pScriptSystem);
			frontLightRight_part->SetValue("name", "frontLightRight");
			frontLightRight_part->SetValue("class", "Light");
			frontLightRight_part->SetValue("component", "frontLightRight");
			frontLightRight_part->SetValue("helper", "frontLightRight");
			parts->PushBack(frontLightRight_part);
		}
		{
			SmartScriptTable massBox_part(gEnv->pScriptSystem);
			massBox_part->SetValue("name", "massBox");
			massBox_part->SetValue("class", "MassBox");
			massBox_part->SetValue("mass", 7200.0f);
			massBox_part->SetValue("position", Vec3(-0.0093, -0.75459999, 1.0181));
			massBox_part->SetValue("disablePhysics", false);
			massBox_part->SetValue("disableCollision", false);
			massBox_part->SetValue("isHidden", false);
			SmartScriptTable massBox_part_helpers(gEnv->pScriptSystem);
			massBox_part->SetValue("Helpers", massBox_part_helpers);
			SmartScriptTable massBox_part_massbox(gEnv->pScriptSystem);
			massBox_part_massbox->SetValue("size", Vec3(1.399, 4.2059999, 0.898));
			massBox_part_massbox->SetValue("drivingOffset", -0.63999999f);
			massBox_part->SetValue("MassBox", massBox_part_massbox);
			parts->PushBack(massBox_part);
		}
		{
			SmartScriptTable headLightCenter_part(gEnv->pScriptSystem);
			headLightCenter_part->SetValue("name", "headLightCenter");
			headLightCenter_part->SetValue("class", "Light");
			headLightCenter_part->SetValue("component", "");
			headLightCenter_part->SetValue("helper", "headLightCenter");
			SmartScriptTable headLightCenter_part_components(gEnv->pScriptSystem);
			headLightCenter_part_components->PushBack("headLightLeft");
			headLightCenter_part_components->PushBack("headLightRight");
			headLightCenter_part->SetValue("Components", headLightCenter_part_components);
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
		{
			SmartScriptTable brakeLightLeft_part(gEnv->pScriptSystem);
			brakeLightLeft_part->SetValue("name", "brakeLightLeft");
			brakeLightLeft_part->SetValue("class", "Light");
			brakeLightLeft_part->SetValue("component", "rearLightsLeft");
			brakeLightLeft_part->SetValue("helper", "rearLightLeft");
			SmartScriptTable brakeLightLeft_part_light(gEnv->pScriptSystem);
			brakeLightLeft_part_light->SetValue("type", "Brake");
			brakeLightLeft_part->SetValue("Light", brakeLightLeft_part_light);
			parts->PushBack(brakeLightLeft_part);
		}
		{
			SmartScriptTable brakeLightRight_part(gEnv->pScriptSystem);
			brakeLightRight_part->SetValue("name", "brakeLightRight");
			brakeLightRight_part->SetValue("class", "Light");
			brakeLightRight_part->SetValue("component", "rearLightsRight");
			brakeLightRight_part->SetValue("helper", "rearLightRight");
			SmartScriptTable brakeLightRight_part_light(gEnv->pScriptSystem);
			brakeLightRight_part_light->SetValue("type", "Brake");
			brakeLightRight_part->SetValue("Light", brakeLightRight_part_light);
			parts->PushBack(brakeLightRight_part);
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
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.35f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 4.0f);
			damagemultiplier->SetValue("splash", 0.7f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
			damagebehavior_indicator_light->SetValue("sound", "sounds/vehicles:vehicle_accessories:warning_beep_wheeled");
			damagebehavior_indicator_light->SetValue("soundRatioMin", 0.75f);
			damagebehavior_indicator_light->SetValue("helper", "damageLightVehiclePos");
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
		component->SetValue("name", "Engine");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.2363, 1.908, 1.0883));
		component->SetValue("maxBound", Vec3(1.3085999, 3.8879001, 2.1849999));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
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
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
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
		component->SetValue("name", "LeftFuelTank");
		component->SetValue("damageMax", 120.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.3496, 0.1899, 0.57489997));
		component->SetValue("maxBound", Vec3(-0.72710001, 1.2339, 1.1664));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.0f);
		component->SetValue("isOnlyDamageableByPlayer", true);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
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
			damagemultiplier->SetValue("damageType", "fire");
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
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
			damagebehavior->SetValue("class", "Impulse");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("damageRatioMax", 2.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", false);
			SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
			damagebehavior_impulse->SetValue("forceMin", 0.1f);
			damagebehavior_impulse->SetValue("forceMax", 0.2f);
			damagebehavior_impulse->SetValue("direction", Vec3(0.050000001, 0.1, 1));
			damagebehavior_impulse->SetValue("momentum", Vec3(0.2, 0, 0));
			damagebehavior_impulse->SetValue("worldSpace", true);
			damagebehavior->SetValue("Impulse", damagebehavior_impulse);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "LeftFuelTankBurning");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* leftFuelTank = new VehicleComponent();
		leftFuelTank->Init(this, component);
		m_components.push_back(leftFuelTank);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "RightFuelTank");
		component->SetValue("damageMax", 120.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0.69239998, 0.2095, 0.58289999));
		component->SetValue("maxBound", Vec3(1.2886, 1.2456, 1.1653));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.0f);
		component->SetValue("isOnlyDamageableByPlayer", true);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
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
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.1f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
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
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.75f);
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
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
			damagebehavior->SetValue("class", "Impulse");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("damageRatioMax", 2.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", false);
			SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
			damagebehavior_impulse->SetValue("forceMin", 0.1f);
			damagebehavior_impulse->SetValue("forceMax", 0.2f);
			damagebehavior_impulse->SetValue("direction", Vec3(0.050000001, 0.1, 1));
			damagebehavior_impulse->SetValue("momentum", Vec3(0.2, 0, 0));
			damagebehavior_impulse->SetValue("worldSpace", true);
			damagebehavior->SetValue("Impulse", damagebehavior_impulse);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "RightFuelTankBurning");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* rightFuelTank = new VehicleComponent();
		rightFuelTank->Init(this, component);
		m_components.push_back(rightFuelTank);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "wheel1");
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
		component->SetValue("name", "sparewheel");
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		component->SetValue("isOnlyDamageableByPlayer", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("splash", 0.75f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* sparewheel = new VehicleComponent();
		sparewheel->Init(this, component);
		m_components.push_back(sparewheel);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "leftBackBench");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.2891001, -4.5331998, 1.7582));
		component->SetValue("maxBound", Vec3(-0.86519998, 0.80320001, 2.1338));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* leftBackBench = new VehicleComponent();
		leftBackBench->Init(this, component);
		m_components.push_back(leftBackBench);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "rightBackBench");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.76709998, -4.5380998, 1.799));
		component->SetValue("maxBound", Vec3(1.1909, 0.90140003, 2.1335001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* rightBackBench = new VehicleComponent();
		rightBackBench->Init(this, component);
		m_components.push_back(rightBackBench);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "roofEntrance");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.8, 2.2, 3));
		component->SetValue("maxBound", Vec3(0.8, 3.5, 3.5));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* roofEntrance = new VehicleComponent();
		roofEntrance->Init(this, component);
		m_components.push_back(roofEntrance);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "headLightLeft");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.0937999, 3.6287, 1.4232));
		component->SetValue("maxBound", Vec3(-0.76169997, 3.8478999, 1.6492));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "LightShot");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* headLightLeft = new VehicleComponent();
		headLightLeft->Init(this, component);
		m_components.push_back(headLightLeft);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "headLightRight");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.77289999, 3.6654999, 1.424));
		component->SetValue("maxBound", Vec3(1.0884, 3.8345001, 1.6645));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "LightShot");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* headLightRight = new VehicleComponent();
		headLightRight->Init(this, component);
		m_components.push_back(headLightRight);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "rearLightsLeft");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.0815001, -5.0049, 0.96719998));
		component->SetValue("maxBound", Vec3(-0.79439998, -4.7852001, 1.1227));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "LightShot");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* rearLightsLeft = new VehicleComponent();
		rearLightsLeft->Init(this, component);
		m_components.push_back(rearLightsLeft);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "rearLightsRight");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.78170002, -4.9713998, 0.9709));
		component->SetValue("maxBound", Vec3(1.0835, -4.8010001, 1.1208));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "LightShot");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* rearLightsRight = new VehicleComponent();
		rearLightsRight->Init(this, component);
		m_components.push_back(rearLightsRight);
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
			damagebehavior_burn->SetValue("selfDamage", 20.0f);
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
		particles_exhaust_enginestart->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke_off");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke_fast");
		particles_exhaust_enginerunning->SetValue("boostEffect", "vehicle_fx.vehicle_exhaust.heavy_boost");
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
		particles_exhaust_enginerunning->SetValue("minPowerCountScale", 0.89999998f);
		particles_exhaust_enginerunning->SetValue("minPowerSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPower", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerSizeScale", 1.1f);
		particles_exhaust_enginerunning->SetValue("maxPowerCountScale", 2.0f);
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
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.truck_explosion.chinese_truck");
			VehicleDestroyed_damageeffect->SetValue("scaleMax", 1.5f);
			VehicleDestroyed_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable LeftFuelTankBurning_damageeffect(gEnv->pScriptSystem);
			LeftFuelTankBurning_damageeffect->SetValue("name", "LeftFuelTankBurning");
			LeftFuelTankBurning_damageeffect->SetValue("helper", "");
			LeftFuelTankBurning_damageeffect->SetValue("effect", "vehicle_fx.chinese_truck.fuel_tank");
			LeftFuelTankBurning_damageeffect->SetValue("scaleMax", 2.0f);
			LeftFuelTankBurning_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			LeftFuelTankBurning_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(LeftFuelTankBurning_damageeffect);
		}
		{
			SmartScriptTable RightFuelTankBurning_damageeffect(gEnv->pScriptSystem);
			RightFuelTankBurning_damageeffect->SetValue("name", "RightFuelTankBurning");
			RightFuelTankBurning_damageeffect->SetValue("helper", "");
			RightFuelTankBurning_damageeffect->SetValue("effect", "vehicle_fx.chinese_truck.fuel_tank");
			RightFuelTankBurning_damageeffect->SetValue("scaleMax", 2.0f);
			RightFuelTankBurning_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			RightFuelTankBurning_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(RightFuelTankBurning_damageeffect);
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
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Truck_engine_smoke_a");
			EngineDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged75_damageeffect(gEnv->pScriptSystem);
			EngineDamaged75_damageeffect->SetValue("name", "EngineDamaged75");
			EngineDamaged75_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Truck_engine_smoke_b");
			EngineDamaged75_damageeffect->SetValue("scaleMax", 2.0f);
			EngineDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged75_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "vehicleBurnPos");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Truck_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 2.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "vehicleBurnPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.Truck_hull_smoke_b");
			HullDamaged75_damageeffect->SetValue("scaleMax", 2.0f);
			HullDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 0.5f);
			environmentlayer->SetValue("minSpeedSizeScale", 0.375f);
			environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			environmentlayer->SetValue("minSpeedSpeedScale", 1.0f);
			environmentlayer->SetValue("maxSpeed", 20.0f);
			environmentlayer->SetValue("maxSpeedSizeScale", 1.53f);
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
					wheels->PushBack(2);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(3);
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
					wheels->PushBack(6);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			environmentlayer_wheels->SetValue("WheelGroups", environmentlayer_wheels_wheelgroups);
			environmentlayer->SetValue("Wheels", environmentlayer_wheels);
			particles_environmentlayers->PushBack(environmentlayer);
		}
		{
			SmartScriptTable rims_environmentlayer(gEnv->pScriptSystem);
			rims_environmentlayer->SetValue("name", "rims");
			rims_environmentlayer->SetValue("minSpeed", 0.30000001f);
			rims_environmentlayer->SetValue("minSpeedSizeScale", 0.25f);
			rims_environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			rims_environmentlayer->SetValue("maxSpeed", 10.0f);
			rims_environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			rims_environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			SmartScriptTable rims_environmentlayer_alignment(gEnv->pScriptSystem);
			rims_environmentlayer_alignment->SetValue("alignGroundHeight", 0.0f);
			rims_environmentlayer_alignment->SetValue("maxHeightSizeScale", 1.0f);
			rims_environmentlayer_alignment->SetValue("maxHeightCountScale", 1.0f);
			rims_environmentlayer_alignment->SetValue("alignToWater", false);
			rims_environmentlayer->SetValue("Alignment", rims_environmentlayer_alignment);
			SmartScriptTable rims_environmentlayer_emitters(gEnv->pScriptSystem);
			rims_environmentlayer->SetValue("Emitters", rims_environmentlayer_emitters);
			SmartScriptTable rims_environmentlayer_wheels(gEnv->pScriptSystem);
			rims_environmentlayer_wheels->SetValue("all", true);
			rims_environmentlayer_wheels->SetValue("allActive", false);
			rims_environmentlayer->SetValue("Wheels", rims_environmentlayer_wheels);
			particles_environmentlayers->PushBack(rims_environmentlayer);
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
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable _state(gEnv->pScriptSystem);
			_state->SetValue("name", "");
			_state->SetValue("animation", "Truck_b_steeringWheel");
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

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("vMaxSteerMax", 20.0f);
		movement->SetValue("steerSpeed", 100.0f);
		movement->SetValue("steerSpeedMin", 130.0f);
		movement->SetValue("steerSpeedScale", 1.0f);
		movement->SetValue("steerSpeedScaleMin", 1.0f);
		movement->SetValue("v0SteerMax", 23.0f);
		movement->SetValue("kvSteerMax", 10.0f);
		movement->SetValue("steerRelaxation", 500.0f);
		movement->SetValue("pedalLimitMax", 0.2f);
		movement->SetValue("rpmInterpSpeed", 4.0f);
		movement->SetValue("rpmRelaxSpeed", 2.0f);
		movement->SetValue("rpmGearShiftSpeed", 4.0f);
		SmartScriptTable movement_wheeled(gEnv->pScriptSystem);
		movement_wheeled->SetValue("axleFriction", 1000);
		movement_wheeled->SetValue("axleFrictionMax", 5500);
		movement_wheeled->SetValue("brakeTorque", 20000);
		movement_wheeled->SetValue("clutchSpeed", 2.5f);
		movement_wheeled->SetValue("damping", 0.1f);
		movement_wheeled->SetValue("engineIdleRPM", 250);
		movement_wheeled->SetValue("engineMaxRPM", 1400);
		movement_wheeled->SetValue("engineMinRPM", 10);
		movement_wheeled->SetValue("enginePower", 650);
		movement_wheeled->SetValue("engineShiftDownRPM", 550);
		movement_wheeled->SetValue("engineShiftUpRPM", 1100);
		movement_wheeled->SetValue("engineStartRPM", 250);
		movement_wheeled->SetValue("stabilizer", 1.0f);
		movement_wheeled->SetValue("maxBrakingFriction", 0.75f);
		movement_wheeled->SetValue("maxSteer", 0.51999998f);
		movement_wheeled->SetValue("maxSpeed", 20.0f);
		movement_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_wheeled->SetValue("minEnergy", 0.039999999f);
		movement_wheeled->SetValue("slipThreshold", 0.1f);
		movement_wheeled->SetValue("suspDampingMin", -0.15000001f);
		movement_wheeled->SetValue("suspDampingMax", -0.75f);
		movement_wheeled->SetValue("gearDirSwitchRPM", 1500);
		movement_wheeled->SetValue("dynFriction", 1.2f);
		SmartScriptTable movement_wheeled_gearratios(gEnv->pScriptSystem);
		movement_wheeled_gearratios->PushBack(-10.0f);
		movement_wheeled_gearratios->PushBack(0.0f);
		movement_wheeled_gearratios->PushBack(6.0f);
		movement_wheeled_gearratios->PushBack(3.0f);
		movement_wheeled->SetValue("gearRatios", movement_wheeled_gearratios);
		movement->SetValue("Wheeled", movement_wheeled);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSmokeOut");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("roadBumpMinSusp", 8.0f);
		movement_soundparams->SetValue("roadBumpMinSpeed", 5.0f);
		movement_soundparams->SetValue("roadBumpIntensity", 0.40000001f);
		movement_soundparams->SetValue("maxSlipSpeed", 12.5f);
		movement_soundparams->SetValue("airbrake", 0.5f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable enginePower_tweak(gEnv->pScriptSystem);
				enginePower_tweak->SetValue("name", "enginePower");
				enginePower_tweak->SetValue("value", 1.1f);
				enginePower_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(enginePower_tweak);
			}
			{
				SmartScriptTable engineMaxRPM_tweak(gEnv->pScriptSystem);
				engineMaxRPM_tweak->SetValue("name", "engineMaxRPM");
				engineMaxRPM_tweak->SetValue("value", 1.1f);
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
			movement_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);

		if (!this->SetMovement("StdWheeled", movement))
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
		seat->SetValue("enterHelper", "passenger01_enter_pos");
		seat->SetValue("sitHelper", "passenger01_sit_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("transitionAnim", true);
		if (m_modName == "MP")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else if (m_modName == "Spawntruck")
		{
			seat->SetValue("remotelyUseActionsFromSeat", "");
		}
		else if (m_modName == "Unarmed")
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
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -110));
			view->SetValue("rotationMax", Vec3(5, 0, 110));
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
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Sound");
			SmartScriptTable seataction_sound(gEnv->pScriptSystem);
			seataction_sound->SetValue("sound", "Sounds/vehicles:asian_truck:horn");
			seataction_sound->SetValue("helper", "engineSmokeOut");
			seataction->SetValue("Sound", seataction_sound);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "SteeringWheel");
			SmartScriptTable seataction_steeringwheel(gEnv->pScriptSystem);
			seataction_steeringwheel->SetValue("vehicleAnimation", "steeringWheel");
			SmartScriptTable seataction_steeringwheel_car(gEnv->pScriptSystem);
			seataction_steeringwheel_car->SetValue("wheelRotationMax", 0.050000001f);
			seataction_steeringwheel->SetValue("Car", seataction_steeringwheel_car);
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
			seataction_lights_lightparts->PushBack("headLightRight");
			seataction_lights_lightparts->PushBack("headLightCenter");
			seataction_lights_lightparts->PushBack("headLightLeftFlare");
			seataction_lights_lightparts->PushBack("headLightRightFlare");
			seataction_lights_lightparts->PushBack("rearLightLeft");
			seataction_lights_lightparts->PushBack("rearLightRight");
			seataction_lights->SetValue("LightParts", seataction_lights_lightparts);
			seataction->SetValue("Lights", seataction_lights);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Lights");
			SmartScriptTable seataction_lights(gEnv->pScriptSystem);
			seataction_lights->SetValue("activation", "brake");
			SmartScriptTable seataction_lights_lightparts(gEnv->pScriptSystem);
			seataction_lights_lightparts->PushBack("brakeLightLeft");
			seataction_lights_lightparts->PushBack("brakeLightRight");
			seataction_lights->SetValue("LightParts", seataction_lights_lightparts);
			seataction->SetValue("Lights", seataction_lights);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 0.60000002f);
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
		seat->SetValue("enterHelper", "passenger02_enter_pos01");
		seat->SetValue("sitHelper", "gunner_pos");
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
			view->SetValue("isAvailableRemotely", true);
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
				if (m_modName == "Unarmed")
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
		seat_sounds->SetValue("mood", 0.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* gunner = new VehicleSeat();
		gunner->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "gunner", gunner });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger02_enter_pos01");
		seat->SetValue("sitHelper", "passenger02_sit_pos01");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -110));
			view->SetValue("rotationMax", Vec3(5, 0, 110));
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
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 0.60000002f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger = new VehicleSeat();
		passenger->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger", passenger });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat1");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_back_left_pos");
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("sitHelper", "passenger_25_pos");
		}
		else
		{
			seat->SetValue("sitHelper", "passenger_04_pos");
		}
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
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
		seat->SetValue("enterHelper", "passenger_back_left_pos");
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("sitHelper", "passenger_23_pos");
		}
		else
		{
			seat->SetValue("sitHelper", "passenger_05_pos");
		}
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
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
		seat->SetValue("enterHelper", "passenger_back_left_pos");
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("sitHelper", "passenger_21_pos");
		}
		else
		{
			seat->SetValue("sitHelper", "passenger_06_pos");
		}
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
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
		seat->SetValue("enterHelper", "passenger_back_left_pos");
		seat->SetValue("sitHelper", "passenger_07_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("transitionAnim", true);
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("locked", true);
		}
		else
		{
			seat->SetValue("locked", false);
		}
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
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
		seat->SetValue("enterHelper", "passenger_back_right_pos");
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("sitHelper", "passenger_24_pos");
		}
		else
		{
			seat->SetValue("sitHelper", "passenger_08_pos");
		}
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
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
		seat->SetValue("enterHelper", "passenger_back_right_pos");
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("sitHelper", "passenger_22_pos");
		}
		else
		{
			seat->SetValue("sitHelper", "passenger_09_pos");
		}
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat6 = new VehicleSeat();
		backseat6->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat6", backseat6 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat7");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_back_right_pos");
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("sitHelper", "passenger_20_pos");
		}
		else
		{
			seat->SetValue("sitHelper", "passenger_10_pos");
		}
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat7 = new VehicleSeat();
		backseat7->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat7", backseat7 });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat8");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_back_right_pos");
		seat->SetValue("sitHelper", "passenger_11_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		seat->SetValue("transitionAnim", true);
		if (m_modName == "Spawntruck")
		{
			seat->SetValue("locked", true);
		}
		else
		{
			seat->SetValue("locked", false);
		}
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(90, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2.5, 0.2));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 1));
			view_actionthirdperson->SetValue("lagSpeed", 4.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-4, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		if (m_modName == "Hardtop")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Hardtop_MP")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else if (m_modName == "Spawntruck")
		{
			seat_sounds->SetValue("mood", 0.3f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.0f);
		}
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* backseat8 = new VehicleSeat();
		backseat8->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "backseat8", backseat8 });
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
		seatGroup->SetValue("name", "Cabin");
		SmartScriptTable seatGroup_seats(gEnv->pScriptSystem);
		seatGroup_seats->PushBack("driver");
		seatGroup_seats->PushBack("gunner");
		seatGroup_seats->PushBack("passenger");
		seatGroup->SetValue("Seats", seatGroup_seats);

		VehicleSeatGroup* cabin = new VehicleSeatGroup();
		cabin->Init(this, seatGroup);
		m_seatGroups.push_back(cabin);
	}

	// Paints
	if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/asian_truck_b/asian_truck_b.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/asian_truck_b/us_truck_b.mtl");
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
				activation->SetValue("param1", "part");
				activation->SetValue("param2", "door1");
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
				activation->SetValue("param1", "part");
				activation->SetValue("param2", "door2");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("passenger");
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
				activation->SetValue("param2", "roofEntrance");
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
				activation->SetValue("param2", "leftBackBench");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat1");
			action_enter_seats->PushBack("backseat3");
			action_enter_seats->PushBack("backseat5");
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
				activation->SetValue("param2", "rightBackBench");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat2");
			action_enter_seats->PushBack("backseat4");
			action_enter_seats->PushBack("backseat6");
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
		damages->SetValue("submergedRatioMax", 0.69999999f);
		damages->SetValue("submergedDamageMult", 1.0f);
		damages->SetValue("collDamageThreshold", 20.0f);
		damages->SetValue("groundCollisionMinMult", 1.3f);
		damages->SetValue("groundCollisionMaxMult", 1.4f);
		damages->SetValue("groundCollisionMinSpeed", 6.0f);
		damages->SetValue("groundCollisionMaxSpeed", 18.0f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.02f);
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
			damagemultiplier->SetValue("multiplier", 0.15000001f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.050000001f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.050000001f);
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
			damagemultiplier->SetValue("multiplier", 0.18f);
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
			damagemultiplier->SetValue("multiplier", 0.2f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		damages->SetValue("DamageMultipliers", damages_damagemultipliers);
		SmartScriptTable damages_damagesgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable Destroy_damagesgroup(gEnv->pScriptSystem);
			Destroy_damagesgroup->SetValue("name", "Destroy");
			SmartScriptTable Destroy_damagesgroup_damagessubgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.40000001f);
				damagessubgroup->SetValue("randomness", 0.5f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Destroy");
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Effect");
					SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
					damagebehavior_effect->SetValue("effect", "VehicleDestroyed");
					damagebehavior->SetValue("Effect", damagebehavior_effect);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Impulse");
					damagebehavior->SetValue("damageRatioMin", 1.0f);
					damagebehavior->SetValue("ignoreVehicleDestruction", true);
					SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
					damagebehavior_impulse->SetValue("forceMin", 2.7f);
					damagebehavior_impulse->SetValue("forceMax", 3.0f);
					damagebehavior_impulse->SetValue("direction", Vec3(0.050000001, 0.1, 1));
					damagebehavior_impulse->SetValue("momentum", Vec3(0.2, 0, 0));
					damagebehavior_impulse->SetValue("worldSpace", true);
					damagebehavior->SetValue("Impulse", damagebehavior_impulse);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "SpawnDebris");
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
				Destroy_damagesgroup_damagessubgroups->PushBack(damagessubgroup);
			}
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.40000001f);
				damagessubgroup->SetValue("randomness", 0.5f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Explosion");
					SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
					damagebehavior_explosion->SetValue("damage", 400.0f);
					damagebehavior_explosion->SetValue("helper", "vehicleBurnPos");
					damagebehavior_explosion->SetValue("pressure", 3000.0f);
					damagebehavior_explosion->SetValue("radius", 10.0f);
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
					damagebehavior_burn->SetValue("helper", "vehicleBurnPos");
					damagebehavior_burn->SetValue("damage", 5.0f);
					damagebehavior_burn->SetValue("selfDamage", 15.0f);
					damagebehavior_burn->SetValue("radius", 3.0f);
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
		{
			SmartScriptTable IndicatorWheels_damagesgroup(gEnv->pScriptSystem);
			IndicatorWheels_damagesgroup->SetValue("name", "IndicatorWheels");
			SmartScriptTable IndicatorWheels_damagesgroup_damagessubgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.0f);
				damagessubgroup->SetValue("randomness", 0.0f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Indicator");
					damagebehavior->SetValue("damageRatioMin", 0.5f);
					damagebehavior->SetValue("damageRatioMax", 1.0f);
					SmartScriptTable damagebehavior_indicator(gEnv->pScriptSystem);
					SmartScriptTable damagebehavior_indicator_light(gEnv->pScriptSystem);
					damagebehavior_indicator_light->SetValue("material", "damagelight_wheels");
					damagebehavior_indicator_light->SetValue("sound", "");
					damagebehavior_indicator->SetValue("Light", damagebehavior_indicator_light);
					damagebehavior->SetValue("Indicator", damagebehavior_indicator);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
				IndicatorWheels_damagesgroup_damagessubgroups->PushBack(damagessubgroup);
			}
			IndicatorWheels_damagesgroup->SetValue("DamagesSubGroups", IndicatorWheels_damagesgroup_damagessubgroups);
			damages_damagesgroups->PushBack(IndicatorWheels_damagesgroup);
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
