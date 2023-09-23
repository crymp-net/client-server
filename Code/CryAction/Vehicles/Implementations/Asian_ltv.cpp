#include "Asian_ltv.h"

Asian_ltv::Asian_ltv()
{
	// enable Asian_ltv::Init function
	this->Vehicle::PatchVTable();
}

Asian_ltv::~Asian_ltv()
{
}

bool Asian_ltv::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 30.0f;
	m_buoyancy.waterResistance = 50.0f;
	m_buoyancy.waterDamping = 0.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.0020000001f;
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
			body_part->SetValue("isHidden", false);
			SmartScriptTable body_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable hull_part(gEnv->pScriptSystem);
				hull_part->SetValue("name", "hull");
				hull_part->SetValue("class", "AnimatedJoint");
				hull_part->SetValue("component", "Hull");
				SmartScriptTable hull_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable steeringwheel_part(gEnv->pScriptSystem);
					steeringwheel_part->SetValue("name", "steeringwheel");
					steeringwheel_part->SetValue("class", "AnimatedJoint");
					steeringwheel_part->SetValue("mass", 0.0f);
					SmartScriptTable steeringwheel_part_helpers(gEnv->pScriptSystem);
					steeringwheel_part->SetValue("Helpers", steeringwheel_part_helpers);
					hull_part_parts->PushBack(steeringwheel_part);
				}
				{
					SmartScriptTable dials_speedometer_part(gEnv->pScriptSystem);
					dials_speedometer_part->SetValue("name", "dials_speedometer");
					dials_speedometer_part->SetValue("class", "AnimatedJoint");
					dials_speedometer_part->SetValue("component", "Hull");
					dials_speedometer_part->SetValue("mass", 0.0f);
					SmartScriptTable dials_speedometer_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable dials_speedometer_part_animatedjoint_dials(gEnv->pScriptSystem);
					dials_speedometer_part_animatedjoint_dials->SetValue("rotationMax", 105.0f);
					dials_speedometer_part_animatedjoint_dials->SetValue("ofs", -24.0f);
					dials_speedometer_part_animatedjoint->SetValue("Dials", dials_speedometer_part_animatedjoint_dials);
					dials_speedometer_part->SetValue("AnimatedJoint", dials_speedometer_part_animatedjoint);
					hull_part_parts->PushBack(dials_speedometer_part);
				}
				{
					SmartScriptTable dials_revometer_part(gEnv->pScriptSystem);
					dials_revometer_part->SetValue("name", "dials_revometer");
					dials_revometer_part->SetValue("class", "AnimatedJoint");
					dials_revometer_part->SetValue("component", "Hull");
					dials_revometer_part->SetValue("mass", 0.0f);
					SmartScriptTable dials_revometer_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable dials_revometer_part_animatedjoint_dials(gEnv->pScriptSystem);
					dials_revometer_part_animatedjoint_dials->SetValue("rotationMax", 220.0f);
					dials_revometer_part_animatedjoint->SetValue("Dials", dials_revometer_part_animatedjoint_dials);
					dials_revometer_part->SetValue("AnimatedJoint", dials_revometer_part_animatedjoint);
					hull_part_parts->PushBack(dials_revometer_part);
				}
				hull_part->SetValue("Parts", hull_part_parts);
				body_part_parts->PushBack(hull_part);
			}
			{
				SmartScriptTable door_left_front_part(gEnv->pScriptSystem);
				door_left_front_part->SetValue("name", "door_left_front");
				door_left_front_part->SetValue("class", "AnimatedJoint");
				door_left_front_part->SetValue("component", "Hull");
				door_left_front_part->SetValue("mass", 50.0f);
				if (m_modName == "Unarmed")
				{
					door_left_front_part->SetValue("useOption", 1);
				}
				else
				{
					door_left_front_part->SetValue("useOption", 0);
				}
				SmartScriptTable door_left_front_part_animatedjoint(gEnv->pScriptSystem);
				door_left_front_part_animatedjoint->SetValue("detachBaseForce", Vec3(-1, 0, 0));
				door_left_front_part->SetValue("AnimatedJoint", door_left_front_part_animatedjoint);
				body_part_parts->PushBack(door_left_front_part);
			}
			{
				SmartScriptTable door_right_front_part(gEnv->pScriptSystem);
				door_right_front_part->SetValue("name", "door_right_front");
				door_right_front_part->SetValue("class", "AnimatedJoint");
				door_right_front_part->SetValue("component", "Hull");
				door_right_front_part->SetValue("mass", 50.0f);
				if (m_modName == "Unarmed")
				{
					door_right_front_part->SetValue("useOption", 1);
				}
				else
				{
					door_right_front_part->SetValue("useOption", 0);
				}
				SmartScriptTable door_right_front_part_animatedjoint(gEnv->pScriptSystem);
				door_right_front_part_animatedjoint->SetValue("detachBaseForce", Vec3(1, 0, 0));
				door_right_front_part->SetValue("AnimatedJoint", door_right_front_part_animatedjoint);
				body_part_parts->PushBack(door_right_front_part);
			}
			{
				SmartScriptTable door_left_rear_part(gEnv->pScriptSystem);
				door_left_rear_part->SetValue("name", "door_left_rear");
				door_left_rear_part->SetValue("class", "AnimatedJoint");
				door_left_rear_part->SetValue("component", "Hull");
				door_left_rear_part->SetValue("mass", 50.0f);
				if (m_modName == "Unarmed")
				{
					door_left_rear_part->SetValue("useOption", 1);
				}
				else
				{
					door_left_rear_part->SetValue("useOption", 0);
				}
				SmartScriptTable door_left_rear_part_animatedjoint(gEnv->pScriptSystem);
				door_left_rear_part_animatedjoint->SetValue("detachBaseForce", Vec3(-1, 0, 0));
				door_left_rear_part->SetValue("AnimatedJoint", door_left_rear_part_animatedjoint);
				body_part_parts->PushBack(door_left_rear_part);
			}
			{
				SmartScriptTable door_right_rear_part(gEnv->pScriptSystem);
				door_right_rear_part->SetValue("name", "door_right_rear");
				door_right_rear_part->SetValue("class", "AnimatedJoint");
				door_right_rear_part->SetValue("component", "Hull");
				door_right_rear_part->SetValue("mass", 50.0f);
				if (m_modName == "Unarmed")
				{
					door_right_rear_part->SetValue("useOption", 1);
				}
				else
				{
					door_right_rear_part->SetValue("useOption", 0);
				}
				SmartScriptTable door_right_rear_part_animatedjoint(gEnv->pScriptSystem);
				door_right_rear_part_animatedjoint->SetValue("detachBaseForce", Vec3(1, 0, 0));
				door_right_rear_part->SetValue("AnimatedJoint", door_right_rear_part_animatedjoint);
				body_part_parts->PushBack(door_right_rear_part);
			}
			{
				SmartScriptTable roof_part_1_part(gEnv->pScriptSystem);
				roof_part_1_part->SetValue("name", "roof_part_1");
				roof_part_1_part->SetValue("class", "AnimatedJoint");
				roof_part_1_part->SetValue("component", "Hull");
				roof_part_1_part->SetValue("mass", 50.0f);
				if (m_modName == "MP")
				{
					roof_part_1_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Unarmed")
				{
					roof_part_1_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Gauss")
				{
					roof_part_1_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAC")
				{
					roof_part_1_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAR")
				{
					roof_part_1_part->SetValue("useOption", 0);
				}
				else
				{
					roof_part_1_part->SetValue("useOption", 1);
				}
				SmartScriptTable roof_part_1_part_animatedjoint(gEnv->pScriptSystem);
				roof_part_1_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 1));
				roof_part_1_part->SetValue("AnimatedJoint", roof_part_1_part_animatedjoint);
				body_part_parts->PushBack(roof_part_1_part);
			}
			{
				SmartScriptTable roof_part_2_part(gEnv->pScriptSystem);
				roof_part_2_part->SetValue("name", "roof_part_2");
				roof_part_2_part->SetValue("class", "AnimatedJoint");
				roof_part_2_part->SetValue("component", "Hull");
				roof_part_2_part->SetValue("mass", 50.0f);
				if (m_modName == "MP")
				{
					roof_part_2_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Unarmed")
				{
					roof_part_2_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Gauss")
				{
					roof_part_2_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAC")
				{
					roof_part_2_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAR")
				{
					roof_part_2_part->SetValue("useOption", 0);
				}
				else
				{
					roof_part_2_part->SetValue("useOption", 1);
				}
				SmartScriptTable roof_part_2_part_animatedjoint(gEnv->pScriptSystem);
				roof_part_2_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 1));
				roof_part_2_part->SetValue("AnimatedJoint", roof_part_2_part_animatedjoint);
				body_part_parts->PushBack(roof_part_2_part);
			}
			{
				SmartScriptTable roof_part_3_part(gEnv->pScriptSystem);
				roof_part_3_part->SetValue("name", "roof_part_3");
				roof_part_3_part->SetValue("class", "AnimatedJoint");
				roof_part_3_part->SetValue("component", "Hull");
				roof_part_3_part->SetValue("mass", 50.0f);
				if (m_modName == "MP")
				{
					roof_part_3_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Unarmed")
				{
					roof_part_3_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Gauss")
				{
					roof_part_3_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAC")
				{
					roof_part_3_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAR")
				{
					roof_part_3_part->SetValue("useOption", 0);
				}
				else
				{
					roof_part_3_part->SetValue("useOption", 1);
				}
				SmartScriptTable roof_part_3_part_animatedjoint(gEnv->pScriptSystem);
				roof_part_3_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 1));
				roof_part_3_part->SetValue("AnimatedJoint", roof_part_3_part_animatedjoint);
				body_part_parts->PushBack(roof_part_3_part);
			}
			{
				SmartScriptTable roof_part_4_part(gEnv->pScriptSystem);
				roof_part_4_part->SetValue("name", "roof_part_4");
				roof_part_4_part->SetValue("class", "AnimatedJoint");
				roof_part_4_part->SetValue("component", "Hull");
				roof_part_4_part->SetValue("mass", 50.0f);
				if (m_modName == "MP")
				{
					roof_part_4_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Unarmed")
				{
					roof_part_4_part->SetValue("useOption", 0);
				}
				else if (m_modName == "Gauss")
				{
					roof_part_4_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAC")
				{
					roof_part_4_part->SetValue("useOption", 0);
				}
				else if (m_modName == "MOAR")
				{
					roof_part_4_part->SetValue("useOption", 0);
				}
				else
				{
					roof_part_4_part->SetValue("useOption", 1);
				}
				SmartScriptTable roof_part_4_part_animatedjoint(gEnv->pScriptSystem);
				roof_part_4_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 1));
				roof_part_4_part->SetValue("AnimatedJoint", roof_part_4_part_animatedjoint);
				body_part_parts->PushBack(roof_part_4_part);
			}
			{
				SmartScriptTable grill_guard_part(gEnv->pScriptSystem);
				grill_guard_part->SetValue("name", "grill_guard");
				grill_guard_part->SetValue("class", "AnimatedJoint");
				grill_guard_part->SetValue("component", "Hull");
				grill_guard_part->SetValue("mass", 50.0f);
				grill_guard_part->SetValue("useOption", 1);
				SmartScriptTable grill_guard_part_animatedjoint(gEnv->pScriptSystem);
				grill_guard_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 1, 0));
				grill_guard_part->SetValue("AnimatedJoint", grill_guard_part_animatedjoint);
				body_part_parts->PushBack(grill_guard_part);
			}
			{
				SmartScriptTable sparewheel_part(gEnv->pScriptSystem);
				sparewheel_part->SetValue("name", "sparewheel");
				sparewheel_part->SetValue("class", "AnimatedJoint");
				sparewheel_part->SetValue("component", "sparewheel");
				sparewheel_part->SetValue("mass", 50.0f);
				SmartScriptTable sparewheel_part_animatedjoint(gEnv->pScriptSystem);
				sparewheel_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, -1, 0));
				sparewheel_part_animatedjoint->SetValue("detachProbability", 1.0f);
				sparewheel_part->SetValue("AnimatedJoint", sparewheel_part_animatedjoint);
				body_part_parts->PushBack(sparewheel_part);
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
						gunnerEyePos_helper->SetValue("position", Vec3(0.0029, -0.054000001, 2.6229999));
						gunnerEyePos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(gunnerEyePos_helper);
					}
					{
						SmartScriptTable shitenOutPos_helper(gEnv->pScriptSystem);
						shitenOutPos_helper->SetValue("name", "shitenOutPos");
						shitenOutPos_helper->SetValue("position", Vec3(-0.0049000001, 1.7822, 2.424));
						shitenOutPos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(shitenOutPos_helper);
					}
					{
						SmartScriptTable searchLightPos_helper(gEnv->pScriptSystem);
						searchLightPos_helper->SetValue("name", "searchLightPos");
						searchLightPos_helper->SetValue("position", Vec3(0.165, 0.5, 2.33));
						searchLightPos_helper->SetValue("direction", Vec3(0, 1, 0));
						shiten_part_helpers->PushBack(searchLightPos_helper);
					}
					shiten_part->SetValue("Helpers", shiten_part_helpers);
					SmartScriptTable shiten_part_animatedjoint(gEnv->pScriptSystem);
					shiten_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 0));
					SmartScriptTable shiten_part_animatedjoint_rotation(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation->SetValue("pitchSpeed", 70.0f);
					shiten_part_animatedjoint_rotation->SetValue("worldSpace", 1.0f);
					SmartScriptTable shiten_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					shiten_part_animatedjoint_rotation_pitchlimits->PushBack(-18.0f);
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
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(0.0f);
				shitenTurret_part_animatedjoint_rotation_yawlimits->PushBack(0.0f);
				shitenTurret_part_animatedjoint_rotation->SetValue("yawLimits", shitenTurret_part_animatedjoint_rotation_yawlimits);
				shitenTurret_part_animatedjoint->SetValue("Rotation", shitenTurret_part_animatedjoint_rotation);
				shitenTurret_part->SetValue("AnimatedJoint", shitenTurret_part_animatedjoint);
				body_part_parts->PushBack(shitenTurret_part);
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
				wheel_1_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel_1_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_1_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_1_part_subpartwheel->SetValue("slipFrictionMod", 0.30000001f);
				wheel_1_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_1_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel_1_part_subpartwheel->SetValue("rimRadius", 0.31999999f);
				wheel_1_part_subpartwheel->SetValue("torqueScale", 1.1f);
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
				wheel_2_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel_2_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_2_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_2_part_subpartwheel->SetValue("slipFrictionMod", 0.30000001f);
				wheel_2_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_2_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel_2_part_subpartwheel->SetValue("rimRadius", 0.31999999f);
				wheel_2_part_subpartwheel->SetValue("torqueScale", 1.1f);
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
				wheel_3_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel_3_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_3_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_3_part_subpartwheel->SetValue("slipFrictionMod", 0.34999999f);
				wheel_3_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_3_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel_3_part_subpartwheel->SetValue("rimRadius", 0.31999999f);
				wheel_3_part_subpartwheel->SetValue("torqueScale", 0.89999998f);
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
				wheel_4_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel_4_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel_4_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel_4_part_subpartwheel->SetValue("slipFrictionMod", 0.34999999f);
				wheel_4_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel_4_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel_4_part_subpartwheel->SetValue("rimRadius", 0.31999999f);
				wheel_4_part_subpartwheel->SetValue("torqueScale", 0.89999998f);
				wheel_4_part->SetValue("SubPartWheel", wheel_4_part_subpartwheel);
				body_part_parts->PushBack(wheel_4_part);
			}
			body_part->SetValue("Parts", body_part_parts);
			SmartScriptTable body_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable driver_eye_pos_helper(gEnv->pScriptSystem);
				driver_eye_pos_helper->SetValue("name", "driver_eye_pos");
				driver_eye_pos_helper->SetValue("position", Vec3(-0.60689998, 0.75, 1.87));
				driver_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(driver_eye_pos_helper);
			}
			{
				SmartScriptTable passenger_eye_pos_helper(gEnv->pScriptSystem);
				passenger_eye_pos_helper->SetValue("name", "passenger_eye_pos");
				passenger_eye_pos_helper->SetValue("position", Vec3(0.68989998, 0.67580003, 1.87));
				passenger_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(passenger_eye_pos_helper);
			}
			{
				SmartScriptTable passenger_sit_pos_helper(gEnv->pScriptSystem);
				passenger_sit_pos_helper->SetValue("name", "passenger_sit_pos");
				passenger_sit_pos_helper->SetValue("position", Vec3(0.78390002, 0.60619998, 0.93699998));
				passenger_sit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(passenger_sit_pos_helper);
			}
			{
				SmartScriptTable driver_sit_pos_helper(gEnv->pScriptSystem);
				driver_sit_pos_helper->SetValue("name", "driver_sit_pos");
				driver_sit_pos_helper->SetValue("position", Vec3(-0.66769999, 0.68409997, 0.95899999));
				driver_sit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(driver_sit_pos_helper);
			}
			{
				SmartScriptTable driver_exit_pos_helper(gEnv->pScriptSystem);
				driver_exit_pos_helper->SetValue("name", "driver_exit_pos");
				driver_exit_pos_helper->SetValue("position", Vec3(-2.1470001, 0.47119999, 0.815));
				driver_exit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(driver_exit_pos_helper);
			}
			{
				SmartScriptTable passenger_exit_pos_helper(gEnv->pScriptSystem);
				passenger_exit_pos_helper->SetValue("name", "passenger_exit_pos");
				passenger_exit_pos_helper->SetValue("position", Vec3(2.1001, 0.49509999, 0.75599998));
				passenger_exit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(passenger_exit_pos_helper);
			}
			{
				SmartScriptTable engineSmokeOut_helper(gEnv->pScriptSystem);
				engineSmokeOut_helper->SetValue("name", "engineSmokeOut");
				engineSmokeOut_helper->SetValue("position", Vec3(0.034200002, 2.6040001, 1.087));
				engineSmokeOut_helper->SetValue("direction", Vec3(0, 0, 1));
				body_part_helpers->PushBack(engineSmokeOut_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, 0.1355, 0.9558));
				burningPos_helper->SetValue("direction", Vec3(0, 0, 1));
				body_part_helpers->PushBack(burningPos_helper);
			}
			{
				SmartScriptTable exhaust_pos_left_helper(gEnv->pScriptSystem);
				exhaust_pos_left_helper->SetValue("name", "exhaust_pos_left");
				exhaust_pos_left_helper->SetValue("position", Vec3(-0.79100001, -2.3999, 0.83099997));
				exhaust_pos_left_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(exhaust_pos_left_helper);
			}
			{
				SmartScriptTable exhaust_pos_right_helper(gEnv->pScriptSystem);
				exhaust_pos_right_helper->SetValue("name", "exhaust_pos_right");
				exhaust_pos_right_helper->SetValue("position", Vec3(0.79100001, -2.3999, 0.83099997));
				exhaust_pos_right_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(exhaust_pos_right_helper);
			}
			{
				SmartScriptTable right_rear_exit_pos_helper(gEnv->pScriptSystem);
				right_rear_exit_pos_helper->SetValue("name", "right_rear_exit_pos");
				right_rear_exit_pos_helper->SetValue("position", Vec3(-2.1698999, -0.61400002, 0.78100002));
				right_rear_exit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(right_rear_exit_pos_helper);
			}
			{
				SmartScriptTable left_rear_exit_pos_helper(gEnv->pScriptSystem);
				left_rear_exit_pos_helper->SetValue("name", "left_rear_exit_pos");
				left_rear_exit_pos_helper->SetValue("position", Vec3(1.9683, -0.73000002, 0.745));
				left_rear_exit_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(left_rear_exit_pos_helper);
			}
			{
				SmartScriptTable right_rear_eye_pos_helper(gEnv->pScriptSystem);
				right_rear_eye_pos_helper->SetValue("name", "right_rear_eye_pos");
				right_rear_eye_pos_helper->SetValue("position", Vec3(0.72610003, -0.33500001, 1.824));
				right_rear_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(right_rear_eye_pos_helper);
			}
			{
				SmartScriptTable left_rear_eye_pos_helper(gEnv->pScriptSystem);
				left_rear_eye_pos_helper->SetValue("name", "left_rear_eye_pos");
				left_rear_eye_pos_helper->SetValue("position", Vec3(-0.62989998, -0.32010001, 1.8));
				left_rear_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(left_rear_eye_pos_helper);
			}
			{
				SmartScriptTable fuelCanPos_helper(gEnv->pScriptSystem);
				fuelCanPos_helper->SetValue("name", "fuelCanPos");
				fuelCanPos_helper->SetValue("position", Vec3(-0.49290001, -2.3717999, 1.251));
				fuelCanPos_helper->SetValue("direction", Vec3(0, 0, 1));
				body_part_helpers->PushBack(fuelCanPos_helper);
			}
			{
				SmartScriptTable dashboardPos_helper(gEnv->pScriptSystem);
				dashboardPos_helper->SetValue("name", "dashboardPos");
				dashboardPos_helper->SetValue("position", Vec3(-0.71880001, 1.321, 1.506));
				dashboardPos_helper->SetValue("direction", Vec3(0, 0, 1));
				body_part_helpers->PushBack(dashboardPos_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(-0.56230003, 3.2509999, 1.1289999));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(0.60229999, 3.2439001, 1.1332));
				headLightRight_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightRight_helper);
			}
			{
				SmartScriptTable headLightCenter_helper(gEnv->pScriptSystem);
				headLightCenter_helper->SetValue("name", "headLightCenter");
				headLightCenter_helper->SetValue("position", Vec3(-0.017100001, 3.3276, 1.148));
				headLightCenter_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightCenter_helper);
			}
			{
				SmartScriptTable brakeLightLeft_helper(gEnv->pScriptSystem);
				brakeLightLeft_helper->SetValue("name", "brakeLightLeft");
				brakeLightLeft_helper->SetValue("position", Vec3(-1.0369, -2.3141999, 1.1487));
				brakeLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(brakeLightLeft_helper);
			}
			{
				SmartScriptTable brakeLightRight_helper(gEnv->pScriptSystem);
				brakeLightRight_helper->SetValue("name", "brakeLightRight");
				brakeLightRight_helper->SetValue("position", Vec3(1.0325, -2.3069, 1.1496));
				brakeLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(brakeLightRight_helper);
			}
			{
				SmartScriptTable rearLightLeft_helper(gEnv->pScriptSystem);
				rearLightLeft_helper->SetValue("name", "rearLightLeft");
				rearLightLeft_helper->SetValue("position", Vec3(-0.93040001, -2.3145001, 1.1487));
				rearLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightLeft_helper);
			}
			{
				SmartScriptTable rearLightRight_helper(gEnv->pScriptSystem);
				rearLightRight_helper->SetValue("name", "rearLightRight");
				rearLightRight_helper->SetValue("position", Vec3(0.92629999, -2.3069, 1.1496));
				rearLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightRight_helper);
			}
			{
				SmartScriptTable centerPos_helper(gEnv->pScriptSystem);
				centerPos_helper->SetValue("name", "centerPos");
				centerPos_helper->SetValue("position", Vec3(-0.0383, 0.2297, 0.90899998));
				centerPos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(centerPos_helper);
			}
			body_part->SetValue("Helpers", body_part_helpers);
			SmartScriptTable body_part_animated(gEnv->pScriptSystem);
			body_part_animated->SetValue("filename", "Objects/Vehicles/ltv/ltv.cga");
			body_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/ltv/ltv_damaged.cga");
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
			massBox_part->SetValue("mass", 1500.0f);
			massBox_part->SetValue("position", Vec3(0, 0.28490001, 1));
			massBox_part->SetValue("disablePhysics", false);
			massBox_part->SetValue("disableCollision", false);
			massBox_part->SetValue("isHidden", false);
			SmartScriptTable massBox_part_helpers(gEnv->pScriptSystem);
			massBox_part->SetValue("Helpers", massBox_part_helpers);
			SmartScriptTable massBox_part_massbox(gEnv->pScriptSystem);
			massBox_part_massbox->SetValue("size", Vec3(1.25, 2, 1));
			massBox_part_massbox->SetValue("drivingOffset", -0.69999999f);
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
			rearLightLeft_part->SetValue("component", "rearLightLeft");
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
			rearLightRight_part->SetValue("component", "rearLightRight");
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
			brakeLightLeft_part->SetValue("component", "rearLightLeft");
			brakeLightLeft_part->SetValue("helper", "brakeLightLeft");
			SmartScriptTable brakeLightLeft_part_light(gEnv->pScriptSystem);
			brakeLightLeft_part_light->SetValue("type", "Brake");
			brakeLightLeft_part->SetValue("Light", brakeLightLeft_part_light);
			parts->PushBack(brakeLightLeft_part);
		}
		{
			SmartScriptTable brakeLightRight_part(gEnv->pScriptSystem);
			brakeLightRight_part->SetValue("name", "brakeLightRight");
			brakeLightRight_part->SetValue("class", "Light");
			brakeLightRight_part->SetValue("component", "rearLightRight");
			brakeLightRight_part->SetValue("helper", "brakeLightRight");
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
		component->SetValue("damageMax", 180.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("useDamageLevels", false);
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
			damagebehavior->SetValue("damageRatioMin", 0.60000002f);
			SmartScriptTable damagebehavior_indicator(gEnv->pScriptSystem);
			SmartScriptTable damagebehavior_indicator_light(gEnv->pScriptSystem);
			damagebehavior_indicator_light->SetValue("material", "damagelight_chassis");
			damagebehavior_indicator_light->SetValue("sound", "sounds/vehicles:vehicle_accessories:warning_beep_wheeled");
			damagebehavior_indicator_light->SetValue("soundRatioMin", 0.75f);
			damagebehavior_indicator_light->SetValue("helper", "dashboardPos");
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
		component->SetValue("name", "Engine");
		component->SetValue("damageMax", 160.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.2219, 1.7659, 0.56400001));
		component->SetValue("maxBound", Vec3(1.1921, 3.35, 1.681));
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
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			damagebehavior->SetValue("damageRatioMax", 0.75f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "EngineDamaged50");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
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
			damagebehavior->SetValue("class", "Indicator");
			damagebehavior->SetValue("damageRatioMin", 0.75f);
			SmartScriptTable damagebehavior_indicator(gEnv->pScriptSystem);
			SmartScriptTable damagebehavior_indicator_light(gEnv->pScriptSystem);
			damagebehavior_indicator_light->SetValue("material", "damagelight_engine");
			damagebehavior_indicator_light->SetValue("sound", "");
			damagebehavior_indicator->SetValue("Light", damagebehavior_indicator_light);
			damagebehavior->SetValue("Indicator", damagebehavior_indicator);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			damagebehavior->SetValue("damageRatioMax", 0.5f);
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
		component->SetValue("name", "wheel1");
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		component->SetValue("isOnlyDamageableByPlayer", true);
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
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		component->SetValue("isOnlyDamageableByPlayer", true);
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
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		component->SetValue("isOnlyDamageableByPlayer", true);
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
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
		component->SetValue("hullAffection", 0.0f);
		component->SetValue("isOnlyDamageableByPlayer", true);
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
		component->SetValue("name", "sparewheel");
		component->SetValue("damageMax", 100.0f);
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
		component->SetValue("name", "headLightLeft");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.76389998, 3.0808001, 1.0501));
		component->SetValue("maxBound", Vec3(-0.40259999, 3.302, 1.2343));
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
		component->SetValue("minBound", Vec3(0.41409999, 3.1698999, 1.0426));
		component->SetValue("maxBound", Vec3(0.75779998, 3.3408, 1.2356));
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
		component->SetValue("name", "rearLightLeft");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.1332999, -2.3766999, 1.0627));
		component->SetValue("maxBound", Vec3(-0.8452, -2.1554999, 1.2153));
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

		VehicleComponent* rearLightLeft = new VehicleComponent();
		rearLightLeft->Init(this, component);
		m_components.push_back(rearLightLeft);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "rearLightRight");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.83420002, -2.3506, 1.0621001));
		component->SetValue("maxBound", Vec3(1.1252, -2.1796999, 1.2219));
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

		VehicleComponent* rearLightRight = new VehicleComponent();
		rearLightRight->Init(this, component);
		m_components.push_back(rearLightRight);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "FuelCan");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-0.6577, -2.5309999, 1.0109));
		component->SetValue("maxBound", Vec3(-0.28029999, -2.2478001, 1.5048));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		component->SetValue("isOnlyDamageableByPlayer", true);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 1.5f);
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
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.0f);
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
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 0.0099999998f);
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
			damagebehavior->SetValue("class", "Impulse");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("damageRatioMax", 2.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
			damagebehavior_impulse->SetValue("forceMin", 0.75f);
			damagebehavior_impulse->SetValue("forceMax", 1.0f);
			damagebehavior_impulse->SetValue("direction", Vec3(0, 0, 1));
			damagebehavior_impulse->SetValue("momentum", Vec3(-0.5, 0.5, 0.2));
			damagebehavior->SetValue("Impulse", damagebehavior_impulse);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "fuelCanBurning");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Burn");
			SmartScriptTable damagebehavior_burn(gEnv->pScriptSystem);
			damagebehavior_burn->SetValue("helper", "fuelCanPos");
			damagebehavior_burn->SetValue("damage", 5.0f);
			damagebehavior_burn->SetValue("selfDamage", 20.0f);
			damagebehavior_burn->SetValue("radius", 2.5f);
			damagebehavior_burn->SetValue("interval", 0.5f);
			damagebehavior->SetValue("Burn", damagebehavior_burn);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* fuelCan = new VehicleComponent();
		fuelCan->Init(this, component);
		m_components.push_back(fuelCan);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterLeftFront");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.2434, 0.32570001, 0.60290003));
		component->SetValue("maxBound", Vec3(-0.98900002, 1.3228, 2.0218));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterLeftFront = new VehicleComponent();
		enterLeftFront->Init(this, component);
		m_components.push_back(enterLeftFront);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterRightFront");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.9817, 0.3303, 0.60290003));
		component->SetValue("maxBound", Vec3(1.2361, 1.3601, 2.0218));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterRightFront = new VehicleComponent();
		enterRightFront->Init(this, component);
		m_components.push_back(enterRightFront);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterLeftRear");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.2383, -0.67070001, 0.60290003));
		component->SetValue("maxBound", Vec3(-0.98390001, 0.1653, 2.0218));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterLeftRear = new VehicleComponent();
		enterLeftRear->Init(this, component);
		m_components.push_back(enterLeftRear);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterRightRear");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.94950002, -0.67070001, 0.60290003));
		component->SetValue("maxBound", Vec3(1.2039, 0.1653, 2.0218));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterRightRear = new VehicleComponent();
		enterRightRear->Init(this, component);
		m_components.push_back(enterRightRear);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "EnterTop");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.58200002, -0.87550002, 2.0553));
		component->SetValue("maxBound", Vec3(0.57370001, 0.7881, 2.7902));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterTop = new VehicleComponent();
		enterTop->Init(this, component);
		m_components.push_back(enterTop);
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
			damagebehavior->SetValue("damageRatioMin", 0.0f);
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
			damagebehavior_burn->SetValue("selfDamage", 15.0f);
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
		particles_exhaust_helpers->PushBack("exhaust_pos_left");
		particles_exhaust_helpers->PushBack("exhaust_pos_right");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke_dark");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke_white");
		particles_exhaust_enginerunning->SetValue("boostEffect", "vehicle_fx.vehicle_exhaust.boost");
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
		particles_exhaust_enginerunning->SetValue("minPowerSizeScale", 0.5f);
		particles_exhaust_enginerunning->SetValue("minPowerCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPowerSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPower", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerSizeScale", 1.5f);
		particles_exhaust_enginerunning->SetValue("maxPowerCountScale", 1.5f);
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
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.LTV_engine_smoke_a");
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged75_damageeffect(gEnv->pScriptSystem);
			EngineDamaged75_damageeffect->SetValue("name", "EngineDamaged75");
			EngineDamaged75_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.LTV_engine_smoke_b");
			particles_damageeffects->PushBack(EngineDamaged75_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "burningPos");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.LTV_hull_smoke_a");
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.LTV_hull_smoke_b");
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "burningPos");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.jeep_explosion.a");
			VehicleDestroyed_damageeffect->SetValue("scaleMax", 1.5f);
			VehicleDestroyed_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable fuelCanBurning_damageeffect(gEnv->pScriptSystem);
			fuelCanBurning_damageeffect->SetValue("name", "fuelCanBurning");
			fuelCanBurning_damageeffect->SetValue("helper", "fuelCanPos");
			fuelCanBurning_damageeffect->SetValue("effect", "vehicle_fx.chinese_truck.fuel_tank_jeep");
			fuelCanBurning_damageeffect->SetValue("scaleMax", 2.0f);
			fuelCanBurning_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			fuelCanBurning_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(fuelCanBurning_damageeffect);
		}
		{
			SmartScriptTable VehicleFlipped_damageeffect(gEnv->pScriptSystem);
			VehicleFlipped_damageeffect->SetValue("name", "VehicleFlipped");
			VehicleFlipped_damageeffect->SetValue("helper", "centerPos");
			VehicleFlipped_damageeffect->SetValue("effect", "smoke_and_fire.Jeep.flipped_jeep");
			particles_damageeffects->PushBack(VehicleFlipped_damageeffect);
		}
		{
			SmartScriptTable LightShot_damageeffect(gEnv->pScriptSystem);
			LightShot_damageeffect->SetValue("name", "LightShot");
			LightShot_damageeffect->SetValue("helper", "");
			LightShot_damageeffect->SetValue("effect", "explosions.light.jeep_lights");
			particles_damageeffects->PushBack(LightShot_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged25_damageeffect(gEnv->pScriptSystem);
			EngineDamaged25_damageeffect->SetValue("name", "EngineDamaged25");
			EngineDamaged25_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Land.LTV_engine_smoke_a");
			EngineDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable _environmentlayer(gEnv->pScriptSystem);
			_environmentlayer->SetValue("name", "");
			_environmentlayer->SetValue("minSpeed", 0.5f);
			_environmentlayer->SetValue("minSpeedSizeScale", 1.0f);
			_environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			_environmentlayer->SetValue("minSpeedSpeedScale", 1.0f);
			_environmentlayer->SetValue("maxSpeed", 25.0f);
			_environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			_environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			_environmentlayer->SetValue("maxSpeedSpeedScale", 1.0f);
			SmartScriptTable _environmentlayer_alignment(gEnv->pScriptSystem);
			_environmentlayer_alignment->SetValue("alignGroundHeight", 0.0f);
			_environmentlayer_alignment->SetValue("maxHeightSizeScale", 1.0f);
			_environmentlayer_alignment->SetValue("maxHeightCountScale", 1.0f);
			_environmentlayer_alignment->SetValue("alignToWater", false);
			_environmentlayer->SetValue("Alignment", _environmentlayer_alignment);
			SmartScriptTable _environmentlayer_emitters(gEnv->pScriptSystem);
			_environmentlayer->SetValue("Emitters", _environmentlayer_emitters);
			SmartScriptTable _environmentlayer_wheels(gEnv->pScriptSystem);
			SmartScriptTable _environmentlayer_wheels_wheelgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(3);
					wheelgroup->PushBack(wheels);
				}
				_environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(4);
					wheelgroup->PushBack(wheels);
				}
				_environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			_environmentlayer_wheels->SetValue("WheelGroups", _environmentlayer_wheels_wheelgroups);
			_environmentlayer->SetValue("Wheels", _environmentlayer_wheels);
			particles_environmentlayers->PushBack(_environmentlayer);
		}
		{
			SmartScriptTable slip_environmentlayer(gEnv->pScriptSystem);
			slip_environmentlayer->SetValue("name", "slip");
			slip_environmentlayer->SetValue("minSpeed", 1.0f);
			slip_environmentlayer->SetValue("minSpeedSizeScale", 0.80000001f);
			slip_environmentlayer->SetValue("minSpeedCountScale", 0.5f);
			slip_environmentlayer->SetValue("maxSpeed", 7.0f);
			slip_environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			slip_environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			SmartScriptTable slip_environmentlayer_alignment(gEnv->pScriptSystem);
			slip_environmentlayer_alignment->SetValue("alignGroundHeight", 0.0f);
			slip_environmentlayer_alignment->SetValue("maxHeightSizeScale", 1.0f);
			slip_environmentlayer_alignment->SetValue("maxHeightCountScale", 1.0f);
			slip_environmentlayer_alignment->SetValue("alignToWater", false);
			slip_environmentlayer->SetValue("Alignment", slip_environmentlayer_alignment);
			SmartScriptTable slip_environmentlayer_emitters(gEnv->pScriptSystem);
			slip_environmentlayer->SetValue("Emitters", slip_environmentlayer_emitters);
			SmartScriptTable slip_environmentlayer_wheels(gEnv->pScriptSystem);
			SmartScriptTable slip_environmentlayer_wheels_wheelgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(1);
					wheelgroup->PushBack(wheels);
				}
				slip_environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(2);
					wheelgroup->PushBack(wheels);
				}
				slip_environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(3);
					wheelgroup->PushBack(wheels);
				}
				slip_environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			{
				SmartScriptTable wheelgroup(gEnv->pScriptSystem);
				{
					SmartScriptTable wheels(gEnv->pScriptSystem);
					wheels->PushBack(4);
					wheelgroup->PushBack(wheels);
				}
				slip_environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			slip_environmentlayer_wheels->SetValue("WheelGroups", slip_environmentlayer_wheels_wheelgroups);
			slip_environmentlayer->SetValue("Wheels", slip_environmentlayer_wheels);
			particles_environmentlayers->PushBack(slip_environmentlayer);
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
			_state->SetValue("animation", "steeringWheel");
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
		movement->SetValue("steerSpeed", 70.0f);
		movement->SetValue("steerSpeedMin", 100.0f);
		movement->SetValue("steerSpeedScale", 1.0f);
		movement->SetValue("steerSpeedScaleMin", 1.0f);
		movement->SetValue("v0SteerMax", 33.0f);
		movement->SetValue("kvSteerMax", 4.0f);
		movement->SetValue("steerRelaxation", 500.0f);
		movement->SetValue("pedalLimitMax", 0.1f);
		movement->SetValue("rpmInterpSpeed", 4.0f);
		movement->SetValue("rpmRelaxSpeed", 2.0f);
		movement->SetValue("rpmGearShiftSpeed", 3.0f);
		SmartScriptTable movement_wheeled(gEnv->pScriptSystem);
		movement_wheeled->SetValue("axleFriction", 700);
		movement_wheeled->SetValue("axleFrictionMax", 2000);
		movement_wheeled->SetValue("brakeTorque", 5000);
		movement_wheeled->SetValue("brakeImpulse", 0.5f);
		movement_wheeled->SetValue("clutchSpeed", 2.3f);
		movement_wheeled->SetValue("damping", 0.02f);
		movement_wheeled->SetValue("engineIdleRPM", 500);
		movement_wheeled->SetValue("engineMaxRPM", 3050);
		movement_wheeled->SetValue("engineMinRPM", 100);
		movement_wheeled->SetValue("enginePower", 160);
		movement_wheeled->SetValue("engineShiftDownRPM", 1300);
		movement_wheeled->SetValue("engineShiftUpRPM", 2000);
		movement_wheeled->SetValue("engineStartRPM", 300);
		movement_wheeled->SetValue("stabilizer", 0.2f);
		movement_wheeled->SetValue("maxBrakingFriction", 0.60000002f);
		movement_wheeled->SetValue("rearWheelBrakingFrictionMult", 0.65f);
		movement_wheeled->SetValue("maxSteer", 0.44999999f);
		movement_wheeled->SetValue("maxSpeed", 23.0f);
		movement_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_wheeled->SetValue("minEnergy", 0.01f);
		movement_wheeled->SetValue("slipThreshold", 0.02f);
		movement_wheeled->SetValue("suspDampingMin", -0.18000001f);
		movement_wheeled->SetValue("suspDampingMax", -0.40000001f);
		movement_wheeled->SetValue("suspDampingMaxSpeed", 15.0f);
		movement_wheeled->SetValue("gearDirSwitchRPM", 1000);
		movement_wheeled->SetValue("dynFriction", 1.1f);
		SmartScriptTable movement_wheeled_gearratios(gEnv->pScriptSystem);
		movement_wheeled_gearratios->PushBack(-20.0f);
		movement_wheeled_gearratios->PushBack(0.0f);
		movement_wheeled_gearratios->PushBack(8.5f);
		movement_wheeled_gearratios->PushBack(6.0f);
		movement_wheeled_gearratios->PushBack(5.5f);
		movement_wheeled->SetValue("gearRatios", movement_wheeled_gearratios);
		movement->SetValue("Wheeled", movement_wheeled);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSmokeOut");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("roadBumpMinSusp", 10.0f);
		movement_soundparams->SetValue("roadBumpMinSpeed", 6.0f);
		movement_soundparams->SetValue("roadBumpIntensity", 0.30000001f);
		movement_soundparams->SetValue("maxSlipSpeed", 11.0f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable ai_tweakgroup(gEnv->pScriptSystem);
			ai_tweakgroup->SetValue("name", "ai");
			SmartScriptTable ai_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable enginePower_tweak(gEnv->pScriptSystem);
				enginePower_tweak->SetValue("name", "enginePower");
				enginePower_tweak->SetValue("value", 1.0f);
				enginePower_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(enginePower_tweak);
			}
			{
				SmartScriptTable engineMaxRPM_tweak(gEnv->pScriptSystem);
				engineMaxRPM_tweak->SetValue("name", "engineMaxRPM");
				engineMaxRPM_tweak->SetValue("value", 1.0f);
				engineMaxRPM_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(engineMaxRPM_tweak);
			}
			{
				SmartScriptTable brakeTorque_tweak(gEnv->pScriptSystem);
				brakeTorque_tweak->SetValue("name", "brakeTorque");
				brakeTorque_tweak->SetValue("value", 1.0f);
				brakeTorque_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(brakeTorque_tweak);
			}
			{
				SmartScriptTable pedalLimitMax_tweak(gEnv->pScriptSystem);
				pedalLimitMax_tweak->SetValue("name", "pedalLimitMax");
				pedalLimitMax_tweak->SetValue("value", 1.0f);
				pedalLimitMax_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(pedalLimitMax_tweak);
			}
			{
				SmartScriptTable dynFriction_tweak(gEnv->pScriptSystem);
				dynFriction_tweak->SetValue("name", "dynFriction");
				dynFriction_tweak->SetValue("value", 1.0f);
				dynFriction_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(dynFriction_tweak);
			}
			ai_tweakgroup->SetValue("Tweaks", ai_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(ai_tweakgroup);
		}
		{
			SmartScriptTable player_boost_tweakgroup(gEnv->pScriptSystem);
			player_boost_tweakgroup->SetValue("name", "player_boost");
			SmartScriptTable player_boost_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable enginePower_tweak(gEnv->pScriptSystem);
				enginePower_tweak->SetValue("name", "enginePower");
				enginePower_tweak->SetValue("value", 1.4f);
				enginePower_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(enginePower_tweak);
			}
			{
				SmartScriptTable engineMaxRPM_tweak(gEnv->pScriptSystem);
				engineMaxRPM_tweak->SetValue("name", "engineMaxRPM");
				engineMaxRPM_tweak->SetValue("value", 1.25f);
				engineMaxRPM_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(engineMaxRPM_tweak);
			}
			{
				SmartScriptTable axleFriction_tweak(gEnv->pScriptSystem);
				axleFriction_tweak->SetValue("name", "axleFriction");
				axleFriction_tweak->SetValue("value", 0.75f);
				axleFriction_tweak->SetValue("op", 1);
				player_boost_tweakgroup_tweaks->PushBack(axleFriction_tweak);
			}
			player_boost_tweakgroup->SetValue("Tweaks", player_boost_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_boost_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);
		SmartScriptTable movement_airdamp(gEnv->pScriptSystem);
		movement_airdamp->SetValue("dampAngle", Vec3(1.2, 0.75, 0));
		movement_airdamp->SetValue("dampAngVel", Vec3(1.5, 0.80000001, 0.80000001));
		movement->SetValue("AirDamp", movement_airdamp);

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
		seat->SetValue("enterHelper", "passenger01Enter_pos");
		seat->SetValue("sitHelper", "passenger01Sit_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("agVehicleName", "US_ltv");
		seat->SetValue("transitionAnim", true);
		if (m_modName == "MP")
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
			view->SetValue("rotationMin", Vec3(-12, 0, -100));
			view->SetValue("rotationMax", Vec3(5, 0, 100));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "driver_eye_pos");
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
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0.5));
			view_actionthirdperson->SetValue("lagSpeed", 3.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-2, 0.029999999, 2));
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
			SmartScriptTable seataction_steeringwheel_car(gEnv->pScriptSystem);
			seataction_steeringwheel_car->SetValue("wheelRotationMax", 0.050000001f);
			seataction_steeringwheel->SetValue("Car", seataction_steeringwheel_car);
			seataction->SetValue("SteeringWheel", seataction_steeringwheel);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Sound");
			SmartScriptTable seataction_sound(gEnv->pScriptSystem);
			seataction_sound->SetValue("sound", "Sounds/vehicles:us_ltv:horn");
			seataction_sound->SetValue("helper", "engineSmokeOut");
			seataction->SetValue("Sound", seataction_sound);
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
		if (m_modName == "MP")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Unarmed")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Gauss")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAC")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAR")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.2f);
		}
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
		seat->SetValue("enterHelper", "passenger04Enter_pos");
		seat->SetValue("sitHelper", "gunner_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "shiten");
		seat->SetValue("isPassengerExposed", true);
		seat->SetValue("usesSeatForEntering", "");
		seat->SetValue("seatGroupIndex", 2);
		seat->SetValue("agVehicleName", "US_ltv");
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
		seat->SetValue("enterHelper", "passenger02Enter_pos");
		seat->SetValue("sitHelper", "passenger02Sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
		seat->SetValue("agVehicleName", "US_ltv");
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -100));
			view->SetValue("rotationMax", Vec3(5, 0, 100));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "passenger_eye_pos");
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0.5));
			view_actionthirdperson->SetValue("lagSpeed", 3.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-2, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		if (m_modName == "MP")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Unarmed")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Gauss")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAC")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAR")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.2f);
		}
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger = new VehicleSeat();
		passenger->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger", passenger });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger_left_rear");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger03Enter_pos");
		seat->SetValue("sitHelper", "passenger03Sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		seat->SetValue("agVehicleName", "US_ltv");
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(5, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "left_rear_eye_pos");
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0.5));
			view_actionthirdperson->SetValue("lagSpeed", 3.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-2, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.5f);
		if (m_modName == "MP")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Unarmed")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Gauss")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAC")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAR")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.2f);
		}
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger_left_rear = new VehicleSeat();
		passenger_left_rear->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger_left_rear", passenger_left_rear });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger_right_rear");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger04Enter_pos");
		seat->SetValue("sitHelper", "passenger04Sit_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 5);
		seat->SetValue("agVehicleName", "US_ltv");
		seat->SetValue("transitionAnim", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -90));
			view->SetValue("rotationMax", Vec3(5, 0, 90));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "right_rear_eye_pos");
			view_firstperson->SetValue("relativeToHorizon", 0.5f);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, -1.25, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0.5));
			view_actionthirdperson->SetValue("lagSpeed", 3.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(-2, 0.029999999, 2));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.5f);
		if (m_modName == "MP")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Unarmed")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "Gauss")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAC")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else if (m_modName == "MOAR")
		{
			seat_sounds->SetValue("mood", 0.6f);
		}
		else
		{
			seat_sounds->SetValue("mood", 0.2f);
		}
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger_right_rear = new VehicleSeat();
		passenger_right_rear->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger_right_rear", passenger_right_rear });
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
		seatGroup_seats->PushBack("passenger_left_rear");
		seatGroup_seats->PushBack("passenger_right_rear");
		seatGroup->SetValue("Seats", seatGroup_seats);

		VehicleSeatGroup* cabin = new VehicleSeatGroup();
		cabin->Init(this, seatGroup);
		m_seatGroups.push_back(cabin);
	}

	// Paints
	if (m_paintName == "")
	{
		this->SetPaintMaterial("objects/vehicles/ltv/asian_ltv.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/ltv/us_ltv.mtl");
	}
	else if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/ltv/asian_ltv.mtl");
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
				activation->SetValue("param2", "EnterLeftFront");
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
				activation->SetValue("param2", "EnterRightFront");
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
				activation->SetValue("param2", "EnterLeftRear");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("passenger_left_rear");
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
				activation->SetValue("param2", "EnterRightRear");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("passenger_right_rear");
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
				activation->SetValue("param2", "EnterTop");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("gunner");
			action_enter_seats->PushBack("driver");
			action_enter_seats->PushBack("passenger");
			action_enter_seats->PushBack("passenger_left_rear");
			action_enter_seats->PushBack("passenger_right_rear");
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
		damages->SetValue("submergedRatioMax", 0.64999998f);
		damages->SetValue("submergedDamageMult", 1.0f);
		damages->SetValue("collDamageThreshold", 10.0f);
		damages->SetValue("groundCollisionMinMult", 1.3f);
		damages->SetValue("groundCollisionMaxMult", 1.4f);
		damages->SetValue("groundCollisionMinSpeed", 8.0f);
		damages->SetValue("groundCollisionMaxSpeed", 22.0f);
		damages->SetValue("vehicleCollisionDestructionSpeed", 18.0f);
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
			damagemultiplier->SetValue("multiplier", 0.2f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		damages->SetValue("DamageMultipliers", damages_damagemultipliers);
		SmartScriptTable damages_damagesgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable Destroy_damagesgroup(gEnv->pScriptSystem);
			Destroy_damagesgroup->SetValue("name", "Destroy");
			Destroy_damagesgroup->SetValue("useTemplate", "CarDestroy");
			SmartScriptTable Destroy_damagesgroup_damagessubgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.30000001f);
				damagessubgroup->SetValue("randomness", 0.5f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Impulse");
					damagebehavior->SetValue("damageRatioMin", 1.0f);
					damagebehavior->SetValue("ignoreVehicleDestruction", true);
					SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
					damagebehavior_impulse->SetValue("forceMin", 3.7f);
					damagebehavior_impulse->SetValue("forceMax", 4.0f);
					damagebehavior_impulse->SetValue("direction", Vec3(0.050000001, 0.1, 1));
					damagebehavior_impulse->SetValue("momentum", Vec3(0.2, 0, 0));
					damagebehavior_impulse->SetValue("worldSpace", true);
					damagebehavior->SetValue("Impulse", damagebehavior_impulse);
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
