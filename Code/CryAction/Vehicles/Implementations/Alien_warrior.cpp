#include "Alien_warrior.h"

Alien_warrior::Alien_warrior()
{
	// enable Alien_warrior::Init function
	this->Vehicle::PatchVTable();
}

Alien_warrior::~Alien_warrior()
{
}

bool Alien_warrior::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_physics.damping = 0.15f;
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	// Unknown attribute collDamageThreshold="100000"
	m_retainGravity = true;
	m_buoyancy.waterDensity = 400.0f;
	m_buoyancy.waterResistance = 600.0f;
	m_buoyancy.waterDamping = 0.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.002f;
	m_physics.maxLoggedCollisions = 2;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable warrior_part(gEnv->pScriptSystem);
			warrior_part->SetValue("name", "warrior");
			warrior_part->SetValue("class", "Animated");
			warrior_part->SetValue("mass", 0.0f);
			warrior_part->SetValue("isHidden", false);
			SmartScriptTable warrior_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable turret_part(gEnv->pScriptSystem);
				turret_part->SetValue("name", "turret");
				turret_part->SetValue("class", "AnimatedJoint");
				turret_part->SetValue("mass", 0.0f);
				turret_part->SetValue("isHidden", false);
				SmartScriptTable turret_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable cannon_part(gEnv->pScriptSystem);
					cannon_part->SetValue("name", "cannon");
					cannon_part->SetValue("class", "AnimatedJoint");
					cannon_part->SetValue("mass", 0.0f);
					SmartScriptTable cannon_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable gunOut_helper(gEnv->pScriptSystem);
						gunOut_helper->SetValue("name", "gunOut");
						gunOut_helper->SetValue("position", Vec3(16.321, 30.738, 19.782));
						gunOut_helper->SetValue("direction", Vec3(0.004, 0.986, -0.168));
						cannon_part_helpers->PushBack(gunOut_helper);
					}
					{
						SmartScriptTable driverEyePos_helper(gEnv->pScriptSystem);
						driverEyePos_helper->SetValue("name", "driverEyePos");
						driverEyePos_helper->SetValue("position", Vec3(0.031, 1.016, 31.256));
						driverEyePos_helper->SetValue("direction", Vec3(0, 0.966, -0.259));
						cannon_part_helpers->PushBack(driverEyePos_helper);
					}
					cannon_part->SetValue("Helpers", cannon_part_helpers);
					SmartScriptTable cannon_part_animatedjoint(gEnv->pScriptSystem);
					cannon_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 0, 0));
					SmartScriptTable cannon_part_animatedjoint_rotation(gEnv->pScriptSystem);
					cannon_part_animatedjoint_rotation->SetValue("pitchSpeed", 20.0f);
					cannon_part_animatedjoint_rotation->SetValue("pitchAccel", 100.0f);
					SmartScriptTable cannon_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					cannon_part_animatedjoint_rotation_pitchlimits->PushBack(-10.0f);
					cannon_part_animatedjoint_rotation_pitchlimits->PushBack(20.0f);
					cannon_part_animatedjoint_rotation->SetValue("pitchLimits", cannon_part_animatedjoint_rotation_pitchlimits);
					cannon_part_animatedjoint->SetValue("Rotation", cannon_part_animatedjoint_rotation);
					cannon_part->SetValue("AnimatedJoint", cannon_part_animatedjoint);
					turret_part_parts->PushBack(cannon_part);
				}
				{
					SmartScriptTable generator_part(gEnv->pScriptSystem);
					generator_part->SetValue("name", "generator");
					generator_part->SetValue("class", "AnimatedJoint");
					generator_part->SetValue("mass", 0.0f);
					SmartScriptTable generator_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable generator_part_animatedjoint_rotation(gEnv->pScriptSystem);
					generator_part_animatedjoint_rotation->SetValue("pitchSpeed", 20.0f);
					SmartScriptTable generator_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					generator_part_animatedjoint_rotation_pitchlimits->PushBack(-45.0f);
					generator_part_animatedjoint_rotation_pitchlimits->PushBack(10.0f);
					generator_part_animatedjoint_rotation->SetValue("pitchLimits", generator_part_animatedjoint_rotation_pitchlimits);
					generator_part_animatedjoint->SetValue("Rotation", generator_part_animatedjoint_rotation);
					generator_part->SetValue("AnimatedJoint", generator_part_animatedjoint);
					turret_part_parts->PushBack(generator_part);
				}
				turret_part->SetValue("Parts", turret_part_parts);
				SmartScriptTable turret_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable driverExitPos_helper(gEnv->pScriptSystem);
					driverExitPos_helper->SetValue("name", "driverExitPos");
					driverExitPos_helper->SetValue("position", Vec3(0.025, -0.284, 29.602));
					driverExitPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(driverExitPos_helper);
				}
				{
					SmartScriptTable driverSitPos_helper(gEnv->pScriptSystem);
					driverSitPos_helper->SetValue("name", "driverSitPos");
					driverSitPos_helper->SetValue("position", Vec3(1.193, 0.819, 23.048));
					driverSitPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(driverSitPos_helper);
				}
				{
					SmartScriptTable topSitPos_helper(gEnv->pScriptSystem);
					topSitPos_helper->SetValue("name", "topSitPos");
					topSitPos_helper->SetValue("position", Vec3(-0.029, 0.595, 28.978));
					topSitPos_helper->SetValue("direction", Vec3(0, 1, 0));
					turret_part_helpers->PushBack(topSitPos_helper);
				}
				turret_part->SetValue("Helpers", turret_part_helpers);
				SmartScriptTable turret_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable turret_part_animatedjoint_rotation(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation->SetValue("yawSpeed", 20.0f);
				turret_part_animatedjoint_rotation->SetValue("yawAccel", 100.0f);
				SmartScriptTable turret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation_yawlimits->PushBack(-60.0f);
				turret_part_animatedjoint_rotation_yawlimits->PushBack(60.0f);
				turret_part_animatedjoint_rotation->SetValue("yawLimits", turret_part_animatedjoint_rotation_yawlimits);
				turret_part_animatedjoint->SetValue("Rotation", turret_part_animatedjoint_rotation);
				turret_part->SetValue("AnimatedJoint", turret_part_animatedjoint);
				warrior_part_parts->PushBack(turret_part);
			}
			{
				SmartScriptTable transient_part(gEnv->pScriptSystem);
				transient_part->SetValue("name", "transient");
				transient_part->SetValue("class", "AnimatedJoint");
				transient_part->SetValue("mass", 0.0f);
				SmartScriptTable transient_part_animatedjoint(gEnv->pScriptSystem);
				transient_part->SetValue("AnimatedJoint", transient_part_animatedjoint);
				warrior_part_parts->PushBack(transient_part);
			}
			{
				SmartScriptTable attachment_base_01_part(gEnv->pScriptSystem);
				attachment_base_01_part->SetValue("name", "attachment_base_01");
				attachment_base_01_part->SetValue("class", "EntityAttachment");
				attachment_base_01_part->SetValue("component", "");
				attachment_base_01_part->SetValue("helper", "attachment_base_01_helper");
				attachment_base_01_part->SetValue("mass", 0.0f);
				attachment_base_01_part->SetValue("disablePhysics", false);
				attachment_base_01_part->SetValue("disableCollision", false);
				attachment_base_01_part->SetValue("isHidden", false);
				SmartScriptTable attachment_base_01_part_helpers(gEnv->pScriptSystem);
				attachment_base_01_part->SetValue("Helpers", attachment_base_01_part_helpers);
				// Unknown element "Attachment"
				warrior_part_parts->PushBack(attachment_base_01_part);
			}
			{
				SmartScriptTable base_part(gEnv->pScriptSystem);
				base_part->SetValue("name", "base");
				base_part->SetValue("class", "AnimatedJoint");
				base_part->SetValue("mass", 0.0f);
				SmartScriptTable base_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable leg_01_part(gEnv->pScriptSystem);
					leg_01_part->SetValue("name", "leg_01");
					leg_01_part->SetValue("class", "AnimatedJoint");
					leg_01_part->SetValue("component", "");
					leg_01_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_01_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_01_part(gEnv->pScriptSystem);
						foot_01_part->SetValue("name", "foot_01");
						foot_01_part->SetValue("class", "AnimatedJoint");
						foot_01_part->SetValue("component", "thruster_01");
						foot_01_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_01_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_01_part(gEnv->pScriptSystem);
							attachment_foot_01_part->SetValue("name", "attachment_foot_01");
							attachment_foot_01_part->SetValue("class", "EntityAttachment");
							attachment_foot_01_part->SetValue("component", "");
							attachment_foot_01_part->SetValue("helper", "attachment_foot_01_helper");
							attachment_foot_01_part->SetValue("mass", 0.0f);
							foot_01_part_parts->PushBack(attachment_foot_01_part);
						}
						foot_01_part->SetValue("Parts", foot_01_part_parts);
						SmartScriptTable foot_01_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_01_helper(gEnv->pScriptSystem);
							thruster_01_helper->SetValue("name", "thruster_01");
							thruster_01_helper->SetValue("position", Vec3(-15.23, 30.3, 0.502));
							thruster_01_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_01_part_helpers->PushBack(thruster_01_helper);
						}
						{
							SmartScriptTable attachment_foot_01_helper_helper(gEnv->pScriptSystem);
							attachment_foot_01_helper_helper->SetValue("name", "attachment_foot_01_helper");
							attachment_foot_01_helper_helper->SetValue("position", Vec3(-16.109, 32.734, 5.338));
							attachment_foot_01_helper_helper->SetValue("direction", Vec3(-0.078, 0.132, -0.988));
							foot_01_part_helpers->PushBack(attachment_foot_01_helper_helper);
						}
						foot_01_part->SetValue("Helpers", foot_01_part_helpers);
						SmartScriptTable foot_01_part_animatedjoint(gEnv->pScriptSystem);
						foot_01_part->SetValue("AnimatedJoint", foot_01_part_animatedjoint);
						leg_01_part_parts->PushBack(foot_01_part);
					}
					leg_01_part->SetValue("Parts", leg_01_part_parts);
					SmartScriptTable leg_01_part_animatedjoint(gEnv->pScriptSystem);
					leg_01_part->SetValue("AnimatedJoint", leg_01_part_animatedjoint);
					base_part_parts->PushBack(leg_01_part);
				}
				{
					SmartScriptTable leg_02_part(gEnv->pScriptSystem);
					leg_02_part->SetValue("name", "leg_02");
					leg_02_part->SetValue("class", "AnimatedJoint");
					leg_02_part->SetValue("component", "");
					leg_02_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_02_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_02_part(gEnv->pScriptSystem);
						foot_02_part->SetValue("name", "foot_02");
						foot_02_part->SetValue("class", "AnimatedJoint");
						foot_02_part->SetValue("component", "thruster_02");
						foot_02_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_02_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_02_part(gEnv->pScriptSystem);
							attachment_foot_02_part->SetValue("name", "attachment_foot_02");
							attachment_foot_02_part->SetValue("class", "EntityAttachment");
							attachment_foot_02_part->SetValue("component", "");
							attachment_foot_02_part->SetValue("helper", "attachment_foot_02_helper");
							attachment_foot_02_part->SetValue("mass", 0.0f);
							foot_02_part_parts->PushBack(attachment_foot_02_part);
						}
						foot_02_part->SetValue("Parts", foot_02_part_parts);
						SmartScriptTable foot_02_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_02_helper(gEnv->pScriptSystem);
							thruster_02_helper->SetValue("name", "thruster_02");
							thruster_02_helper->SetValue("position", Vec3(15.257, 30.659, 0.502));
							thruster_02_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_02_part_helpers->PushBack(thruster_02_helper);
						}
						{
							SmartScriptTable attachment_foot_02_helper_helper(gEnv->pScriptSystem);
							attachment_foot_02_helper_helper->SetValue("name", "attachment_foot_02_helper");
							attachment_foot_02_helper_helper->SetValue("position", Vec3(16.012, 32.789, 5.488));
							attachment_foot_02_helper_helper->SetValue("direction", Vec3(0.109, 0.134, -0.985));
							foot_02_part_helpers->PushBack(attachment_foot_02_helper_helper);
						}
						foot_02_part->SetValue("Helpers", foot_02_part_helpers);
						SmartScriptTable foot_02_part_animatedjoint(gEnv->pScriptSystem);
						foot_02_part->SetValue("AnimatedJoint", foot_02_part_animatedjoint);
						leg_02_part_parts->PushBack(foot_02_part);
					}
					leg_02_part->SetValue("Parts", leg_02_part_parts);
					SmartScriptTable leg_02_part_animatedjoint(gEnv->pScriptSystem);
					leg_02_part->SetValue("AnimatedJoint", leg_02_part_animatedjoint);
					base_part_parts->PushBack(leg_02_part);
				}
				{
					SmartScriptTable leg_03_part(gEnv->pScriptSystem);
					leg_03_part->SetValue("name", "leg_03");
					leg_03_part->SetValue("class", "AnimatedJoint");
					leg_03_part->SetValue("component", "");
					leg_03_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_03_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_03_part(gEnv->pScriptSystem);
						foot_03_part->SetValue("name", "foot_03");
						foot_03_part->SetValue("class", "AnimatedJoint");
						foot_03_part->SetValue("component", "thruster_03");
						foot_03_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_03_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_03_part(gEnv->pScriptSystem);
							attachment_foot_03_part->SetValue("name", "attachment_foot_03");
							attachment_foot_03_part->SetValue("class", "EntityAttachment");
							attachment_foot_03_part->SetValue("component", "");
							attachment_foot_03_part->SetValue("helper", "attachment_foot_03_helper");
							attachment_foot_03_part->SetValue("mass", 0.0f);
							attachment_foot_03_part->SetValue("disablePhysics", false);
							attachment_foot_03_part->SetValue("disableCollision", false);
							attachment_foot_03_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_03_part_helpers(gEnv->pScriptSystem);
							attachment_foot_03_part->SetValue("Helpers", attachment_foot_03_part_helpers);
							foot_03_part_parts->PushBack(attachment_foot_03_part);
						}
						foot_03_part->SetValue("Parts", foot_03_part_parts);
						SmartScriptTable foot_03_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_03_helper(gEnv->pScriptSystem);
							thruster_03_helper->SetValue("name", "thruster_03");
							thruster_03_helper->SetValue("position", Vec3(-23.831, 20.277, 0.502));
							thruster_03_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_03_part_helpers->PushBack(thruster_03_helper);
						}
						{
							SmartScriptTable attachment_foot_03_helper_helper(gEnv->pScriptSystem);
							attachment_foot_03_helper_helper->SetValue("name", "attachment_foot_03_helper");
							attachment_foot_03_helper_helper->SetValue("position", Vec3(-25.422, 21.912, 5.398));
							attachment_foot_03_helper_helper->SetValue("direction", Vec3(-0.114, 0.114, -0.987));
							foot_03_part_helpers->PushBack(attachment_foot_03_helper_helper);
						}
						foot_03_part->SetValue("Helpers", foot_03_part_helpers);
						SmartScriptTable foot_03_part_animatedjoint(gEnv->pScriptSystem);
						foot_03_part->SetValue("AnimatedJoint", foot_03_part_animatedjoint);
						leg_03_part_parts->PushBack(foot_03_part);
					}
					leg_03_part->SetValue("Parts", leg_03_part_parts);
					SmartScriptTable leg_03_part_animatedjoint(gEnv->pScriptSystem);
					leg_03_part->SetValue("AnimatedJoint", leg_03_part_animatedjoint);
					base_part_parts->PushBack(leg_03_part);
				}
				{
					SmartScriptTable leg_04_part(gEnv->pScriptSystem);
					leg_04_part->SetValue("name", "leg_04");
					leg_04_part->SetValue("class", "AnimatedJoint");
					leg_04_part->SetValue("component", "");
					leg_04_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_04_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_04_part(gEnv->pScriptSystem);
						foot_04_part->SetValue("name", "foot_04");
						foot_04_part->SetValue("class", "AnimatedJoint");
						foot_04_part->SetValue("component", "thruster_04");
						foot_04_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_04_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_04_part(gEnv->pScriptSystem);
							attachment_foot_04_part->SetValue("name", "attachment_foot_04");
							attachment_foot_04_part->SetValue("class", "EntityAttachment");
							attachment_foot_04_part->SetValue("component", "");
							attachment_foot_04_part->SetValue("helper", "attachment_foot_04_helper");
							attachment_foot_04_part->SetValue("mass", 0.0f);
							attachment_foot_04_part->SetValue("disablePhysics", false);
							attachment_foot_04_part->SetValue("disableCollision", false);
							attachment_foot_04_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_04_part_helpers(gEnv->pScriptSystem);
							attachment_foot_04_part->SetValue("Helpers", attachment_foot_04_part_helpers);
							foot_04_part_parts->PushBack(attachment_foot_04_part);
						}
						foot_04_part->SetValue("Parts", foot_04_part_parts);
						SmartScriptTable foot_04_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_04_helper(gEnv->pScriptSystem);
							thruster_04_helper->SetValue("name", "thruster_04");
							thruster_04_helper->SetValue("position", Vec3(23.64, 20.293, 0.502));
							thruster_04_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_04_part_helpers->PushBack(thruster_04_helper);
						}
						{
							SmartScriptTable attachment_foot_04_helper_helper(gEnv->pScriptSystem);
							attachment_foot_04_helper_helper->SetValue("name", "attachment_foot_04_helper");
							attachment_foot_04_helper_helper->SetValue("position", Vec3(25.408, 21.968, 5.738));
							attachment_foot_04_helper_helper->SetValue("direction", Vec3(0.117, 0.127, -0.985));
							foot_04_part_helpers->PushBack(attachment_foot_04_helper_helper);
						}
						foot_04_part->SetValue("Helpers", foot_04_part_helpers);
						SmartScriptTable foot_04_part_animatedjoint(gEnv->pScriptSystem);
						foot_04_part->SetValue("AnimatedJoint", foot_04_part_animatedjoint);
						leg_04_part_parts->PushBack(foot_04_part);
					}
					leg_04_part->SetValue("Parts", leg_04_part_parts);
					SmartScriptTable leg_04_part_animatedjoint(gEnv->pScriptSystem);
					leg_04_part->SetValue("AnimatedJoint", leg_04_part_animatedjoint);
					base_part_parts->PushBack(leg_04_part);
				}
				{
					SmartScriptTable leg_05_part(gEnv->pScriptSystem);
					leg_05_part->SetValue("name", "leg_05");
					leg_05_part->SetValue("class", "AnimatedJoint");
					leg_05_part->SetValue("component", "");
					leg_05_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_05_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_05_part(gEnv->pScriptSystem);
						foot_05_part->SetValue("name", "foot_05");
						foot_05_part->SetValue("class", "AnimatedJoint");
						foot_05_part->SetValue("component", "thruster_05");
						foot_05_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_05_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_05_part(gEnv->pScriptSystem);
							attachment_foot_05_part->SetValue("name", "attachment_foot_05");
							attachment_foot_05_part->SetValue("class", "EntityAttachment");
							attachment_foot_05_part->SetValue("component", "");
							attachment_foot_05_part->SetValue("helper", "attachment_foot_05_helper");
							attachment_foot_05_part->SetValue("mass", 0.0f);
							attachment_foot_05_part->SetValue("disablePhysics", false);
							attachment_foot_05_part->SetValue("disableCollision", false);
							attachment_foot_05_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_05_part_helpers(gEnv->pScriptSystem);
							attachment_foot_05_part->SetValue("Helpers", attachment_foot_05_part_helpers);
							foot_05_part_parts->PushBack(attachment_foot_05_part);
						}
						foot_05_part->SetValue("Parts", foot_05_part_parts);
						SmartScriptTable foot_05_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_05_helper(gEnv->pScriptSystem);
							thruster_05_helper->SetValue("name", "thruster_05");
							thruster_05_helper->SetValue("position", Vec3(-30.59, -1.107, 0.502));
							thruster_05_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_05_part_helpers->PushBack(thruster_05_helper);
						}
						{
							SmartScriptTable attachment_foot_05_helper_helper(gEnv->pScriptSystem);
							attachment_foot_05_helper_helper->SetValue("name", "attachment_foot_05_helper");
							attachment_foot_05_helper_helper->SetValue("position", Vec3(-32.948, -1.233, 5.488));
							attachment_foot_05_helper_helper->SetValue("direction", Vec3(-0.088, 0.006, -0.996));
							foot_05_part_helpers->PushBack(attachment_foot_05_helper_helper);
						}
						foot_05_part->SetValue("Helpers", foot_05_part_helpers);
						SmartScriptTable foot_05_part_animatedjoint(gEnv->pScriptSystem);
						foot_05_part->SetValue("AnimatedJoint", foot_05_part_animatedjoint);
						leg_05_part_parts->PushBack(foot_05_part);
					}
					leg_05_part->SetValue("Parts", leg_05_part_parts);
					SmartScriptTable leg_05_part_animatedjoint(gEnv->pScriptSystem);
					leg_05_part->SetValue("AnimatedJoint", leg_05_part_animatedjoint);
					base_part_parts->PushBack(leg_05_part);
				}
				{
					SmartScriptTable leg_06_part(gEnv->pScriptSystem);
					leg_06_part->SetValue("name", "leg_06");
					leg_06_part->SetValue("class", "AnimatedJoint");
					leg_06_part->SetValue("component", "");
					leg_06_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_06_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_06_part(gEnv->pScriptSystem);
						foot_06_part->SetValue("name", "foot_06");
						foot_06_part->SetValue("class", "AnimatedJoint");
						foot_06_part->SetValue("component", "thruster_06");
						foot_06_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_06_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_06_part(gEnv->pScriptSystem);
							attachment_foot_06_part->SetValue("name", "attachment_foot_06");
							attachment_foot_06_part->SetValue("class", "EntityAttachment");
							attachment_foot_06_part->SetValue("component", "");
							attachment_foot_06_part->SetValue("helper", "attachment_foot_06_helper");
							attachment_foot_06_part->SetValue("mass", 0.0f);
							attachment_foot_06_part->SetValue("disablePhysics", false);
							attachment_foot_06_part->SetValue("disableCollision", false);
							attachment_foot_06_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_06_part_helpers(gEnv->pScriptSystem);
							attachment_foot_06_part->SetValue("Helpers", attachment_foot_06_part_helpers);
							foot_06_part_parts->PushBack(attachment_foot_06_part);
						}
						foot_06_part->SetValue("Parts", foot_06_part_parts);
						SmartScriptTable foot_06_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_06_helper(gEnv->pScriptSystem);
							thruster_06_helper->SetValue("name", "thruster_06");
							thruster_06_helper->SetValue("position", Vec3(30.66, -1.012, 0.502));
							thruster_06_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_06_part_helpers->PushBack(thruster_06_helper);
						}
						{
							SmartScriptTable attachment_foot_06_helper_helper(gEnv->pScriptSystem);
							attachment_foot_06_helper_helper->SetValue("name", "attachment_foot_06_helper");
							attachment_foot_06_helper_helper->SetValue("position", Vec3(32.915, -1.045, 5.498));
							attachment_foot_06_helper_helper->SetValue("direction", Vec3(0.242, 0.022, -0.97));
							foot_06_part_helpers->PushBack(attachment_foot_06_helper_helper);
						}
						foot_06_part->SetValue("Helpers", foot_06_part_helpers);
						SmartScriptTable foot_06_part_animatedjoint(gEnv->pScriptSystem);
						foot_06_part->SetValue("AnimatedJoint", foot_06_part_animatedjoint);
						leg_06_part_parts->PushBack(foot_06_part);
					}
					leg_06_part->SetValue("Parts", leg_06_part_parts);
					SmartScriptTable leg_06_part_animatedjoint(gEnv->pScriptSystem);
					leg_06_part->SetValue("AnimatedJoint", leg_06_part_animatedjoint);
					base_part_parts->PushBack(leg_06_part);
				}
				{
					SmartScriptTable leg_07_part(gEnv->pScriptSystem);
					leg_07_part->SetValue("name", "leg_07");
					leg_07_part->SetValue("class", "AnimatedJoint");
					leg_07_part->SetValue("component", "");
					leg_07_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_07_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_07_part(gEnv->pScriptSystem);
						foot_07_part->SetValue("name", "foot_07");
						foot_07_part->SetValue("class", "AnimatedJoint");
						foot_07_part->SetValue("component", "");
						foot_07_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_07_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_07_part(gEnv->pScriptSystem);
							attachment_foot_07_part->SetValue("name", "attachment_foot_07");
							attachment_foot_07_part->SetValue("class", "EntityAttachment");
							attachment_foot_07_part->SetValue("component", "");
							attachment_foot_07_part->SetValue("helper", "attachment_foot_07_helper");
							attachment_foot_07_part->SetValue("mass", 0.0f);
							attachment_foot_07_part->SetValue("disablePhysics", false);
							attachment_foot_07_part->SetValue("disableCollision", false);
							attachment_foot_07_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_07_part_helpers(gEnv->pScriptSystem);
							attachment_foot_07_part->SetValue("Helpers", attachment_foot_07_part_helpers);
							foot_07_part_parts->PushBack(attachment_foot_07_part);
						}
						foot_07_part->SetValue("Parts", foot_07_part_parts);
						SmartScriptTable foot_07_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_07_helper(gEnv->pScriptSystem);
							thruster_07_helper->SetValue("name", "thruster_07");
							thruster_07_helper->SetValue("position", Vec3(-25.939, -19.184, 0.502));
							thruster_07_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_07_part_helpers->PushBack(thruster_07_helper);
						}
						{
							SmartScriptTable attachment_foot_07_helper_helper(gEnv->pScriptSystem);
							attachment_foot_07_helper_helper->SetValue("name", "attachment_foot_07_helper");
							attachment_foot_07_helper_helper->SetValue("position", Vec3(-28.066, -20.686, 5.488));
							attachment_foot_07_helper_helper->SetValue("direction", Vec3(-0.086, -0.036, -0.996));
							foot_07_part_helpers->PushBack(attachment_foot_07_helper_helper);
						}
						foot_07_part->SetValue("Helpers", foot_07_part_helpers);
						SmartScriptTable foot_07_part_animatedjoint(gEnv->pScriptSystem);
						foot_07_part->SetValue("AnimatedJoint", foot_07_part_animatedjoint);
						leg_07_part_parts->PushBack(foot_07_part);
					}
					leg_07_part->SetValue("Parts", leg_07_part_parts);
					SmartScriptTable leg_07_part_animatedjoint(gEnv->pScriptSystem);
					leg_07_part->SetValue("AnimatedJoint", leg_07_part_animatedjoint);
					base_part_parts->PushBack(leg_07_part);
				}
				{
					SmartScriptTable leg_08_part(gEnv->pScriptSystem);
					leg_08_part->SetValue("name", "leg_08");
					leg_08_part->SetValue("class", "AnimatedJoint");
					leg_08_part->SetValue("component", "");
					leg_08_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_08_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_08_part(gEnv->pScriptSystem);
						foot_08_part->SetValue("name", "foot_08");
						foot_08_part->SetValue("class", "AnimatedJoint");
						foot_08_part->SetValue("component", "");
						foot_08_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_08_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_08_part(gEnv->pScriptSystem);
							attachment_foot_08_part->SetValue("name", "attachment_foot_08");
							attachment_foot_08_part->SetValue("class", "EntityAttachment");
							attachment_foot_08_part->SetValue("component", "");
							attachment_foot_08_part->SetValue("helper", "attachment_foot_08_helper");
							attachment_foot_08_part->SetValue("mass", 0.0f);
							attachment_foot_08_part->SetValue("disablePhysics", false);
							attachment_foot_08_part->SetValue("disableCollision", false);
							attachment_foot_08_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_08_part_helpers(gEnv->pScriptSystem);
							attachment_foot_08_part->SetValue("Helpers", attachment_foot_08_part_helpers);
							foot_08_part_parts->PushBack(attachment_foot_08_part);
						}
						foot_08_part->SetValue("Parts", foot_08_part_parts);
						SmartScriptTable foot_08_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_08_helper(gEnv->pScriptSystem);
							thruster_08_helper->SetValue("name", "thruster_08");
							thruster_08_helper->SetValue("position", Vec3(27.086, -20.083, 0.502));
							thruster_08_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_08_part_helpers->PushBack(thruster_08_helper);
						}
						{
							SmartScriptTable attachment_foot_08_helper_helper(gEnv->pScriptSystem);
							attachment_foot_08_helper_helper->SetValue("name", "attachment_foot_08_helper");
							attachment_foot_08_helper_helper->SetValue("position", Vec3(28.491, -21.353, 5.907));
							attachment_foot_08_helper_helper->SetValue("direction", Vec3(0.134, -0.11, -0.985));
							foot_08_part_helpers->PushBack(attachment_foot_08_helper_helper);
						}
						foot_08_part->SetValue("Helpers", foot_08_part_helpers);
						SmartScriptTable foot_08_part_animatedjoint(gEnv->pScriptSystem);
						foot_08_part->SetValue("AnimatedJoint", foot_08_part_animatedjoint);
						leg_08_part_parts->PushBack(foot_08_part);
					}
					leg_08_part->SetValue("Parts", leg_08_part_parts);
					SmartScriptTable leg_08_part_animatedjoint(gEnv->pScriptSystem);
					leg_08_part->SetValue("AnimatedJoint", leg_08_part_animatedjoint);
					base_part_parts->PushBack(leg_08_part);
				}
				{
					SmartScriptTable leg_09_part(gEnv->pScriptSystem);
					leg_09_part->SetValue("name", "leg_09");
					leg_09_part->SetValue("class", "AnimatedJoint");
					leg_09_part->SetValue("component", "");
					leg_09_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_09_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_09_part(gEnv->pScriptSystem);
						foot_09_part->SetValue("name", "foot_09");
						foot_09_part->SetValue("class", "AnimatedJoint");
						foot_09_part->SetValue("component", "");
						foot_09_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_09_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_09_part(gEnv->pScriptSystem);
							attachment_foot_09_part->SetValue("name", "attachment_foot_09");
							attachment_foot_09_part->SetValue("class", "EntityAttachment");
							attachment_foot_09_part->SetValue("component", "");
							attachment_foot_09_part->SetValue("helper", "attachment_foot_09_helper");
							attachment_foot_09_part->SetValue("mass", 0.0f);
							attachment_foot_09_part->SetValue("disablePhysics", false);
							attachment_foot_09_part->SetValue("disableCollision", false);
							attachment_foot_09_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_09_part_helpers(gEnv->pScriptSystem);
							attachment_foot_09_part->SetValue("Helpers", attachment_foot_09_part_helpers);
							foot_09_part_parts->PushBack(attachment_foot_09_part);
						}
						foot_09_part->SetValue("Parts", foot_09_part_parts);
						SmartScriptTable foot_09_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_09_helper(gEnv->pScriptSystem);
							thruster_09_helper->SetValue("name", "thruster_09");
							thruster_09_helper->SetValue("position", Vec3(-15.551, -29.397, 0.502));
							thruster_09_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_09_part_helpers->PushBack(thruster_09_helper);
						}
						{
							SmartScriptTable attachment_foot_09_helper_helper(gEnv->pScriptSystem);
							attachment_foot_09_helper_helper->SetValue("name", "attachment_foot_09_helper");
							attachment_foot_09_helper_helper->SetValue("position", Vec3(-16.655, -31.368, 5.528));
							attachment_foot_09_helper_helper->SetValue("direction", Vec3(-0.034, -0.182, -0.983));
							foot_09_part_helpers->PushBack(attachment_foot_09_helper_helper);
						}
						foot_09_part->SetValue("Helpers", foot_09_part_helpers);
						SmartScriptTable foot_09_part_animatedjoint(gEnv->pScriptSystem);
						foot_09_part->SetValue("AnimatedJoint", foot_09_part_animatedjoint);
						leg_09_part_parts->PushBack(foot_09_part);
					}
					leg_09_part->SetValue("Parts", leg_09_part_parts);
					SmartScriptTable leg_09_part_animatedjoint(gEnv->pScriptSystem);
					leg_09_part->SetValue("AnimatedJoint", leg_09_part_animatedjoint);
					base_part_parts->PushBack(leg_09_part);
				}
				{
					SmartScriptTable leg_10_part(gEnv->pScriptSystem);
					leg_10_part->SetValue("name", "leg_10");
					leg_10_part->SetValue("class", "AnimatedJoint");
					leg_10_part->SetValue("component", "");
					leg_10_part->SetValue("mass", 0.0f);
					SmartScriptTable leg_10_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable foot_10_part(gEnv->pScriptSystem);
						foot_10_part->SetValue("name", "foot_10");
						foot_10_part->SetValue("class", "AnimatedJoint");
						foot_10_part->SetValue("component", "");
						foot_10_part->SetValue("mass", 0.0f);
						SmartScriptTable foot_10_part_parts(gEnv->pScriptSystem);
						{
							SmartScriptTable attachment_foot_10_part(gEnv->pScriptSystem);
							attachment_foot_10_part->SetValue("name", "attachment_foot_10");
							attachment_foot_10_part->SetValue("class", "EntityAttachment");
							attachment_foot_10_part->SetValue("component", "");
							attachment_foot_10_part->SetValue("helper", "attachment_foot_10_helper");
							attachment_foot_10_part->SetValue("mass", 0.0f);
							attachment_foot_10_part->SetValue("disablePhysics", false);
							attachment_foot_10_part->SetValue("disableCollision", false);
							attachment_foot_10_part->SetValue("isHidden", false);
							SmartScriptTable attachment_foot_10_part_helpers(gEnv->pScriptSystem);
							attachment_foot_10_part->SetValue("Helpers", attachment_foot_10_part_helpers);
							foot_10_part_parts->PushBack(attachment_foot_10_part);
						}
						foot_10_part->SetValue("Parts", foot_10_part_parts);
						SmartScriptTable foot_10_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable thruster_10_helper(gEnv->pScriptSystem);
							thruster_10_helper->SetValue("name", "thruster_10");
							thruster_10_helper->SetValue("position", Vec3(15.988, -29.917, 0.502));
							thruster_10_helper->SetValue("direction", Vec3(0, 0, -1));
							foot_10_part_helpers->PushBack(thruster_10_helper);
						}
						{
							SmartScriptTable attachment_foot_10_helper_helper(gEnv->pScriptSystem);
							attachment_foot_10_helper_helper->SetValue("name", "attachment_foot_10_helper");
							attachment_foot_10_helper_helper->SetValue("position", Vec3(16.666, -31.855, 6.017));
							attachment_foot_10_helper_helper->SetValue("direction", Vec3(0.068, -0.176, -0.982));
							foot_10_part_helpers->PushBack(attachment_foot_10_helper_helper);
						}
						foot_10_part->SetValue("Helpers", foot_10_part_helpers);
						SmartScriptTable foot_10_part_animatedjoint(gEnv->pScriptSystem);
						foot_10_part->SetValue("AnimatedJoint", foot_10_part_animatedjoint);
						leg_10_part_parts->PushBack(foot_10_part);
					}
					leg_10_part->SetValue("Parts", leg_10_part_parts);
					SmartScriptTable leg_10_part_animatedjoint(gEnv->pScriptSystem);
					leg_10_part->SetValue("AnimatedJoint", leg_10_part_animatedjoint);
					base_part_parts->PushBack(leg_10_part);
				}
				base_part->SetValue("Parts", base_part_parts);
				SmartScriptTable base_part_animatedjoint(gEnv->pScriptSystem);
				base_part->SetValue("AnimatedJoint", base_part_animatedjoint);
				warrior_part_parts->PushBack(base_part);
			}
			warrior_part->SetValue("Parts", warrior_part_parts);
			SmartScriptTable warrior_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable gravityPos_helper(gEnv->pScriptSystem);
				gravityPos_helper->SetValue("name", "gravityPos");
				gravityPos_helper->SetValue("position", Vec3(0.181, 1.125, 22.119));
				gravityPos_helper->SetValue("direction", Vec3(0, 1, 0));
				warrior_part_helpers->PushBack(gravityPos_helper);
			}
			{
				SmartScriptTable thruster_body_01_helper(gEnv->pScriptSystem);
				thruster_body_01_helper->SetValue("name", "thruster_body_01");
				thruster_body_01_helper->SetValue("position", Vec3(-6.492, 13.002, 8.002));
				thruster_body_01_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_body_01_helper);
			}
			{
				SmartScriptTable thruster_body_02_helper(gEnv->pScriptSystem);
				thruster_body_02_helper->SetValue("name", "thruster_body_02");
				thruster_body_02_helper->SetValue("position", Vec3(6.508, 13.003, 8.002));
				thruster_body_02_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_body_02_helper);
			}
			{
				SmartScriptTable thruster_body_03_helper(gEnv->pScriptSystem);
				thruster_body_03_helper->SetValue("name", "thruster_body_03");
				thruster_body_03_helper->SetValue("position", Vec3(-6.491, -8.939, 8.002));
				thruster_body_03_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_body_03_helper);
			}
			{
				SmartScriptTable thruster_body_04_helper(gEnv->pScriptSystem);
				thruster_body_04_helper->SetValue("name", "thruster_body_04");
				thruster_body_04_helper->SetValue("position", Vec3(6.509, -8.707, 8.002));
				thruster_body_04_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_body_04_helper);
			}
			{
				SmartScriptTable attachment_base_01_helper_helper(gEnv->pScriptSystem);
				attachment_base_01_helper_helper->SetValue("name", "attachment_base_01_helper");
				attachment_base_01_helper_helper->SetValue("position", Vec3(-0.092, 23.319, 9.603));
				attachment_base_01_helper_helper->SetValue("direction", Vec3(0, 0.966, -0.259));
				warrior_part_helpers->PushBack(attachment_base_01_helper_helper);
			}
			{
				SmartScriptTable platform_pos_helper(gEnv->pScriptSystem);
				platform_pos_helper->SetValue("name", "platform_pos");
				platform_pos_helper->SetValue("position", Vec3(-0.031, 23.996, 7.335));
				platform_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				warrior_part_helpers->PushBack(platform_pos_helper);
			}
			{
				SmartScriptTable thruster_01_base_helper(gEnv->pScriptSystem);
				thruster_01_base_helper->SetValue("name", "thruster_01_base");
				thruster_01_base_helper->SetValue("position", Vec3(-10, 20, 8.002));
				thruster_01_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_01_base_helper);
			}
			{
				SmartScriptTable thruster_02_base_helper(gEnv->pScriptSystem);
				thruster_02_base_helper->SetValue("name", "thruster_02_base");
				thruster_02_base_helper->SetValue("position", Vec3(10, 20, 8.002));
				thruster_02_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_02_base_helper);
			}
			{
				SmartScriptTable thruster_03_base_helper(gEnv->pScriptSystem);
				thruster_03_base_helper->SetValue("name", "thruster_03_base");
				thruster_03_base_helper->SetValue("position", Vec3(-15.5, 11.5, 8.002));
				thruster_03_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_03_base_helper);
			}
			{
				SmartScriptTable thruster_04_base_helper(gEnv->pScriptSystem);
				thruster_04_base_helper->SetValue("name", "thruster_04_base");
				thruster_04_base_helper->SetValue("position", Vec3(15.5, 11.5, 8.002));
				thruster_04_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_04_base_helper);
			}
			{
				SmartScriptTable thruster_05_base_helper(gEnv->pScriptSystem);
				thruster_05_base_helper->SetValue("name", "thruster_05_base");
				thruster_05_base_helper->SetValue("position", Vec3(-19.5, -1, 8.002));
				thruster_05_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_05_base_helper);
			}
			{
				SmartScriptTable thruster_06_base_helper(gEnv->pScriptSystem);
				thruster_06_base_helper->SetValue("name", "thruster_06_base");
				thruster_06_base_helper->SetValue("position", Vec3(19.5, -1, 8.002));
				thruster_06_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_06_base_helper);
			}
			{
				SmartScriptTable thruster_07_base_helper(gEnv->pScriptSystem);
				thruster_07_base_helper->SetValue("name", "thruster_07_base");
				thruster_07_base_helper->SetValue("position", Vec3(-18.5, -13, 8.002));
				thruster_07_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_07_base_helper);
			}
			{
				SmartScriptTable thruster_08_base_helper(gEnv->pScriptSystem);
				thruster_08_base_helper->SetValue("name", "thruster_08_base");
				thruster_08_base_helper->SetValue("position", Vec3(18.5, -13, 8.002));
				thruster_08_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_08_base_helper);
			}
			{
				SmartScriptTable thruster_09_base_helper(gEnv->pScriptSystem);
				thruster_09_base_helper->SetValue("name", "thruster_09_base");
				thruster_09_base_helper->SetValue("position", Vec3(-12, -23, 8.002));
				thruster_09_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_09_base_helper);
			}
			{
				SmartScriptTable thruster_10_base_helper(gEnv->pScriptSystem);
				thruster_10_base_helper->SetValue("name", "thruster_10_base");
				thruster_10_base_helper->SetValue("position", Vec3(12, -23, 8.002));
				thruster_10_base_helper->SetValue("direction", Vec3(0, 0, -1));
				warrior_part_helpers->PushBack(thruster_10_base_helper);
			}
			warrior_part->SetValue("Helpers", warrior_part_helpers);
			SmartScriptTable warrior_part_animated(gEnv->pScriptSystem);
			warrior_part_animated->SetValue("filename", "objects/characters/alien/warrior/warrior_wb_v2.cga");
			warrior_part->SetValue("Animated", warrior_part_animated);
			parts->PushBack(warrior_part);
		}
		{
			SmartScriptTable mass_part(gEnv->pScriptSystem);
			mass_part->SetValue("name", "mass");
			mass_part->SetValue("class", "MassBox");
			mass_part->SetValue("component", "");
			mass_part->SetValue("mass", 40000.0f);
			mass_part->SetValue("position", Vec3(0, 0.771, 7.588));
			mass_part->SetValue("disablePhysics", false);
			mass_part->SetValue("disableCollision", false);
			mass_part->SetValue("isHidden", false);
			SmartScriptTable mass_part_helpers(gEnv->pScriptSystem);
			mass_part->SetValue("Helpers", mass_part_helpers);
			SmartScriptTable mass_part_massbox(gEnv->pScriptSystem);
			mass_part_massbox->SetValue("size", Vec3(23.123, 29.739, 5.571));
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
		component->SetValue("damageMax", 10000.0f);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* hull = new VehicleComponent();
		hull->Init(this, component);
		m_components.push_back(hull);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "enterArea");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("minBound", Vec3(-2.739, -1.703, 28.562));
		component->SetValue("maxBound", Vec3(2.952, 4.069, 31.875));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterArea = new VehicleComponent();
		enterArea->Init(this, component);
		m_components.push_back(enterArea);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "enterAreaInside");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("minBound", Vec3(-2.739, -3.419, 21.039));
		component->SetValue("maxBound", Vec3(2.952, 2.353, 24.352));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* enterAreaInside = new VehicleComponent();
		enterAreaInside->Init(this, component);
		m_components.push_back(enterAreaInside);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_01");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(-19.411, 24.573, -2.034));
		component->SetValue("maxBound", Vec3(-9.995, 34.235, 9.742));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
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
			damagebehavior_effect->SetValue("effect", "Thruster1_Destroyed");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_01 = new VehicleComponent();
		thruster_01->Init(this, component);
		m_components.push_back(thruster_01);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_02");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(10.361, 25.115, -2.264));
		component->SetValue("maxBound", Vec3(19.595, 34.591, 9.054));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
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
			damagebehavior_effect->SetValue("effect", "Thruster2_Destroyed");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_02 = new VehicleComponent();
		thruster_02->Init(this, component);
		m_components.push_back(thruster_02);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_03");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(-27.344, 14.433, -2.409));
		component->SetValue("maxBound", Vec3(-17.382, 24.563, 9.865));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
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
			damagebehavior_effect->SetValue("effect", "Thruster3_Destroyed");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_03 = new VehicleComponent();
		thruster_03->Init(this, component);
		m_components.push_back(thruster_03);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_04");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(17.687, 13.806, -2.274));
		component->SetValue("maxBound", Vec3(27.662, 24.588, 8.86));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
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
			damagebehavior_effect->SetValue("effect", "Thruster4_Destroyed");
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_04 = new VehicleComponent();
		thruster_04->Init(this, component);
		m_components.push_back(thruster_04);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_05");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(-35.219, -6.627, -2.247));
		component->SetValue("maxBound", Vec3(-24.026, 3.503, 9.577));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_05 = new VehicleComponent();
		thruster_05->Init(this, component);
		m_components.push_back(thruster_05);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_06");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(23.987, -6.031, -2.479));
		component->SetValue("maxBound", Vec3(34.72, 4.102, 9.887));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_06 = new VehicleComponent();
		thruster_06->Init(this, component);
		m_components.push_back(thruster_06);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_07");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(-30.706, -23.434, -2.479));
		component->SetValue("maxBound", Vec3(-19.973, -13.301, 9.887));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_07 = new VehicleComponent();
		thruster_07->Init(this, component);
		m_components.push_back(thruster_07);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_08");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(20.584, -24.031, -2.479));
		component->SetValue("maxBound", Vec3(31.317, -13.898, 9.887));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_08 = new VehicleComponent();
		thruster_08->Init(this, component);
		m_components.push_back(thruster_08);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_09");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(-20.128, -33.694, -2.479));
		component->SetValue("maxBound", Vec3(-9.395, -23.561, 9.887));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_09 = new VehicleComponent();
		thruster_09->Init(this, component);
		m_components.push_back(thruster_09);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "thruster_10");
		component->SetValue("damageMax", 400.0f);
		component->SetValue("minBound", Vec3(9.276, -33.603, -2.479));
		component->SetValue("maxBound", Vec3(20.009, -22.848, 9.887));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isFatal", false);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* thruster_10 = new VehicleComponent();
		thruster_10->Init(this, component);
		m_components.push_back(thruster_10);
	}

	this->RegisterComponentParts(componentParts);

	// Particles
	{
		SmartScriptTable particles(gEnv->pScriptSystem);
		SmartScriptTable particles_exhaust(gEnv->pScriptSystem);
		particles_exhaust->SetValue("insideWater", false);
		particles_exhaust->SetValue("outsideWater", true);
		SmartScriptTable particles_exhaust_helpers(gEnv->pScriptSystem);
		particles_exhaust_helpers->PushBack("thruster_01");
		particles_exhaust_helpers->PushBack("thruster_02");
		particles_exhaust_helpers->PushBack("thruster_03");
		particles_exhaust_helpers->PushBack("thruster_04");
		particles_exhaust_helpers->PushBack("thruster_05");
		particles_exhaust_helpers->PushBack("thruster_06");
		particles_exhaust_helpers->PushBack("thruster_07");
		particles_exhaust_helpers->PushBack("thruster_08");
		particles_exhaust_helpers->PushBack("thruster_09");
		particles_exhaust_helpers->PushBack("thruster_10");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "alien_special.Warrior.thrusters");
		particles_exhaust_enginerunning->SetValue("boostEffect", "");
		particles_exhaust_enginerunning->SetValue("baseSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeed", 15.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPower", 0.0f);
		particles_exhaust_enginerunning->SetValue("minPowerSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPowerCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPower", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerCountScale", 1.0f);
		particles_exhaust->SetValue("EngineRunning", particles_exhaust_enginerunning);
		particles->SetValue("Exhaust", particles_exhaust);
		SmartScriptTable particles_damageeffects(gEnv->pScriptSystem);
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "vehicleBurnPos");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.truck_explosion.chinese_truck");
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable Thruster1_Destroyed_damageeffect(gEnv->pScriptSystem);
			Thruster1_Destroyed_damageeffect->SetValue("name", "Thruster1_Destroyed");
			Thruster1_Destroyed_damageeffect->SetValue("helper", "thruster_01");
			Thruster1_Destroyed_damageeffect->SetValue("effect", "explosions.grenade_terrain.explosion");
			particles_damageeffects->PushBack(Thruster1_Destroyed_damageeffect);
		}
		{
			SmartScriptTable Thruster2_Destroyed_damageeffect(gEnv->pScriptSystem);
			Thruster2_Destroyed_damageeffect->SetValue("name", "Thruster2_Destroyed");
			Thruster2_Destroyed_damageeffect->SetValue("helper", "thruster_02");
			Thruster2_Destroyed_damageeffect->SetValue("effect", "explosions.grenade_terrain.explosion");
			particles_damageeffects->PushBack(Thruster2_Destroyed_damageeffect);
		}
		{
			SmartScriptTable Thruster3_Destroyed_damageeffect(gEnv->pScriptSystem);
			Thruster3_Destroyed_damageeffect->SetValue("name", "Thruster3_Destroyed");
			Thruster3_Destroyed_damageeffect->SetValue("helper", "thruster_03");
			Thruster3_Destroyed_damageeffect->SetValue("effect", "explosions.grenade_terrain.explosion");
			particles_damageeffects->PushBack(Thruster3_Destroyed_damageeffect);
		}
		{
			SmartScriptTable Thruster4_Destroyed_damageeffect(gEnv->pScriptSystem);
			Thruster4_Destroyed_damageeffect->SetValue("name", "Thruster4_Destroyed");
			Thruster4_Destroyed_damageeffect->SetValue("helper", "thruster_04");
			Thruster4_Destroyed_damageeffect->SetValue("effect", "explosions.grenade_terrain.explosion");
			particles_damageeffects->PushBack(Thruster4_Destroyed_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 0.0f);
			environmentlayer->SetValue("minSpeedSizeScale", 0.5f);
			environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			environmentlayer->SetValue("maxSpeed", 15.0f);
			environmentlayer->SetValue("maxSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("maxSpeedCountScale", 1.0f);
			SmartScriptTable environmentlayer_alignment(gEnv->pScriptSystem);
			environmentlayer_alignment->SetValue("alignGroundHeight", 2.0f);
			environmentlayer_alignment->SetValue("maxHeightSizeScale", 0.0f);
			environmentlayer_alignment->SetValue("maxHeightCountScale", 0.75f);
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

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("maxThrustersDamaged", 4);
		movement->SetValue("recoveryTime", 40);
		movement->SetValue("collapsedLegAngle", 10.0f);
		movement->SetValue("collapsedFeetAngle", -90.0f);
		SmartScriptTable movement_hovercraft(gEnv->pScriptSystem);
		movement_hovercraft->SetValue("hoverHeight", 2.0f);
		movement_hovercraft->SetValue("hoverVariance", 0.15f);
		movement_hovercraft->SetValue("hoverFrequency", 0.5f);
		movement_hovercraft->SetValue("numThrusters", 0);
		movement_hovercraft->SetValue("thrusterBottomOffset", 0.0f);
		movement_hovercraft->SetValue("thrusterMaxHeightCoeff", 2.0f);
		movement_hovercraft->SetValue("stiffness", 1.3f);
		movement_hovercraft->SetValue("damping", 0.6f);
		movement_hovercraft->SetValue("dampingLimit", 4.0f);
		movement_hovercraft->SetValue("thrusterTilt", 0.0f);
		movement_hovercraft->SetValue("velMax", 6.0f);
		movement_hovercraft->SetValue("acceleration", 3.0f);
		movement_hovercraft->SetValue("accelerationMultiplier", 1.5f);
		movement_hovercraft->SetValue("pushOffset", Vec3(0, 0, 13));
		movement_hovercraft->SetValue("pushTilt", 0.0f);
		movement_hovercraft->SetValue("linearDamping", 2.0f);
		movement_hovercraft->SetValue("turnRateMax", 0.3f);
		movement_hovercraft->SetValue("turnAccel", 0.35f);
		movement_hovercraft->SetValue("turnAccelMultiplier", 1.0f);
		movement_hovercraft->SetValue("turnDamping", 4.0f);
		movement_hovercraft->SetValue("cornerForce", 0.5f);
		movement_hovercraft->SetValue("cornerOffset", Vec3(0, -5, 25));
		movement_hovercraft->SetValue("cornerTilt", 0.0f);
		movement_hovercraft->SetValue("sampleByHelpers", true);
		movement_hovercraft->SetValue("thrusterHeightAdaption", 5.0f);
		movement_hovercraft->SetValue("thrusterUpdate", 0.0f);
		movement_hovercraft->SetValue("bEngineAlwaysOn", false);
		SmartScriptTable movement_hovercraft_thrusters(gEnv->pScriptSystem);
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_body_01");
			thruster->SetValue("hoverVariance", -1.0f);
			thruster->SetValue("heightAdaption", 0.0f);
			thruster->SetValue("hoverHeight", 7.0f);
			thruster->SetValue("pushing", true);
			thruster->SetValue("cylinder", 0.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_body_02");
			thruster->SetValue("hoverVariance", -1.0f);
			thruster->SetValue("heightAdaption", 0.0f);
			thruster->SetValue("hoverHeight", 7.0f);
			thruster->SetValue("pushing", true);
			thruster->SetValue("cylinder", 0.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_body_03");
			thruster->SetValue("hoverVariance", -1.0f);
			thruster->SetValue("heightAdaption", 0.0f);
			thruster->SetValue("hoverHeight", 7.0f);
			thruster->SetValue("pushing", true);
			thruster->SetValue("cylinder", 0.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_body_04");
			thruster->SetValue("hoverVariance", -1.0f);
			thruster->SetValue("heightAdaption", 0.0f);
			thruster->SetValue("hoverHeight", 7.0f);
			thruster->SetValue("pushing", true);
			thruster->SetValue("cylinder", 0.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_01");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_02");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_03");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_04");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_05");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_06");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_07");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_08");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_09");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		{
			SmartScriptTable thruster(gEnv->pScriptSystem);
			thruster->SetValue("helper", "thruster_10");
			thruster->SetValue("hoverVariance", 0.0f);
			thruster->SetValue("heightAdaption", -1.0f);
			thruster->SetValue("hoverHeight", -1.0f);
			thruster->SetValue("pushing", false);
			thruster->SetValue("cylinder", 3.0f);
			movement_hovercraft_thrusters->PushBack(thruster);
		}
		movement_hovercraft->SetValue("Thrusters", movement_hovercraft_thrusters);
		SmartScriptTable movement_hovercraft_soundparams(gEnv->pScriptSystem);
		movement_hovercraft_soundparams->SetValue("engineSoundPosition", "");
		movement_hovercraft_soundparams->SetValue("runSoundDelay", 0.1f);
		movement_hovercraft_soundparams->SetValue("rpmPitchSpeed", 1.5f);
		movement_hovercraft->SetValue("SoundParams", movement_hovercraft_soundparams);
		movement->SetValue("Hovercraft", movement_hovercraft);

		if (!this->SetMovement("Warrior", movement))
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
		seat->SetValue("enterHelper", "driverSitPos");
		seat->SetValue("exitHelper", "driverSitPos");
		seat->SetValue("sitHelper", "driverSitPos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("blurMult", 0.0f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "driverSitPos");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 65.0f);
			view_thirdperson->SetValue("heightOffset", -10.0f);
			view_thirdperson->SetValue("speed", 6.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "RotateTurret");
			seataction->SetValue("isAvailableRemotely", true);
			SmartScriptTable seataction_rotateturret(gEnv->pScriptSystem);
			seataction_rotateturret->SetValue("pitchPart", "cannon");
			seataction_rotateturret->SetValue("yawPart", "turret");
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
				weapon->SetValue("class", "SingularityCannon");
				weapon->SetValue("part", "cannon");
				weapon->SetValue("inheritVelocity", false);
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("gunOut");
				weapon->SetValue("Helpers", weapon_helpers);
				seataction_weapons_weapons->PushBack(weapon);
			}
			seataction_weapons->SetValue("Weapons", seataction_weapons_weapons);
			seataction->SetValue("Weapons", seataction_weapons);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		seats->PushBack(seat);

		VehicleSeat* driver = new VehicleSeat();
		driver->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "driver", driver });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "driver_top");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "driverExitPos");
		seat->SetValue("exitHelper", "driverExitPos");
		seat->SetValue("sitHelper", "topSitPos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("remotelyUseActionsFromSeat", "driver");
		seat->SetValue("blurMult", 0.0f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "driverEyePos");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", true);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 65.0f);
			view_thirdperson->SetValue("heightOffset", -10.0f);
			view_thirdperson->SetValue("speed", 6.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		seat->SetValue("SeatActions", seat_seatactions);
		seats->PushBack(seat);

		VehicleSeat* driver_top = new VehicleSeat();
		driver_top->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "driver_top", driver_top });
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
		seatGroup->SetValue("name", "driver");
		SmartScriptTable seatGroup_seats(gEnv->pScriptSystem);
		seatGroup_seats->PushBack("driver");
		seatGroup->SetValue("Seats", seatGroup_seats);

		VehicleSeatGroup* driver = new VehicleSeatGroup();
		driver->Init(this, seatGroup);
		m_seatGroups.push_back(driver);
	}
	{
		SmartScriptTable seatGroup(gEnv->pScriptSystem);
		seatGroup->SetValue("name", "driver_top");
		SmartScriptTable seatGroup_seats(gEnv->pScriptSystem);
		seatGroup_seats->PushBack("driver_top");
		seatGroup->SetValue("Seats", seatGroup_seats);

		VehicleSeatGroup* driver_top = new VehicleSeatGroup();
		driver_top->Init(this, seatGroup);
		m_seatGroups.push_back(driver_top);
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
				activation->SetValue("param2", "enterArea");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("driver_top");
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
				activation->SetValue("param2", "enterAreaInside");
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

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Actions", actions);
		this->InitActions(table);
	}

	this->InitMaxDamage();
	this->AttachScriptBindToSeats();

	return this->BindVehicleToNetwork();
}
