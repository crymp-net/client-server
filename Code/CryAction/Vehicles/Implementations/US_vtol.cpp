#include "US_vtol.h"

US_vtol::US_vtol()
{
	// enable US_vtol::Init function
	this->Vehicle::PatchVTable();
}

US_vtol::~US_vtol()
{
}

bool US_vtol::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "vtol";

	// Physics
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 150.0f;
	m_buoyancy.waterResistance = 2500.0f;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.16f;
	m_physics.maxLoggedCollisions = 2;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable hull_part(gEnv->pScriptSystem);
			hull_part->SetValue("name", "hull");
			hull_part->SetValue("class", "Animated");
			hull_part->SetValue("component", "Hull");
			hull_part->SetValue("mass", 19000.0f);
			hull_part->SetValue("disablePhysics", false);
			SmartScriptTable hull_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable vtol_door_back_part(gEnv->pScriptSystem);
				vtol_door_back_part->SetValue("name", "vtol_door_back");
				vtol_door_back_part->SetValue("class", "AnimatedJoint");
				vtol_door_back_part->SetValue("component", "vtol_door_back");
				hull_part_parts->PushBack(vtol_door_back_part);
			}
			{
				SmartScriptTable wing_left_part(gEnv->pScriptSystem);
				wing_left_part->SetValue("name", "wing_left");
				wing_left_part->SetValue("class", "AnimatedJoint");
				wing_left_part->SetValue("component", "WingLeft");
				SmartScriptTable wing_left_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable wing_left_end_part(gEnv->pScriptSystem);
					wing_left_end_part->SetValue("name", "wing_left_end");
					wing_left_end_part->SetValue("class", "AnimatedJoint");
					wing_left_end_part->SetValue("component", "WingLeft");
					SmartScriptTable wing_left_end_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable positionLightLeftWing_part(gEnv->pScriptSystem);
						positionLightLeftWing_part->SetValue("name", "positionLightLeftWing");
						positionLightLeftWing_part->SetValue("class", "Light");
						positionLightLeftWing_part->SetValue("component", "WingLeft");
						positionLightLeftWing_part->SetValue("helper", "positionLightLeftWingPos");
						SmartScriptTable positionLightLeftWing_part_light(gEnv->pScriptSystem);
						positionLightLeftWing_part_light->SetValue("type", "Rear");
						positionLightLeftWing_part_light->SetValue("frequency", 1.0f);
						positionLightLeftWing_part_light->SetValue("duration", 0.2f);
						positionLightLeftWing_part->SetValue("Light", positionLightLeftWing_part_light);
						wing_left_end_part_parts->PushBack(positionLightLeftWing_part);
					}
					wing_left_end_part->SetValue("Parts", wing_left_end_part_parts);
					SmartScriptTable wing_left_end_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable positionLightLeftWingPos_helper(gEnv->pScriptSystem);
						positionLightLeftWingPos_helper->SetValue("name", "positionLightLeftWingPos");
						positionLightLeftWingPos_helper->SetValue("position", Vec3(-4.8183999, -0.92140001, -3.9507999));
						positionLightLeftWingPos_helper->SetValue("direction", Vec3(0, 1, 0));
						wing_left_end_part_helpers->PushBack(positionLightLeftWingPos_helper);
					}
					wing_left_end_part->SetValue("Helpers", wing_left_end_part_helpers);
					SmartScriptTable wing_left_end_part_animatedjoint(gEnv->pScriptSystem);
					wing_left_end_part_animatedjoint->SetValue("detachBaseForce", Vec3(-1, 0, 0));
					wing_left_end_part->SetValue("AnimatedJoint", wing_left_end_part_animatedjoint);
					wing_left_part_parts->PushBack(wing_left_end_part);
				}
				wing_left_part->SetValue("Parts", wing_left_part_parts);
				SmartScriptTable wing_left_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable thruster1_helper(gEnv->pScriptSystem);
					thruster1_helper->SetValue("name", "thruster1");
					thruster1_helper->SetValue("position", Vec3(4.2519999, 2.0811, -3.1500001));
					thruster1_helper->SetValue("direction", Vec3(0, 0, -1));
					wing_left_part_helpers->PushBack(thruster1_helper);
				}
				{
					SmartScriptTable thruster2_helper(gEnv->pScriptSystem);
					thruster2_helper->SetValue("name", "thruster2");
					thruster2_helper->SetValue("position", Vec3(4.2410002, 0.84689999, -3.1489999));
					thruster2_helper->SetValue("direction", Vec3(0, 0, -1));
					wing_left_part_helpers->PushBack(thruster2_helper);
				}
				wing_left_part->SetValue("Helpers", wing_left_part_helpers);
				hull_part_parts->PushBack(wing_left_part);
			}
			{
				SmartScriptTable wing_right_01_part(gEnv->pScriptSystem);
				wing_right_01_part->SetValue("name", "wing_right_01");
				wing_right_01_part->SetValue("class", "AnimatedJoint");
				wing_right_01_part->SetValue("component", "WingRight");
				SmartScriptTable wing_right_01_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable wing_right_02_part(gEnv->pScriptSystem);
					wing_right_02_part->SetValue("name", "wing_right_02");
					wing_right_02_part->SetValue("class", "AnimatedJoint");
					wing_right_02_part->SetValue("component", "WingRight");
					SmartScriptTable wing_right_02_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable positionLightRightWing_part(gEnv->pScriptSystem);
						positionLightRightWing_part->SetValue("name", "positionLightRightWing");
						positionLightRightWing_part->SetValue("class", "Light");
						positionLightRightWing_part->SetValue("component", "WingRight");
						positionLightRightWing_part->SetValue("helper", "positionLightRightWingPos");
						SmartScriptTable positionLightRightWing_part_light(gEnv->pScriptSystem);
						positionLightRightWing_part_light->SetValue("type", "Rear");
						positionLightRightWing_part_light->SetValue("frequency", 1.0f);
						positionLightRightWing_part_light->SetValue("duration", 0.2f);
						positionLightRightWing_part->SetValue("Light", positionLightRightWing_part_light);
						wing_right_02_part_parts->PushBack(positionLightRightWing_part);
					}
					wing_right_02_part->SetValue("Parts", wing_right_02_part_parts);
					SmartScriptTable wing_right_02_part_helpers(gEnv->pScriptSystem);
					{
						SmartScriptTable positionLightRightWingPos_helper(gEnv->pScriptSystem);
						positionLightRightWingPos_helper->SetValue("name", "positionLightRightWingPos");
						positionLightRightWingPos_helper->SetValue("position", Vec3(4.8262, -0.92140001, -3.9507999));
						positionLightRightWingPos_helper->SetValue("direction", Vec3(0, 1, 0));
						wing_right_02_part_helpers->PushBack(positionLightRightWingPos_helper);
					}
					wing_right_02_part->SetValue("Helpers", wing_right_02_part_helpers);
					SmartScriptTable wing_right_02_part_animatedjoint(gEnv->pScriptSystem);
					wing_right_02_part_animatedjoint->SetValue("detachBaseForce", Vec3(1, 0, 0));
					wing_right_02_part->SetValue("AnimatedJoint", wing_right_02_part_animatedjoint);
					wing_right_01_part_parts->PushBack(wing_right_02_part);
				}
				wing_right_01_part->SetValue("Parts", wing_right_01_part_parts);
				SmartScriptTable wing_right_01_part_helpers(gEnv->pScriptSystem);
				{
					SmartScriptTable thruster3_helper(gEnv->pScriptSystem);
					thruster3_helper->SetValue("name", "thruster3");
					thruster3_helper->SetValue("position", Vec3(-4.2800002, 0.85500002, -3.1170001));
					thruster3_helper->SetValue("direction", Vec3(0, 0, -1));
					wing_right_01_part_helpers->PushBack(thruster3_helper);
				}
				{
					SmartScriptTable thruster4_helper(gEnv->pScriptSystem);
					thruster4_helper->SetValue("name", "thruster4");
					thruster4_helper->SetValue("position", Vec3(-4.2800002, 2.0759001, -3.1559999));
					thruster4_helper->SetValue("direction", Vec3(0, 0, -1));
					wing_right_01_part_helpers->PushBack(thruster4_helper);
				}
				wing_right_01_part->SetValue("Helpers", wing_right_01_part_helpers);
				hull_part_parts->PushBack(wing_right_01_part);
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
						gunOut_helper->SetValue("position", Vec3(-0.00039999999, 9.7363005, -2.4590001));
						gunOut_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
						vtol_gun_pitch_part_helpers->PushBack(gunOut_helper);
					}
					{
						SmartScriptTable gunCamera_helper(gEnv->pScriptSystem);
						gunCamera_helper->SetValue("name", "gunCamera");
						gunCamera_helper->SetValue("position", Vec3(-0.38769999, 8.9385004, -2.5799999));
						gunCamera_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
						vtol_gun_pitch_part_helpers->PushBack(gunCamera_helper);
					}
					{
						SmartScriptTable searchLightPos_helper(gEnv->pScriptSystem);
						searchLightPos_helper->SetValue("name", "searchLightPos");
						searchLightPos_helper->SetValue("position", Vec3(0.0134, 8.8633003, -2.7778001));
						searchLightPos_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
						vtol_gun_pitch_part_helpers->PushBack(searchLightPos_helper);
					}
					vtol_gun_pitch_part->SetValue("Helpers", vtol_gun_pitch_part_helpers);
					SmartScriptTable vtol_gun_pitch_part_animatedjoint(gEnv->pScriptSystem);
					vtol_gun_pitch_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 1, 0));
					SmartScriptTable vtol_gun_pitch_part_animatedjoint_rotation(gEnv->pScriptSystem);
					vtol_gun_pitch_part_animatedjoint_rotation->SetValue("pitchSpeed", 75.0f);
					SmartScriptTable vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
					vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits->PushBack(-80.0f);
					vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits->PushBack(0.0f);
					vtol_gun_pitch_part_animatedjoint_rotation->SetValue("pitchLimits", vtol_gun_pitch_part_animatedjoint_rotation_pitchlimits);
					vtol_gun_pitch_part_animatedjoint->SetValue("Rotation", vtol_gun_pitch_part_animatedjoint_rotation);
					vtol_gun_pitch_part->SetValue("AnimatedJoint", vtol_gun_pitch_part_animatedjoint);
					turret_part_parts->PushBack(vtol_gun_pitch_part);
				}
				{
					SmartScriptTable searchLight_part(gEnv->pScriptSystem);
					searchLight_part->SetValue("name", "searchLight");
					searchLight_part->SetValue("class", "Light");
					searchLight_part->SetValue("component", "searchLight");
					searchLight_part->SetValue("helper", "searchLightPos");
					SmartScriptTable searchLight_part_light(gEnv->pScriptSystem);
					searchLight_part_light->SetValue("type", "HeadLightDynamic");
					searchLight_part_light->SetValue("radius", 40.0f);
					searchLight_part_light->SetValue("diffuseMult", 15.0f);
					searchLight_part_light->SetValue("directFactor", 10.0f);
					searchLight_part_light->SetValue("frustumAngle", 25.0f);
					searchLight_part->SetValue("Light", searchLight_part_light);
					turret_part_parts->PushBack(searchLight_part);
				}
				turret_part->SetValue("Parts", turret_part_parts);
				SmartScriptTable turret_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable turret_part_animatedjoint_rotation(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation->SetValue("yawSpeed", 50.0f);
				SmartScriptTable turret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
				turret_part_animatedjoint_rotation_yawlimits->PushBack(-60.0f);
				turret_part_animatedjoint_rotation_yawlimits->PushBack(60.0f);
				turret_part_animatedjoint_rotation->SetValue("yawLimits", turret_part_animatedjoint_rotation_yawlimits);
				turret_part_animatedjoint->SetValue("Rotation", turret_part_animatedjoint_rotation);
				turret_part->SetValue("AnimatedJoint", turret_part_animatedjoint);
				hull_part_parts->PushBack(turret_part);
			}
			hull_part->SetValue("Parts", hull_part_parts);
			SmartScriptTable hull_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable pilot_eye_pos_helper(gEnv->pScriptSystem);
				pilot_eye_pos_helper->SetValue("name", "pilot_eye_pos");
				pilot_eye_pos_helper->SetValue("position", Vec3(0.00069999998, 3.6213, 1.365));
				pilot_eye_pos_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
				hull_part_helpers->PushBack(pilot_eye_pos_helper);
			}
			{
				SmartScriptTable gunner_eye_pos_helper(gEnv->pScriptSystem);
				gunner_eye_pos_helper->SetValue("name", "gunner_eye_pos");
				gunner_eye_pos_helper->SetValue("position", Vec3(0, 6.4201999, -0.59500003));
				gunner_eye_pos_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
				hull_part_helpers->PushBack(gunner_eye_pos_helper);
			}
			{
				SmartScriptTable passenger01_pos_helper(gEnv->pScriptSystem);
				passenger01_pos_helper->SetValue("name", "passenger01_pos");
				hull_part_helpers->PushBack(passenger01_pos_helper);
			}
			{
				SmartScriptTable passenger02_pos_helper(gEnv->pScriptSystem);
				passenger02_pos_helper->SetValue("name", "passenger02_pos");
				hull_part_helpers->PushBack(passenger02_pos_helper);
			}
			{
				SmartScriptTable passenger03_pos_helper(gEnv->pScriptSystem);
				passenger03_pos_helper->SetValue("name", "passenger03_pos");
				hull_part_helpers->PushBack(passenger03_pos_helper);
			}
			{
				SmartScriptTable passenger04_pos_helper(gEnv->pScriptSystem);
				passenger04_pos_helper->SetValue("name", "passenger04_pos");
				hull_part_helpers->PushBack(passenger04_pos_helper);
			}
			{
				SmartScriptTable passenger05_pos_helper(gEnv->pScriptSystem);
				passenger05_pos_helper->SetValue("name", "passenger05_pos");
				hull_part_helpers->PushBack(passenger05_pos_helper);
			}
			{
				SmartScriptTable passenger06_pos_helper(gEnv->pScriptSystem);
				passenger06_pos_helper->SetValue("name", "passenger06_pos");
				hull_part_helpers->PushBack(passenger06_pos_helper);
			}
			{
				SmartScriptTable passenger07_pos_helper(gEnv->pScriptSystem);
				passenger07_pos_helper->SetValue("name", "passenger07_pos");
				hull_part_helpers->PushBack(passenger07_pos_helper);
			}
			{
				SmartScriptTable passenger08_pos_helper(gEnv->pScriptSystem);
				passenger08_pos_helper->SetValue("name", "passenger08_pos");
				hull_part_helpers->PushBack(passenger08_pos_helper);
			}
			{
				SmartScriptTable enter_driver_left_helper(gEnv->pScriptSystem);
				enter_driver_left_helper->SetValue("name", "enter_driver_left");
				enter_driver_left_helper->SetValue("position", Vec3(-3.5422001, 5.9970999, -3.2179999));
				enter_driver_left_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
				hull_part_helpers->PushBack(enter_driver_left_helper);
			}
			{
				SmartScriptTable leftRocketOut_helper(gEnv->pScriptSystem);
				leftRocketOut_helper->SetValue("name", "leftRocketOut");
				leftRocketOut_helper->SetValue("position", Vec3(-1.1423, 4.0042, -0.126));
				leftRocketOut_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
				hull_part_helpers->PushBack(leftRocketOut_helper);
			}
			{
				SmartScriptTable rightRocketOut_helper(gEnv->pScriptSystem);
				rightRocketOut_helper->SetValue("name", "rightRocketOut");
				rightRocketOut_helper->SetValue("position", Vec3(1.1418, 4.0042, -0.12899999));
				rightRocketOut_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
				hull_part_helpers->PushBack(rightRocketOut_helper);
			}
			{
				SmartScriptTable bodyburn_helper(gEnv->pScriptSystem);
				bodyburn_helper->SetValue("name", "bodyburn");
				bodyburn_helper->SetValue("position", Vec3(0, 3.0072999, 1.411));
				bodyburn_helper->SetValue("direction", Vec3(0, -0.029999999, 0.99959999));
				hull_part_helpers->PushBack(bodyburn_helper);
			}
			{
				SmartScriptTable gunner_enter_pos_helper(gEnv->pScriptSystem);
				gunner_enter_pos_helper->SetValue("name", "gunner_enter_pos");
				gunner_enter_pos_helper->SetValue("position", Vec3(-3.5151, 5.0890999, -3.223));
				gunner_enter_pos_helper->SetValue("direction", Vec3(0, 1, -0.0049999999));
				hull_part_helpers->PushBack(gunner_enter_pos_helper);
			}
			{
				SmartScriptTable passenger_enter_pos_helper(gEnv->pScriptSystem);
				passenger_enter_pos_helper->SetValue("name", "passenger_enter_pos");
				passenger_enter_pos_helper->SetValue("position", Vec3(0, -5.8188, -3.473));
				passenger_enter_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				hull_part_helpers->PushBack(passenger_enter_pos_helper);
			}
			{
				SmartScriptTable impulsePos_helper(gEnv->pScriptSystem);
				impulsePos_helper->SetValue("name", "impulsePos");
				impulsePos_helper->SetValue("position", Vec3(0, 2.0002, 0));
				impulsePos_helper->SetValue("direction", Vec3(0, 1, 0));
				hull_part_helpers->PushBack(impulsePos_helper);
			}
			{
				SmartScriptTable backpassenger_exit_pos_helper(gEnv->pScriptSystem);
				backpassenger_exit_pos_helper->SetValue("name", "backpassenger_exit_pos");
				backpassenger_exit_pos_helper->SetValue("position", Vec3(0, -1.0007, -1.6357));
				backpassenger_exit_pos_helper->SetValue("direction", Vec3(0, -1, 0));
				hull_part_helpers->PushBack(backpassenger_exit_pos_helper);
			}
			{
				SmartScriptTable positionLightTopRearPos_helper(gEnv->pScriptSystem);
				positionLightTopRearPos_helper->SetValue("name", "positionLightTopRearPos");
				positionLightTopRearPos_helper->SetValue("position", Vec3(0, -7.1960001, 1.6844));
				positionLightTopRearPos_helper->SetValue("direction", Vec3(0, 1, 0));
				hull_part_helpers->PushBack(positionLightTopRearPos_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, 0, 0.54860002));
				burningPos_helper->SetValue("direction", Vec3(0, 1, 0));
				hull_part_helpers->PushBack(burningPos_helper);
			}
			{
				SmartScriptTable exitPassenger03_helper(gEnv->pScriptSystem);
				exitPassenger03_helper->SetValue("name", "exitPassenger03");
				exitPassenger03_helper->SetValue("position", Vec3(-0.21439999, -6.1756001, -3.5481));
				exitPassenger03_helper->SetValue("direction", Vec3(0.00050000002, 1, 0.003));
				hull_part_helpers->PushBack(exitPassenger03_helper);
			}
			{
				SmartScriptTable exitPassenger04_helper(gEnv->pScriptSystem);
				exitPassenger04_helper->SetValue("name", "exitPassenger04");
				exitPassenger04_helper->SetValue("position", Vec3(-0.016000001, -5.3228998, -3.5437));
				exitPassenger04_helper->SetValue("direction", Vec3(0.00050000002, 1, 0.003));
				hull_part_helpers->PushBack(exitPassenger04_helper);
			}
			{
				SmartScriptTable exitPassenger06_helper(gEnv->pScriptSystem);
				exitPassenger06_helper->SetValue("name", "exitPassenger06");
				exitPassenger06_helper->SetValue("position", Vec3(-0.0188, -5.9709001, -3.5478001));
				exitPassenger06_helper->SetValue("direction", Vec3(0.00050000002, 1, 0.003));
				hull_part_helpers->PushBack(exitPassenger06_helper);
			}
			{
				SmartScriptTable exitPassenger07_helper(gEnv->pScriptSystem);
				exitPassenger07_helper->SetValue("name", "exitPassenger07");
				exitPassenger07_helper->SetValue("position", Vec3(0.1673, -5.7216001, -3.5455999));
				exitPassenger07_helper->SetValue("direction", Vec3(0.00050000002, 1, 0.003));
				hull_part_helpers->PushBack(exitPassenger07_helper);
			}
			hull_part->SetValue("Helpers", hull_part_helpers);
			SmartScriptTable hull_part_animated(gEnv->pScriptSystem);
			hull_part_animated->SetValue("filename", "Objects/Vehicles/US_Vtol/US_Vtol.cga");
			hull_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/US_Vtol/US_Vtol_destroyed.cga");
			hull_part->SetValue("Animated", hull_part_animated);
			parts->PushBack(hull_part);
		}
		{
			SmartScriptTable positionLightTopRear_part(gEnv->pScriptSystem);
			positionLightTopRear_part->SetValue("name", "positionLightTopRear");
			positionLightTopRear_part->SetValue("class", "Light");
			positionLightTopRear_part->SetValue("component", "");
			positionLightTopRear_part->SetValue("helper", "positionLightTopRearPos");
			SmartScriptTable positionLightTopRear_part_light(gEnv->pScriptSystem);
			positionLightTopRear_part_light->SetValue("type", "Rear");
			positionLightTopRear_part_light->SetValue("frequency", 1.0f);
			positionLightTopRear_part_light->SetValue("duration", 0.2f);
			positionLightTopRear_part->SetValue("Light", positionLightTopRear_part_light);
			parts->PushBack(positionLightTopRear_part);
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
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 800.0f);
		}
		else
		{
			component->SetValue("damageMax", 800.0f);
		}
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-4.9707146, -9.1131086, -4.7231870));
		component->SetValue("maxBound", Vec3(4.9696417, 9.7253065, 4.4614549));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.033f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.025f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.0099999998f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "fire");
			damagemultiplier->SetValue("multiplier", 0.0099999998f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.079999998f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.25f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.25f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "c4");
			damagemultiplier->SetValue("multiplier", 1.0f);
			damagemultiplier->SetValue("splash", 0.25f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 2.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "avmine");
			damagemultiplier->SetValue("multiplier", 0.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.5f);
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
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			SmartScriptTable damagebehavior_DestroyEngine_group(gEnv->pScriptSystem);
			damagebehavior_DestroyEngine_group->SetValue("name", "DestroyEngine");
			damagebehavior->SetValue("Group", damagebehavior_DestroyEngine_group);
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
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "CollisionEx");
			damagebehavior->SetValue("damageRatioMin", 0.25f);
			damagebehavior->SetValue("damageRatioMax", 1.0f);
			SmartScriptTable damagebehavior_collisionex(gEnv->pScriptSystem);
			damagebehavior_collisionex->SetValue("component", "CollisionDamages");
			damagebehavior_collisionex->SetValue("damages", 0.1f);
			damagebehavior->SetValue("CollisionEx", damagebehavior_collisionex);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* hull = new VehicleComponent();
		hull->Init(this, component);
		m_components.push_back(hull);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "frontCockpit");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.77539998, 2.0525, -2.4993));
		component->SetValue("maxBound", Vec3(0.73680001, 8.5846996, 2.2499001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* frontCockpit = new VehicleComponent();
		frontCockpit->Init(this, component);
		m_components.push_back(frontCockpit);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Backseat1");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.3831, -0.25659999, -2.3076));
		component->SetValue("maxBound", Vec3(-0.94559997, 0.55930001, -1.0808001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backseat1 = new VehicleComponent();
		backseat1->Init(this, component);
		m_components.push_back(backseat1);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Backseat2");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.3832999, -1.5315, -2.3076));
		component->SetValue("maxBound", Vec3(-0.94580001, -0.71560001, -1.0808001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backseat2 = new VehicleComponent();
		backseat2->Init(this, component);
		m_components.push_back(backseat2);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Backseat3");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-1.3831, -2.7827001, -2.3076));
		component->SetValue("maxBound", Vec3(-0.94559997, -1.9668, -1.0808001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backseat3 = new VehicleComponent();
		backseat3->Init(this, component);
		m_components.push_back(backseat3);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Backseat4");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.80830002, -0.35839999, -2.3076));
		component->SetValue("maxBound", Vec3(1.2458, 0.45750001, -1.0808001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backseat4 = new VehicleComponent();
		backseat4->Init(this, component);
		m_components.push_back(backseat4);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Backseat5");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.80980003, -1.6084, -2.3076));
		component->SetValue("maxBound", Vec3(1.2473, -0.79250002, -1.0808001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backseat5 = new VehicleComponent();
		backseat5->Init(this, component);
		m_components.push_back(backseat5);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Backseat6");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(0.80049998, -2.7827001, -2.3076));
		component->SetValue("maxBound", Vec3(1.238, -1.9668, -1.0808001));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backseat6 = new VehicleComponent();
		backseat6->Init(this, component);
		m_components.push_back(backseat6);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "BackDoor");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.80000001, -4, -2.5999999));
		component->SetValue("maxBound", Vec3(0.80000001, -3, 0));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backDoor = new VehicleComponent();
		backDoor->Init(this, component);
		m_components.push_back(backDoor);
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
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "CollisionDamages");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("minBound", Vec3(-4.9706998, -9.1131001, -1.0159));
		component->SetValue("maxBound", Vec3(4.9695001, 9.7252998, 3.3736999));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* collisionDamages = new VehicleComponent();
		collisionDamages->Init(this, component);
		m_components.push_back(collisionDamages);
	}

	this->RegisterComponentParts(componentParts);

	// Particles
	{
		SmartScriptTable particles(gEnv->pScriptSystem);
		SmartScriptTable particles_exhaust(gEnv->pScriptSystem);
		particles_exhaust->SetValue("insideWater", false);
		particles_exhaust->SetValue("outsideWater", true);
		SmartScriptTable particles_exhaust_helpers(gEnv->pScriptSystem);
		particles_exhaust_helpers->PushBack("thruster1");
		particles_exhaust_helpers->PushBack("thruster2");
		particles_exhaust_helpers->PushBack("thruster3");
		particles_exhaust_helpers->PushBack("thruster4");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "vehicle_fx.vtol.exhaust_start");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "vehicle_fx.vtol.exhaust_start");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "vehicle_fx.vtol.exhaust");
		particles_exhaust_enginerunning->SetValue("boostEffect", "vehicle_fx.vtol.exhaust_start");
		particles_exhaust_enginerunning->SetValue("baseSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSpeedScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeed", 70.0f);
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
			VehicleDestroyed_damageeffect->SetValue("helper", "");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.vtol_explosion.explosion");
			VehicleDestroyed_damageeffect->SetValue("scaleMax", 1.0f);
			VehicleDestroyed_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			VehicleDestroyed_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "burningPos");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.USvtol_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.USvtol_hull_smoke_b");
			HullDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 0.0f);
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
			environmentlayer_alignment->SetValue("alignGroundHeight", 25.0f);
			environmentlayer_alignment->SetValue("maxHeightSizeScale", 0.5f);
			environmentlayer_alignment->SetValue("maxHeightCountScale", 0.60000002f);
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
		animation->SetValue("part", "hull");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable tovertical_state(gEnv->pScriptSystem);
			tovertical_state->SetValue("name", "tovertical");
			tovertical_state->SetValue("animation", "vtol_wings_tovertical");
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
			tohorizontal_state->SetValue("animation", "vtol_wings_tohorizontal");
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
		animation->SetValue("name", "landingGears");
		animation->SetValue("part", "hull");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable closed_state(gEnv->pScriptSystem);
			closed_state->SetValue("name", "closed");
			closed_state->SetValue("animation", "vtol_landing_gear_close");
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
		animation->SetValue("name", "passengerDoors");
		animation->SetValue("part", "hull");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable closed_state(gEnv->pScriptSystem);
			closed_state->SetValue("name", "closed");
			closed_state->SetValue("animation", "vtol_door_close");
			closed_state->SetValue("sound", "Sounds/vehicles:us_vtol:backdoor_close");
			closed_state->SetValue("isLooped", false);
			closed_state->SetValue("speedDefault", 1.0f);
			closed_state->SetValue("speedMin", 1.0f);
			closed_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(closed_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* passengerDoors = new VehicleAnimation();
		passengerDoors->Init(this, animation);
		m_animations.push_back({ "passengerDoors", passengerDoors });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "mg_fire");
		animation->SetValue("part", "hull");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			shoot_state->SetValue("animation", "Vtol_MG_fire");
			shoot_state->SetValue("isLooped", false);
			shoot_state->SetValue("isLoopedEx", true);
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
		movement->SetValue("engineWarmupDelay", 1.0f);
		movement->SetValue("engineIgnitionTime", 0.0f);
		movement->SetValue("enginePowerMax", 2.75f);
		movement->SetValue("rotationDamping", 26.0f);
		movement->SetValue("angleLift", 4.0f);
		movement->SetValue("altitudeMax", 1050.0f);
		movement->SetValue("rotorDiskTiltScale", 0.80000001f);
		movement->SetValue("pitchResponsiveness", 15.0f);
		movement->SetValue("rollResponsiveness", 4.0f);
		movement->SetValue("yawResponsiveness", 7.0f);
		movement->SetValue("maxYawRate", 18.0f);
		movement->SetValue("maxFwdSpeed", 85.0f);
		movement->SetValue("maxLeftSpeed", 50.0f);
		movement->SetValue("maxSpeed", 14.0f);
		movement->SetValue("maxUpSpeed", 17.0f);
		movement->SetValue("basicSpeedFraction", 0.69999999f);
		movement->SetValue("yawDecreaseWithSpeed", 250.0f);
		movement->SetValue("tiltPerVelDifference", 0.039999999f);
		movement->SetValue("maxTiltAngle", 0.15000001f);
		movement->SetValue("extraRollForTurn", 0.0099999998f);
		movement->SetValue("rollForTurnForce", 0.4f);
		movement->SetValue("yawPerRoll", -0.44999999f);
		movement->SetValue("pitchActionPerTilt", 5.0f);
		movement->SetValue("powerInputConst", 0.40000001f);
		movement->SetValue("powerInputDamping", -0.5f);
		movement->SetValue("pitchInputConst", 3.0f);
		movement->SetValue("yawInputConst", 3.0f);
		movement->SetValue("yawInputDamping", -0.5f);
		movement->SetValue("horizFwdForce", 10.0f);
		movement->SetValue("horizLeftForce", 10.0f);
		movement->SetValue("boostForce", 0.25f);
		movement->SetValue("strafeForce", 1.0f);
		movement->SetValue("relaxForce", 0.5f);
		movement->SetValue("maxRollAngle", 35.0f);
		movement->SetValue("impulseHelper", "impulsePos");
		movement->SetValue("wingsSpeed", 0.89999998f);
		movement->SetValue("WingComponentLeft", "WingLeft");
		movement->SetValue("WingComponentRight", "WingRight");
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "rotorSmokeOut");
		movement_soundparams->SetValue("runSoundDelay", 1.0f);
		movement_soundparams->SetValue("rpmPitchSpeed", 1.2f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable ai_tweakgroup(gEnv->pScriptSystem);
			ai_tweakgroup->SetValue("name", "ai");
			SmartScriptTable ai_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable enginePowerMax_tweak(gEnv->pScriptSystem);
				enginePowerMax_tweak->SetValue("name", "enginePowerMax");
				enginePowerMax_tweak->SetValue("value", 0.75f);
				enginePowerMax_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(enginePowerMax_tweak);
			}
			{
				SmartScriptTable horizFwdForce_tweak(gEnv->pScriptSystem);
				horizFwdForce_tweak->SetValue("name", "horizFwdForce");
				horizFwdForce_tweak->SetValue("value", 0.5f);
				horizFwdForce_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(horizFwdForce_tweak);
			}
			{
				SmartScriptTable horizLeftForce_tweak(gEnv->pScriptSystem);
				horizLeftForce_tweak->SetValue("name", "horizLeftForce");
				horizLeftForce_tweak->SetValue("value", 0.5f);
				horizLeftForce_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(horizLeftForce_tweak);
			}
			{
				SmartScriptTable rotorDiskTiltScale_tweak(gEnv->pScriptSystem);
				rotorDiskTiltScale_tweak->SetValue("name", "rotorDiskTiltScale");
				rotorDiskTiltScale_tweak->SetValue("value", 0.44999999f);
				rotorDiskTiltScale_tweak->SetValue("op", 0);
				ai_tweakgroup_tweaks->PushBack(rotorDiskTiltScale_tweak);
			}
			ai_tweakgroup->SetValue("Tweaks", ai_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(ai_tweakgroup);
		}
		{
			SmartScriptTable player_tweakgroup(gEnv->pScriptSystem);
			player_tweakgroup->SetValue("name", "player");
			SmartScriptTable player_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable pitchActionPerTilt_tweak(gEnv->pScriptSystem);
				pitchActionPerTilt_tweak->SetValue("name", "pitchActionPerTilt");
				pitchActionPerTilt_tweak->SetValue("value", 0.15000001f);
				pitchActionPerTilt_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(pitchActionPerTilt_tweak);
			}
			{
				SmartScriptTable pitchInputConst_tweak(gEnv->pScriptSystem);
				pitchInputConst_tweak->SetValue("name", "pitchInputConst");
				pitchInputConst_tweak->SetValue("value", 1.0f);
				pitchInputConst_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(pitchInputConst_tweak);
			}
			{
				SmartScriptTable yawInputConst_tweak(gEnv->pScriptSystem);
				yawInputConst_tweak->SetValue("name", "yawInputConst");
				yawInputConst_tweak->SetValue("value", 1.0f);
				yawInputConst_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(yawInputConst_tweak);
			}
			{
				SmartScriptTable maxYawRate_tweak(gEnv->pScriptSystem);
				maxYawRate_tweak->SetValue("name", "maxYawRate");
				maxYawRate_tweak->SetValue("value", 3.142f);
				maxYawRate_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(maxYawRate_tweak);
			}
			{
				SmartScriptTable maxYawRate_tweak(gEnv->pScriptSystem);
				maxYawRate_tweak->SetValue("name", "maxYawRate");
				maxYawRate_tweak->SetValue("value", 2.0f);
				maxYawRate_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(maxYawRate_tweak);
			}
			{
				SmartScriptTable pitchResponsiveness_tweak(gEnv->pScriptSystem);
				pitchResponsiveness_tweak->SetValue("name", "pitchResponsiveness");
				pitchResponsiveness_tweak->SetValue("value", 4.0f);
				pitchResponsiveness_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(pitchResponsiveness_tweak);
			}
			{
				SmartScriptTable yawResponsiveness_tweak(gEnv->pScriptSystem);
				yawResponsiveness_tweak->SetValue("name", "yawResponsiveness");
				yawResponsiveness_tweak->SetValue("value", 8.5f);
				yawResponsiveness_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(yawResponsiveness_tweak);
			}
			{
				SmartScriptTable rollResponsiveness_tweak(gEnv->pScriptSystem);
				rollResponsiveness_tweak->SetValue("name", "rollResponsiveness");
				rollResponsiveness_tweak->SetValue("value", 5.0f);
				rollResponsiveness_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(rollResponsiveness_tweak);
			}
			{
				SmartScriptTable rotationDamping_tweak(gEnv->pScriptSystem);
				rotationDamping_tweak->SetValue("name", "rotationDamping");
				rotationDamping_tweak->SetValue("value", 28.0f);
				rotationDamping_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(rotationDamping_tweak);
			}
			{
				SmartScriptTable yawPerRoll_tweak(gEnv->pScriptSystem);
				yawPerRoll_tweak->SetValue("name", "yawPerRoll");
				yawPerRoll_tweak->SetValue("value", 5.0f);
				yawPerRoll_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(yawPerRoll_tweak);
			}
			{
				SmartScriptTable rotorDiskTiltScale_tweak(gEnv->pScriptSystem);
				rotorDiskTiltScale_tweak->SetValue("name", "rotorDiskTiltScale");
				rotorDiskTiltScale_tweak->SetValue("value", 0.0f);
				rotorDiskTiltScale_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(rotorDiskTiltScale_tweak);
			}
			{
				SmartScriptTable extraRollForTurn_tweak(gEnv->pScriptSystem);
				extraRollForTurn_tweak->SetValue("name", "extraRollForTurn");
				extraRollForTurn_tweak->SetValue("value", 30.0f);
				extraRollForTurn_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(extraRollForTurn_tweak);
			}
			{
				SmartScriptTable powerInputConst_tweak(gEnv->pScriptSystem);
				powerInputConst_tweak->SetValue("name", "powerInputConst");
				powerInputConst_tweak->SetValue("value", 0.5f);
				powerInputConst_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(powerInputConst_tweak);
			}
			player_tweakgroup->SetValue("Tweaks", player_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);

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
		seat->SetValue("exitHelper", "passenger_enter_pos");
		seat->SetValue("exitOffsetPlayer", Vec3(3.5, 10, 0));
		seat->SetValue("sitHelper", "passenger01_pos");
		seat->SetValue("useBoundsForEntering", true);
		seat->SetValue("isDriver", true);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("blurMult", 0.0f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			view->SetValue("rotationBoundsActionMult", 0.0f);
			view->SetValue("rotationMin", Vec3(-20, 0, -80));
			view->SetValue("rotationMax", Vec3(10, 0, 80));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "pilot_eye_pos");
			view_firstperson->SetValue("hidePlayer", true);
			view_firstperson->SetValue("hideVehicle", false);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			view->SetValue("rotationBoundsActionMult", 0.0f);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "pilot_eye_pos");
			view_firstperson->SetValue("hideVehicle", true);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 0, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0));
			view_actionthirdperson->SetValue("lagSpeed", 6.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0.0099999998, 0, 0.001));
			view->SetValue("ActionThirdPerson", view_actionthirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Weapons");
			SmartScriptTable seataction_weapons(gEnv->pScriptSystem);
			seataction_weapons->SetValue("isSecondary", false);
			SmartScriptTable seataction_weapons_weapons(gEnv->pScriptSystem);
			{
				SmartScriptTable weapon(gEnv->pScriptSystem);
				if (m_modName == "VTOL_Ascension")
				{
					weapon->SetValue("class", "APCCannon_AscMod");
				}
				else
				{
					weapon->SetValue("class", "USCoaxialGun_VTOL");
				}
				weapon->SetValue("part", "cannon");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("rightRocketOut");
				weapon->SetValue("Helpers", weapon_helpers);
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
			seataction_weapons->SetValue("disablesShootToCrosshair", true);
			SmartScriptTable seataction_weapons_weapons(gEnv->pScriptSystem);
			{
				SmartScriptTable weapon(gEnv->pScriptSystem);
				if (m_modName == "VTOL_Ascension")
				{
					weapon->SetValue("class", "SideWinder_AscMod");
				}
				else if (m_modName == "Unarmed")
				{
					weapon->SetValue("class", "");
				}
				else
				{
					weapon->SetValue("class", "SideWinder");
				}
				weapon->SetValue("part", "hull");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("leftRocketOut");
				weapon_helpers->PushBack("rightRocketOut");
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
			seataction_lights_lightparts->PushBack("positionLightTopRear");
			seataction_lights_lightparts->PushBack("positionLightLeftWing");
			seataction_lights_lightparts->PushBack("positionLightRightWing");
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
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "gunner_enter_pos");
		seat->SetValue("exitHelper", "passenger_enter_pos");
		seat->SetValue("exitOffsetPlayer", Vec3(3.5, 12, 0));
		seat->SetValue("sitHelper", "passenger02_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "gunCamera");
			view_firstperson->SetValue("hideVehicle", false);
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ActionThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_actionthirdperson(gEnv->pScriptSystem);
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 0, 0));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0.0099999998, 0.025, 0.001));
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
			seataction_rotateturret->SetValue("pitchPart", "vtol_gun_pitch");
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
				if (m_modName == "Unarmed")
				{
					weapon->SetValue("class", "");
				}
				else
				{
					weapon->SetValue("class", "AvengerCannon");
				}
				weapon->SetValue("part", "vtol_gun_pitch");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("gunOut");
				weapon->SetValue("Helpers", weapon_helpers);
				SmartScriptTable weapon_actions(gEnv->pScriptSystem);
				{
					SmartScriptTable fire_action(gEnv->pScriptSystem);
					fire_action->SetValue("name", "fire");
					SmartScriptTable fire_action_animations(gEnv->pScriptSystem);
					{
						SmartScriptTable mg_fire_animation(gEnv->pScriptSystem);
						mg_fire_animation->SetValue("name", "mg_fire");
						fire_action_animations->PushBack(mg_fire_animation);
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
			seataction->SetValue("class", "Lights");
			seataction->SetValue("isAvailableRemotely", true);
			SmartScriptTable seataction_lights(gEnv->pScriptSystem);
			seataction_lights->SetValue("activation", "toggle");
			seataction_lights->SetValue("sound", 2);
			SmartScriptTable seataction_lights_lightparts(gEnv->pScriptSystem);
			seataction_lights_lightparts->PushBack("searchLight");
			seataction_lights->SetValue("LightParts", seataction_lights_lightparts);
			seataction->SetValue("Lights", seataction_lights);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* gunner = new VehicleSeat();
		gunner->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "gunner", gunner });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat1");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger_enter_pos");
		seat->SetValue("exitHelper", "exitPassenger03");
		seat->SetValue("sitHelper", "passenger03_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-45, 0, -70));
			view->SetValue("rotationMax", Vec3(25, 0, 70));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 0.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
		seat->SetValue("exitHelper", "exitPassenger04");
		seat->SetValue("sitHelper", "passenger04_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-45, 0, -70));
			view->SetValue("rotationMax", Vec3(25, 0, 70));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 0.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
		seat->SetValue("exitHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger05_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 3);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-45, 0, -70));
			view->SetValue("rotationMax", Vec3(25, 0, 70));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 0.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
		seat->SetValue("exitHelper", "exitPassenger06");
		seat->SetValue("sitHelper", "passenger06_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-45, 0, -70));
			view->SetValue("rotationMax", Vec3(25, 0, 70));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 0.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
		seat->SetValue("exitHelper", "exitPassenger07");
		seat->SetValue("sitHelper", "passenger07_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-45, 0, -70));
			view->SetValue("rotationMax", Vec3(25, 0, 70));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 0.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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
		seat->SetValue("exitHelper", "passenger_enter_pos");
		seat->SetValue("sitHelper", "passenger08_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 4);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-45, 0, -70));
			view->SetValue("rotationMax", Vec3(25, 0, 70));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 0.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
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

	// SeatGroups
	{
		SmartScriptTable seatGroup(gEnv->pScriptSystem);
		seatGroup->SetValue("name", "Cabin");
		seatGroup->SetValue("keepEngineWarm", true);
		SmartScriptTable seatGroup_seats(gEnv->pScriptSystem);
		seatGroup_seats->PushBack("driver");
		seatGroup_seats->PushBack("gunner");
		seatGroup->SetValue("Seats", seatGroup_seats);

		VehicleSeatGroup* cabin = new VehicleSeatGroup();
		cabin->Init(this, seatGroup);
		m_seatGroups.push_back(cabin);
	}

	// Inventory
	this->SetAmmoCapacity("a2ahomingmissile", 12);

	// Paints
	if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/us_vtol/us_vtol_nk.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/us_vtol/us_vtol.mtl");
	}

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
			action_landinggears->SetValue("altitudeToRetractGears", 18.0f);
			action_landinggears->SetValue("velocityMax", 20.0f);
			action_landinggears->SetValue("isOnlyAutoForPlayer", true);
			action->SetValue("LandingGears", action_landinggears);
			actions->PushBack(action);
		}
		{
			SmartScriptTable action(gEnv->pScriptSystem);
			action->SetValue("class", "AutomaticDoor");
			SmartScriptTable action_activations(gEnv->pScriptSystem);
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				if (m_modName == "MP")
				{
					activation->SetValue("type", "OnUsed");
				}
				else
				{
					activation->SetValue("type", "OnGroundCollision");
				}
				action_activations->PushBack(activation);
			}
			{
				SmartScriptTable activation(gEnv->pScriptSystem);
				activation->SetValue("type", "OnUsed");
				activation->SetValue("param1", "component");
				activation->SetValue("param2", "vtol_door_back");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_automaticdoor(gEnv->pScriptSystem);
			action_automaticdoor->SetValue("animation", "passengerDoors");
			action_automaticdoor->SetValue("timeMax", 1.0f);
			if (m_modName == "MP")
			{
				action_automaticdoor->SetValue("disabled", true);
			}
			else
			{
				action_automaticdoor->SetValue("disabled", false);
			}
			action->SetValue("AutomaticDoor", action_automaticdoor);
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
				activation->SetValue("param2", "frontCockpit");
				activation->SetValue("distance", 3.0f);
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
				activation->SetValue("param2", "Backseat1");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat1");
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
				activation->SetValue("param2", "Backseat2");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat2");
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
				activation->SetValue("param2", "Backseat3");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat3");
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
				activation->SetValue("param2", "Backseat4");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat4");
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
				activation->SetValue("param2", "Backseat5");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
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
				activation->SetValue("param2", "Backseat6");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat6");
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
				activation->SetValue("param2", "BackDoor");
				activation->SetValue("distance", 3.0f);
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("backseat1");
			action_enter_seats->PushBack("backseat2");
			action_enter_seats->PushBack("backseat3");
			action_enter_seats->PushBack("backseat4");
			action_enter_seats->PushBack("backseat5");
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
		damages->SetValue("submergedRatioMax", 0.40000001f);
		damages->SetValue("submergedDamageMult", 2.0f);
		damages->SetValue("collDamageThreshold", 10.0f);
		damages->SetValue("groundCollisionMinMult", 0.0099999998f);
		damages->SetValue("groundCollisionMaxMult", 2.0f);
		damages->SetValue("groundCollisionMinSpeed", 4.0f);
		damages->SetValue("groundCollisionMaxSpeed", 60.0f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.033f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.025f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.0099999998f);
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
			damagemultiplier->SetValue("multiplier", 0.0099999998f);
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
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.25f);
			damagemultiplier->SetValue("splash", 0.25f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "aacannon");
			damagemultiplier->SetValue("multiplier", 0.35f);
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
			damagemultiplier->SetValue("multiplier", 0.0f);
			damagemultiplier->SetValue("splash", 0.0f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "scout_moac");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 2.0f);
			damagemultiplier->SetValue("splash", 0.5f);
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
					damagebehavior_explosion->SetValue("radius", 15.0f);
					damagebehavior_explosion->SetValue("minRadius", 5.0f);
					damagebehavior_explosion->SetValue("minPhysRadius", 2.2f);
					damagebehavior_explosion->SetValue("physRadius", 3.0f);
					damagebehavior_explosion->SetValue("pressure", 1000.0f);
					damagebehavior->SetValue("Explosion", damagebehavior_explosion);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
				Destroy_damagesgroup_damagessubgroups->PushBack(damagessubgroup);
			}
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.0f);
				damagessubgroup->SetValue("randomness", 0.0f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "HitPassenger");
					SmartScriptTable damagebehavior_hitpassenger(gEnv->pScriptSystem);
					damagebehavior_hitpassenger->SetValue("damage", 100);
					damagebehavior_hitpassenger->SetValue("isDamagePercent", true);
					damagebehavior->SetValue("HitPassenger", damagebehavior_hitpassenger);
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
		{
			SmartScriptTable DestroyEngine_damagesgroup(gEnv->pScriptSystem);
			DestroyEngine_damagesgroup->SetValue("name", "DestroyEngine");
			SmartScriptTable DestroyEngine_damagesgroup_damagessubgroups(gEnv->pScriptSystem);
			{
				SmartScriptTable damagessubgroup(gEnv->pScriptSystem);
				damagessubgroup->SetValue("delay", 0.0f);
				damagessubgroup->SetValue("randomness", 0.0f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "MovementNotification");
					SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
					damagebehavior_movementnotification->SetValue("isDamageAlwaysFull", false);
					damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Effect");
					SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
					damagebehavior_effect->SetValue("effect", "EngineDamaged");
					damagebehavior_effect->SetValue("disableAfterExplosion", true);
					damagebehavior->SetValue("Effect", damagebehavior_effect);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "CameraShake");
					SmartScriptTable damagebehavior_camerashake(gEnv->pScriptSystem);
					damagebehavior->SetValue("CameraShake", damagebehavior_camerashake);
					damagessubgroup_damagebehaviors->PushBack(damagebehavior);
				}
				damagessubgroup->SetValue("DamageBehaviors", damagessubgroup_damagebehaviors);
				DestroyEngine_damagesgroup_damagessubgroups->PushBack(damagessubgroup);
			}
			DestroyEngine_damagesgroup->SetValue("DamagesSubGroups", DestroyEngine_damagesgroup_damagessubgroups);
			damages_damagesgroups->PushBack(DestroyEngine_damagesgroup);
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
