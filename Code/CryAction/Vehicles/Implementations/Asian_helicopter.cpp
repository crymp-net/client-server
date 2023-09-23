#include "Asian_helicopter.h"

Asian_helicopter::Asian_helicopter()
{
	// enable Asian_helicopter::Init function
	this->Vehicle::PatchVTable();
}

Asian_helicopter::~Asian_helicopter()
{
}

bool Asian_helicopter::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "helicopter";

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
			SmartScriptTable body_part(gEnv->pScriptSystem);
			body_part->SetValue("name", "body");
			body_part->SetValue("class", "Animated");
			body_part->SetValue("mass", 18000.0f);
			SmartScriptTable body_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable hull_part(gEnv->pScriptSystem);
				hull_part->SetValue("name", "hull");
				hull_part->SetValue("class", "AnimatedJoint");
				SmartScriptTable hull_part_parts(gEnv->pScriptSystem);
				{
					SmartScriptTable shiten_turret_part(gEnv->pScriptSystem);
					shiten_turret_part->SetValue("name", "shiten_turret");
					shiten_turret_part->SetValue("class", "AnimatedJoint");
					SmartScriptTable shiten_turret_part_parts(gEnv->pScriptSystem);
					{
						SmartScriptTable MG_pitch_part(gEnv->pScriptSystem);
						MG_pitch_part->SetValue("name", "MG_pitch");
						MG_pitch_part->SetValue("class", "AnimatedJoint");
						SmartScriptTable MG_pitch_part_helpers(gEnv->pScriptSystem);
						{
							SmartScriptTable mountedGunOutPos_helper(gEnv->pScriptSystem);
							mountedGunOutPos_helper->SetValue("name", "mountedGunOutPos");
							mountedGunOutPos_helper->SetValue("position", Vec3(0, 7.9394999, -0.22400001));
							mountedGunOutPos_helper->SetValue("direction", Vec3(0, 0.98479998, 0.1736));
							MG_pitch_part_helpers->PushBack(mountedGunOutPos_helper);
						}
						{
							SmartScriptTable gunnerEyePos_helper(gEnv->pScriptSystem);
							gunnerEyePos_helper->SetValue("name", "gunnerEyePos");
							gunnerEyePos_helper->SetValue("position", Vec3(-0.1499, 6.4380002, -0.29049999));
							gunnerEyePos_helper->SetValue("direction", Vec3(0, 0.98479998, 0.1736));
							MG_pitch_part_helpers->PushBack(gunnerEyePos_helper);
						}
						{
							SmartScriptTable searchLightPos_helper(gEnv->pScriptSystem);
							searchLightPos_helper->SetValue("name", "searchLightPos");
							searchLightPos_helper->SetValue("position", Vec3(0.2441, 6.4408998, -0.55540001));
							searchLightPos_helper->SetValue("direction", Vec3(0, 0.98479998, 0.1736));
							MG_pitch_part_helpers->PushBack(searchLightPos_helper);
						}
						MG_pitch_part->SetValue("Helpers", MG_pitch_part_helpers);
						SmartScriptTable MG_pitch_part_animatedjoint(gEnv->pScriptSystem);
						SmartScriptTable MG_pitch_part_animatedjoint_rotation(gEnv->pScriptSystem);
						MG_pitch_part_animatedjoint_rotation->SetValue("pitchSpeed", 100.0f);
						SmartScriptTable MG_pitch_part_animatedjoint_rotation_pitchlimits(gEnv->pScriptSystem);
						MG_pitch_part_animatedjoint_rotation_pitchlimits->PushBack(-60.0f);
						MG_pitch_part_animatedjoint_rotation_pitchlimits->PushBack(10.0f);
						MG_pitch_part_animatedjoint_rotation->SetValue("pitchLimits", MG_pitch_part_animatedjoint_rotation_pitchlimits);
						MG_pitch_part_animatedjoint->SetValue("Rotation", MG_pitch_part_animatedjoint_rotation);
						MG_pitch_part->SetValue("AnimatedJoint", MG_pitch_part_animatedjoint);
						shiten_turret_part_parts->PushBack(MG_pitch_part);
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
						shiten_turret_part_parts->PushBack(searchLight_part);
					}
					{
						SmartScriptTable searchLightFlare_part(gEnv->pScriptSystem);
						searchLightFlare_part->SetValue("name", "searchLightFlare");
						searchLightFlare_part->SetValue("class", "Light");
						searchLightFlare_part->SetValue("component", "searchLight");
						searchLightFlare_part->SetValue("helper", "searchLightPos");
						SmartScriptTable searchLightFlare_part_light(gEnv->pScriptSystem);
						searchLightFlare_part_light->SetValue("type", "HeadLightFlare");
						searchLightFlare_part->SetValue("Light", searchLightFlare_part_light);
						shiten_turret_part_parts->PushBack(searchLightFlare_part);
					}
					shiten_turret_part->SetValue("Parts", shiten_turret_part_parts);
					SmartScriptTable shiten_turret_part_animatedjoint(gEnv->pScriptSystem);
					SmartScriptTable shiten_turret_part_animatedjoint_rotation(gEnv->pScriptSystem);
					shiten_turret_part_animatedjoint_rotation->SetValue("yawSpeed", 100.0f);
					SmartScriptTable shiten_turret_part_animatedjoint_rotation_yawlimits(gEnv->pScriptSystem);
					shiten_turret_part_animatedjoint_rotation_yawlimits->PushBack(-135.0f);
					shiten_turret_part_animatedjoint_rotation_yawlimits->PushBack(135.0f);
					shiten_turret_part_animatedjoint_rotation->SetValue("yawLimits", shiten_turret_part_animatedjoint_rotation_yawlimits);
					shiten_turret_part_animatedjoint->SetValue("Rotation", shiten_turret_part_animatedjoint_rotation);
					shiten_turret_part->SetValue("AnimatedJoint", shiten_turret_part_animatedjoint);
					hull_part_parts->PushBack(shiten_turret_part);
				}
				hull_part->SetValue("Parts", hull_part_parts);
				SmartScriptTable hull_part_helpers(gEnv->pScriptSystem);
				hull_part->SetValue("Helpers", hull_part_helpers);
				body_part_parts->PushBack(hull_part);
			}
			body_part->SetValue("Parts", body_part_parts);
			SmartScriptTable body_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable gunner_enter_pos_1_helper(gEnv->pScriptSystem);
				gunner_enter_pos_1_helper->SetValue("name", "gunner_enter_pos_1");
				gunner_enter_pos_1_helper->SetValue("position", Vec3(-1.5259, 4.7329001, 0.0625));
				gunner_enter_pos_1_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(gunner_enter_pos_1_helper);
			}
			{
				SmartScriptTable gunner_enter_pos_2_helper(gEnv->pScriptSystem);
				gunner_enter_pos_2_helper->SetValue("name", "gunner_enter_pos_2");
				gunner_enter_pos_2_helper->SetValue("position", Vec3(8, 4.6001, 0.80949998));
				gunner_enter_pos_2_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(gunner_enter_pos_2_helper);
			}
			{
				SmartScriptTable gunner_sit_pos_helper(gEnv->pScriptSystem);
				gunner_sit_pos_helper->SetValue("name", "gunner_sit_pos");
				body_part_helpers->PushBack(gunner_sit_pos_helper);
			}
			{
				SmartScriptTable pilot_enter_pos_1_helper(gEnv->pScriptSystem);
				pilot_enter_pos_1_helper->SetValue("name", "pilot_enter_pos_1");
				pilot_enter_pos_1_helper->SetValue("position", Vec3(-8, 3.2859001, 0.50950003));
				pilot_enter_pos_1_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(pilot_enter_pos_1_helper);
			}
			{
				SmartScriptTable pilot_enter_pos_2_helper(gEnv->pScriptSystem);
				pilot_enter_pos_2_helper->SetValue("name", "pilot_enter_pos_2");
				pilot_enter_pos_2_helper->SetValue("position", Vec3(1.8091, 3.2859001, 0.0625));
				pilot_enter_pos_2_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(pilot_enter_pos_2_helper);
			}
			{
				SmartScriptTable pilot_eye_pos_helper(gEnv->pScriptSystem);
				pilot_eye_pos_helper->SetValue("name", "pilot_eye_pos");
				pilot_eye_pos_helper->SetValue("position", Vec3(0, 3.5249, 1.5445));
				pilot_eye_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(pilot_eye_pos_helper);
			}
			{
				SmartScriptTable pilot_sit_pos_helper(gEnv->pScriptSystem);
				pilot_sit_pos_helper->SetValue("name", "pilot_sit_pos");
				body_part_helpers->PushBack(pilot_sit_pos_helper);
			}
			{
				SmartScriptTable Attachment_01_pos_helper(gEnv->pScriptSystem);
				Attachment_01_pos_helper->SetValue("name", "Attachment_01_pos");
				Attachment_01_pos_helper->SetValue("position", Vec3(2.79, 1.3999, 1.5595));
				Attachment_01_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(Attachment_01_pos_helper);
			}
			{
				SmartScriptTable Attachment_02_pos_helper(gEnv->pScriptSystem);
				Attachment_02_pos_helper->SetValue("name", "Attachment_02_pos");
				Attachment_02_pos_helper->SetValue("position", Vec3(-2.79, 1.3999, 1.5595));
				Attachment_02_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(Attachment_02_pos_helper);
			}
			{
				SmartScriptTable mountGunTurretPos_helper(gEnv->pScriptSystem);
				mountGunTurretPos_helper->SetValue("name", "mountGunTurretPos");
				mountGunTurretPos_helper->SetValue("position", Vec3(0, 5.6802001, 0.44949999));
				mountGunTurretPos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(mountGunTurretPos_helper);
			}
			{
				SmartScriptTable engineSmokeOut_helper(gEnv->pScriptSystem);
				engineSmokeOut_helper->SetValue("name", "engineSmokeOut");
				engineSmokeOut_helper->SetValue("position", Vec3(0, -0.89429998, 2.4389));
				engineSmokeOut_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(engineSmokeOut_helper);
			}
			{
				SmartScriptTable rotorSmokeOut_helper(gEnv->pScriptSystem);
				rotorSmokeOut_helper->SetValue("name", "rotorSmokeOut");
				rotorSmokeOut_helper->SetValue("position", Vec3(0, -12.178, 3.9584999));
				rotorSmokeOut_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(rotorSmokeOut_helper);
			}
			{
				SmartScriptTable rocketLauncherOut_left1_helper(gEnv->pScriptSystem);
				rocketLauncherOut_left1_helper->SetValue("name", "rocketLauncherOut_left1");
				rocketLauncherOut_left1_helper->SetValue("position", Vec3(-1.575, -0.60299999, 0.53049999));
				rocketLauncherOut_left1_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(rocketLauncherOut_left1_helper);
			}
			{
				SmartScriptTable rocketLauncherOut_left2_helper(gEnv->pScriptSystem);
				rocketLauncherOut_left2_helper->SetValue("name", "rocketLauncherOut_left2");
				rocketLauncherOut_left2_helper->SetValue("position", Vec3(-2.2409999, -0.60600001, 0.38049999));
				rocketLauncherOut_left2_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(rocketLauncherOut_left2_helper);
			}
			{
				SmartScriptTable rocketLauncherOut_right1_helper(gEnv->pScriptSystem);
				rocketLauncherOut_right1_helper->SetValue("name", "rocketLauncherOut_right1");
				rocketLauncherOut_right1_helper->SetValue("position", Vec3(1.575, -0.60299999, 0.53049999));
				rocketLauncherOut_right1_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(rocketLauncherOut_right1_helper);
			}
			{
				SmartScriptTable rocketLauncherOut_right2_helper(gEnv->pScriptSystem);
				rocketLauncherOut_right2_helper->SetValue("name", "rocketLauncherOut_right2");
				rocketLauncherOut_right2_helper->SetValue("position", Vec3(2.2409999, -0.60600001, 0.38049999));
				rocketLauncherOut_right2_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(rocketLauncherOut_right2_helper);
			}
			{
				SmartScriptTable engine_exhaust_left_helper(gEnv->pScriptSystem);
				engine_exhaust_left_helper->SetValue("name", "engine_exhaust_left");
				engine_exhaust_left_helper->SetValue("position", Vec3(-0.59399998, -1.8789001, 3.2985001));
				engine_exhaust_left_helper->SetValue("direction", Vec3(0, -0.90609998, -0.42300001));
				body_part_helpers->PushBack(engine_exhaust_left_helper);
			}
			{
				SmartScriptTable engine_exhaust_right_helper(gEnv->pScriptSystem);
				engine_exhaust_right_helper->SetValue("name", "engine_exhaust_right");
				engine_exhaust_right_helper->SetValue("position", Vec3(0.56199998, -1.75, 3.3385));
				engine_exhaust_right_helper->SetValue("direction", Vec3(0, -0.9659, -0.259));
				body_part_helpers->PushBack(engine_exhaust_right_helper);
			}
			{
				SmartScriptTable wind_emitter_helper(gEnv->pScriptSystem);
				wind_emitter_helper->SetValue("name", "wind_emitter");
				wind_emitter_helper->SetValue("position", Vec3(0, 0, 6.5885));
				wind_emitter_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(wind_emitter_helper);
			}
			{
				SmartScriptTable landing_pos_helper(gEnv->pScriptSystem);
				landing_pos_helper->SetValue("name", "landing_pos");
				landing_pos_helper->SetValue("position", Vec3(0, 0, -0.2377));
				landing_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(landing_pos_helper);
			}
			{
				SmartScriptTable cockpitdoors_pos_helper(gEnv->pScriptSystem);
				cockpitdoors_pos_helper->SetValue("name", "cockpitdoors_pos");
				cockpitdoors_pos_helper->SetValue("position", Vec3(-1.0608, 4.1978002, 1.11));
				cockpitdoors_pos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(cockpitdoors_pos_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, -3.0548999, 1.2778));
				burningPos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(burningPos_helper);
			}
			{
				SmartScriptTable leftWingExp_helper(gEnv->pScriptSystem);
				leftWingExp_helper->SetValue("name", "leftWingExp");
				leftWingExp_helper->SetValue("position", Vec3(-2.2102001, -1.7012, 0.97729999));
				leftWingExp_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(leftWingExp_helper);
			}
			body_part->SetValue("Helpers", body_part_helpers);
			SmartScriptTable body_part_animated(gEnv->pScriptSystem);
			if (m_modName == "LowBudget")
			{
				body_part_animated->SetValue("filename", "Objects/Vehicles/asian_helicopter_low_budget/asian_helicopter_low_budget_flying.cga");
			}
			else
			{
				body_part_animated->SetValue("filename", "Objects/Vehicles/Asian_Helicopter/asian_helicopter.cga");
			}
			body_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/Asian_Helicopter/asian_helicopter_destroyed.cga");
			body_part->SetValue("Animated", body_part_animated);
			parts->PushBack(body_part);
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
		component->SetValue("name", "LeftWing");
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 150.0f);
		}
		else
		{
			component->SetValue("damageMax", 150.0f);
		}
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-3.7014, -4.5419998, 0.0054000001));
		component->SetValue("maxBound", Vec3(-1.3015, 0.54149997, 1.7254));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 1.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.050000001f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "LeftWingExplosion");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Impulse");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("damageRatioMax", 2.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", false);
			SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
			damagebehavior_impulse->SetValue("forceMin", 2.0f);
			damagebehavior_impulse->SetValue("forceMax", 4.0f);
			damagebehavior_impulse->SetValue("direction", Vec3(0, 0, 1));
			damagebehavior_impulse->SetValue("momentum", Vec3(0, 4, 0));
			damagebehavior->SetValue("Impulse", damagebehavior_impulse);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "AISignal");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_aisignal(gEnv->pScriptSystem);
			damagebehavior_aisignal->SetValue("signalId", 1);
			damagebehavior_aisignal->SetValue("signalText", "HELI_HOVERATTACK2_GETPARTSDAMAGE");
			damagebehavior->SetValue("AISignal", damagebehavior_aisignal);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* leftWing = new VehicleComponent();
		leftWing->Init(this, component);
		m_components.push_back(leftWing);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "RightWing");
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 150.0f);
		}
		else
		{
			component->SetValue("damageMax", 150.0f);
		}
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(1.3015, -4.5419998, 0.0054000001));
		component->SetValue("maxBound", Vec3(3.7014, 0.54149997, 1.7254));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.75f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 3.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.050000001f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "RightWingExplosion");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Impulse");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("damageRatioMax", 2.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", false);
			SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
			damagebehavior_impulse->SetValue("forceMin", 2.0f);
			damagebehavior_impulse->SetValue("forceMax", 4.0f);
			damagebehavior_impulse->SetValue("direction", Vec3(0, 0, 1));
			damagebehavior_impulse->SetValue("momentum", Vec3(0, -4, 0));
			damagebehavior->SetValue("Impulse", damagebehavior_impulse);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "AISignal");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_aisignal(gEnv->pScriptSystem);
			damagebehavior_aisignal->SetValue("signalId", 1);
			damagebehavior_aisignal->SetValue("signalText", "HELI_HOVERATTACK2_GETPARTSDAMAGE");
			damagebehavior->SetValue("AISignal", damagebehavior_aisignal);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* rightWing = new VehicleComponent();
		rightWing->Init(this, component);
		m_components.push_back(rightWing);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "BackRotor");
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 150.0f);
		}
		else
		{
			component->SetValue("damageMax", 150.0f);
		}
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.6001, -13.939, 1.7024));
		component->SetValue("maxBound", Vec3(1.6001, -10.5796, 5.3024001));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.75f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.5f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 3.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.30000001f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 0.5f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "BackRotorDamaged");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Effect");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "BackRotorExplosion");
			damagebehavior_effect->SetValue("disableAfterExplosion", true);
			damagebehavior->SetValue("Effect", damagebehavior_effect);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "MovementNotification");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_movementnotification(gEnv->pScriptSystem);
			damagebehavior_movementnotification->SetValue("isSteering", true);
			damagebehavior->SetValue("MovementNotification", damagebehavior_movementnotification);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "AISignal");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			damagebehavior->SetValue("ignoreVehicleDestruction", true);
			SmartScriptTable damagebehavior_aisignal(gEnv->pScriptSystem);
			damagebehavior_aisignal->SetValue("signalId", 1);
			damagebehavior_aisignal->SetValue("signalText", "TO_HELI_EMERGENCYLANDING");
			damagebehavior->SetValue("AISignal", damagebehavior_aisignal);
			component_damagebehaviors->PushBack(damagebehavior);
		}
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* backRotor = new VehicleComponent();
		backRotor->Init(this, component);
		m_components.push_back(backRotor);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Rotor");
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 200.0f);
		}
		else
		{
			component->SetValue("damageMax", 200.0f);
		}
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-1.75, -3.0576, 2.0325999));
		component->SetValue("maxBound", Vec3(1.75, 1.9277, 3.6919));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			damagemultiplier->SetValue("multiplier", 0.02f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.02f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "tac");
			damagemultiplier->SetValue("multiplier", 3.0f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "melee");
			damagemultiplier->SetValue("multiplier", 0.2f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "gaussbullet");
			damagemultiplier->SetValue("multiplier", 0.05f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		component->SetValue("DamageMultipliers", component_damagemultipliers);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		{
			SmartScriptTable damagebehavior(gEnv->pScriptSystem);
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
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

		VehicleComponent* rotor = new VehicleComponent();
		rotor->Init(this, component);
		m_components.push_back(rotor);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Hull");
		if (m_modName == "MP")
		{
			component->SetValue("damageMax", 300.0f);
		}
		else
		{
			component->SetValue("damageMax", 300.0f);
		}
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-8.8309450, -14.015456, -0.94689238));
		component->SetValue("maxBound", Vec3(9.0925131, 8.9827299, 5.4283361));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "moacbullet");
			damagemultiplier->SetValue("multiplier", 0.02f);
			component_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "frag");
			damagemultiplier->SetValue("multiplier", 0.25f);
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
			damagebehavior->SetValue("damageRatioMin", 0.75f);
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
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* hull = new VehicleComponent();
		hull->Init(this, component);
		m_components.push_back(hull);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "Cockpit");
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* cockpit = new VehicleComponent();
		cockpit->Init(this, component);
		m_components.push_back(cockpit);
	}
	{
		SmartScriptTable component(gEnv->pScriptSystem);
		component->SetValue("name", "RotorTails");
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-9.4877005, -9.1875, 3.0402999));
		component->SetValue("maxBound", Vec3(9.4856997, 8.0595999, 3.9052999));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.0f);
		SmartScriptTable component_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 1.0f);
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

		VehicleComponent* rotorTails = new VehicleComponent();
		rotorTails->Init(this, component);
		m_components.push_back(rotorTails);
	}

	this->RegisterComponentParts(componentParts);

	// Particles
	{
		SmartScriptTable particles(gEnv->pScriptSystem);
		SmartScriptTable particles_exhaust(gEnv->pScriptSystem);
		particles_exhaust->SetValue("insideWater", false);
		particles_exhaust->SetValue("outsideWater", true);
		SmartScriptTable particles_exhaust_helpers(gEnv->pScriptSystem);
		particles_exhaust_helpers->PushBack("engine_exhaust_left");
		particles_exhaust_helpers->PushBack("engine_exhaust_right");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "vehicle_fx.vehicle_exhaust.chopper_start");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "vehicle_fx.vehicle_exhaust.smoke_dark");
		particles_exhaust->SetValue("EngineStop", particles_exhaust_enginestop);
		SmartScriptTable particles_exhaust_enginerunning(gEnv->pScriptSystem);
		particles_exhaust_enginerunning->SetValue("effect", "vehicle_fx.vehicle_exhaust.chopper_running");
		particles_exhaust_enginerunning->SetValue("boostEffect", "");
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
			EngineDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_engine_smoke_a");
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.helicopter_explosion.a");
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable LeftWingExplosion_damageeffect(gEnv->pScriptSystem);
			LeftWingExplosion_damageeffect->SetValue("name", "LeftWingExplosion");
			LeftWingExplosion_damageeffect->SetValue("helper", "LeftWing");
			LeftWingExplosion_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_wing_explosion");
			particles_damageeffects->PushBack(LeftWingExplosion_damageeffect);
		}
		{
			SmartScriptTable RightWingExplosion_damageeffect(gEnv->pScriptSystem);
			RightWingExplosion_damageeffect->SetValue("name", "RightWingExplosion");
			RightWingExplosion_damageeffect->SetValue("helper", "RightWing");
			RightWingExplosion_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_wing_explosion");
			particles_damageeffects->PushBack(RightWingExplosion_damageeffect);
		}
		{
			SmartScriptTable LeftWingDamaged_damageeffect(gEnv->pScriptSystem);
			LeftWingDamaged_damageeffect->SetValue("name", "LeftWingDamaged");
			LeftWingDamaged_damageeffect->SetValue("helper", "LeftWing");
			LeftWingDamaged_damageeffect->SetValue("effect", "explosions.helicopter_explosion.debris_smoke");
			particles_damageeffects->PushBack(LeftWingDamaged_damageeffect);
		}
		{
			SmartScriptTable RightWingDamaged_damageeffect(gEnv->pScriptSystem);
			RightWingDamaged_damageeffect->SetValue("name", "RightWingDamaged");
			RightWingDamaged_damageeffect->SetValue("helper", "RightWing");
			RightWingDamaged_damageeffect->SetValue("effect", "explosions.helicopter_explosion.debris_smoke");
			particles_damageeffects->PushBack(RightWingDamaged_damageeffect);
		}
		{
			SmartScriptTable RotorExplosion_damageeffect(gEnv->pScriptSystem);
			RotorExplosion_damageeffect->SetValue("name", "RotorExplosion");
			RotorExplosion_damageeffect->SetValue("helper", "Rotor");
			RotorExplosion_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_wing_explosion");
			particles_damageeffects->PushBack(RotorExplosion_damageeffect);
		}
		{
			SmartScriptTable RotorDamaged_damageeffect(gEnv->pScriptSystem);
			RotorDamaged_damageeffect->SetValue("name", "RotorDamaged");
			RotorDamaged_damageeffect->SetValue("helper", "Rotor");
			RotorDamaged_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_backroter_smoke_b");
			particles_damageeffects->PushBack(RotorDamaged_damageeffect);
		}
		{
			SmartScriptTable BackRotorDamaged_damageeffect(gEnv->pScriptSystem);
			BackRotorDamaged_damageeffect->SetValue("name", "BackRotorDamaged");
			BackRotorDamaged_damageeffect->SetValue("helper", "BackRotor");
			BackRotorDamaged_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_backroter_smoke_b");
			particles_damageeffects->PushBack(BackRotorDamaged_damageeffect);
		}
		{
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "burningPos");
			HullDamaged25_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_hull_smoke_a");
			HullDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			HullDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			HullDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged75_damageeffect(gEnv->pScriptSystem);
			EngineDamaged75_damageeffect->SetValue("name", "EngineDamaged75");
			EngineDamaged75_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_engine_smoke_b");
			EngineDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged75_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "Vehicle_fx.Damages_Sky.NKheli_hull_smoke_b");
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
			environmentlayer->SetValue("minPowerSizeScale", 0.5f);
			environmentlayer->SetValue("minPowerCountScale", 0.5f);
			environmentlayer->SetValue("maxPowerSizeScale", 1.0f);
			environmentlayer->SetValue("maxPowerCountScale", 1.0f);
			SmartScriptTable environmentlayer_alignment(gEnv->pScriptSystem);
			environmentlayer_alignment->SetValue("alignGroundHeight", 35.0f);
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
		animation->SetValue("name", "rotor");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable default_state(gEnv->pScriptSystem);
			default_state->SetValue("name", "default");
			if (m_modName == "LowBudget")
			{
				default_state->SetValue("animation", "helicopter_low_budget_flying_rotate_mainrotor");
			}
			else
			{
				default_state->SetValue("animation", "helicopter_rotate_mainrotor");
			}
			default_state->SetValue("sound", "sounds/vehicles:asian_helicopter:rotor");
			default_state->SetValue("soundHelper", "rotorSmokeOut");
			default_state->SetValue("isLooped", true);
			default_state->SetValue("speedDefault", 0.0f);
			default_state->SetValue("speedMin", 0.0f);
			default_state->SetValue("speedMax", 1.0f);
			SmartScriptTable default_state_materials(gEnv->pScriptSystem);
			{
				SmartScriptTable rotorblades_material(gEnv->pScriptSystem);
				rotorblades_material->SetValue("name", "rotorblades");
				rotorblades_material->SetValue("setting", "opacity");
				rotorblades_material->SetValue("invertValue", true);
				default_state_materials->PushBack(rotorblades_material);
			}
			{
				SmartScriptTable motion_blur_material(gEnv->pScriptSystem);
				motion_blur_material->SetValue("name", "motion_blur");
				motion_blur_material->SetValue("setting", "opacity");
				motion_blur_material->SetValue("invertValue", false);
				default_state_materials->PushBack(motion_blur_material);
			}
			default_state->SetValue("Materials", default_state_materials);
			animation_states->PushBack(default_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* rotor = new VehicleAnimation();
		rotor->Init(this, animation);
		m_animations.push_back({ "rotor", rotor });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "landingGears");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable closed_state(gEnv->pScriptSystem);
			closed_state->SetValue("name", "closed");
			if (m_modName == "LowBudget")
			{
				closed_state->SetValue("animation", "helicopter_low_budget_flying_landinggears_closing");
			}
			else
			{
				closed_state->SetValue("animation", "helicopter_landinggears_closing");
			}
			closed_state->SetValue("sound", "sounds/vehicles:asian_helicopter:gear_in");
			closed_state->SetValue("soundHelper", "landing_pos");
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
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable closed_state(gEnv->pScriptSystem);
			closed_state->SetValue("name", "closed");
			closed_state->SetValue("animation", "helicopter_sidedoors_closing");
			if (m_modName == "LowBudget")
			{
				closed_state->SetValue("sound", "");
			}
			else
			{
				closed_state->SetValue("sound", "sounds/vehicles:asian_helicopter:sidedoor_close");
			}
			closed_state->SetValue("soundHelper", "");
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
		animation->SetValue("name", "cockpitDoors");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable closed_state(gEnv->pScriptSystem);
			closed_state->SetValue("name", "closed");
			closed_state->SetValue("animation", "helicopter_cockpit_closing");
			if (m_modName == "LowBudget")
			{
				closed_state->SetValue("sound", "");
			}
			else
			{
				closed_state->SetValue("sound", "sounds/vehicles:asian_helicopter:door_open");
			}
			closed_state->SetValue("soundHelper", "cockpitdoors_pos");
			closed_state->SetValue("isLooped", false);
			closed_state->SetValue("speedDefault", 1.0f);
			closed_state->SetValue("speedMin", 1.0f);
			closed_state->SetValue("speedMax", 1.0f);
			animation_states->PushBack(closed_state);
		}
		animation->SetValue("States", animation_states);

		VehicleAnimation* cockpitDoors = new VehicleAnimation();
		cockpitDoors->Init(this, animation);
		m_animations.push_back({ "cockpitDoors", cockpitDoors });
	}
	{
		SmartScriptTable animation(gEnv->pScriptSystem);
		animation->SetValue("name", "mg_fire");
		animation->SetValue("part", "body");
		SmartScriptTable animation_states(gEnv->pScriptSystem);
		{
			SmartScriptTable shoot_state(gEnv->pScriptSystem);
			shoot_state->SetValue("name", "shoot");
			if (m_modName == "LowBudget")
			{
				shoot_state->SetValue("animation", "helicopter_low_budget_flying_MG_fire");
			}
			else
			{
				shoot_state->SetValue("animation", "helicopter_MG_fire");
			}
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
		movement->SetValue("engineWarmupDelay", 3.0f);
		movement->SetValue("enginePowerMax", 2.8f);
		movement->SetValue("rotationDamping", 20.0f);
		movement->SetValue("altitudeMax", 500.0f);
		movement->SetValue("rotorDiskTiltScale", 0.44999999f);
		movement->SetValue("pitchResponsiveness", 19.0f);
		movement->SetValue("rollResponsiveness", 18.0f);
		movement->SetValue("yawResponsiveness", 29.0f);
		movement->SetValue("maxYawRate", 3.142f);
		movement->SetValue("maxFwdSpeed", 35.0f);
		movement->SetValue("maxLeftSpeed", 36.0f);
		movement->SetValue("maxSpeed", 9.0f);
		movement->SetValue("maxUpSpeed", 15.0f);
		movement->SetValue("basicSpeedFraction", 0.69999999f);
		movement->SetValue("yawDecreaseWithSpeed", 12.0f);
		movement->SetValue("tiltPerVelDifference", 0.039999999f);
		movement->SetValue("maxTiltAngle", 0.55000001f);
		movement->SetValue("maxRollAngle", 45.0f);
		movement->SetValue("extraRollForTurn", 0.5f);
		movement->SetValue("rollForTurnForce", 0.5f);
		movement->SetValue("yawPerRoll", -0.1f);
		movement->SetValue("pitchActionPerTilt", 5.0f);
		movement->SetValue("pitchInputConst", 3.0f);
		movement->SetValue("powerInputConst", 1.0f);
		movement->SetValue("powerInputDamping", -0.75f);
		movement->SetValue("relaxForce", 2.0f);
		movement->SetValue("yawInputConst", 4.0f);
		movement->SetValue("yawInputDamping", -0.5f);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "engineSmokeOut");
		movement_soundparams->SetValue("runSoundDelay", 1.8f);
		movement_soundparams->SetValue("rpmPitchSpeed", 1.2f);
		movement->SetValue("SoundParams", movement_soundparams);
		SmartScriptTable movement_tweakgroups(gEnv->pScriptSystem);
		{
			SmartScriptTable player_tweakgroup(gEnv->pScriptSystem);
			player_tweakgroup->SetValue("name", "player");
			SmartScriptTable player_tweakgroup_tweaks(gEnv->pScriptSystem);
			{
				SmartScriptTable powerInputConst_tweak(gEnv->pScriptSystem);
				powerInputConst_tweak->SetValue("name", "powerInputConst");
				powerInputConst_tweak->SetValue("value", 1.0f);
				powerInputConst_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(powerInputConst_tweak);
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
				SmartScriptTable rotationDamping_tweak(gEnv->pScriptSystem);
				rotationDamping_tweak->SetValue("name", "rotationDamping");
				rotationDamping_tweak->SetValue("value", 27.0f);
				rotationDamping_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(rotationDamping_tweak);
			}
			{
				SmartScriptTable pitchActionPerTilt_tweak(gEnv->pScriptSystem);
				pitchActionPerTilt_tweak->SetValue("name", "pitchActionPerTilt");
				pitchActionPerTilt_tweak->SetValue("value", 0.2f);
				pitchActionPerTilt_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(pitchActionPerTilt_tweak);
			}
			{
				SmartScriptTable rollResponsiveness_tweak(gEnv->pScriptSystem);
				rollResponsiveness_tweak->SetValue("name", "rollResponsiveness");
				rollResponsiveness_tweak->SetValue("value", 7.0f);
				rollResponsiveness_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(rollResponsiveness_tweak);
			}
			{
				SmartScriptTable pitchResponsiveness_tweak(gEnv->pScriptSystem);
				pitchResponsiveness_tweak->SetValue("name", "pitchResponsiveness");
				pitchResponsiveness_tweak->SetValue("value", 5.0f);
				pitchResponsiveness_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(pitchResponsiveness_tweak);
			}
			{
				SmartScriptTable yawResponsiveness_tweak(gEnv->pScriptSystem);
				yawResponsiveness_tweak->SetValue("name", "yawResponsiveness");
				yawResponsiveness_tweak->SetValue("value", 10.0f);
				yawResponsiveness_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(yawResponsiveness_tweak);
			}
			{
				SmartScriptTable yawPerRoll_tweak(gEnv->pScriptSystem);
				yawPerRoll_tweak->SetValue("name", "yawPerRoll");
				yawPerRoll_tweak->SetValue("value", 2.5f);
				yawPerRoll_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(yawPerRoll_tweak);
			}
			{
				SmartScriptTable extraRollForTurn_tweak(gEnv->pScriptSystem);
				extraRollForTurn_tweak->SetValue("name", "extraRollForTurn");
				extraRollForTurn_tweak->SetValue("value", 15.0f);
				extraRollForTurn_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(extraRollForTurn_tweak);
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
				maxYawRate_tweak->SetValue("value", 2.5f);
				maxYawRate_tweak->SetValue("op", 1);
				player_tweakgroup_tweaks->PushBack(maxYawRate_tweak);
			}
			{
				SmartScriptTable relaxForce_tweak(gEnv->pScriptSystem);
				relaxForce_tweak->SetValue("name", "relaxForce");
				relaxForce_tweak->SetValue("value", 0.75f);
				relaxForce_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(relaxForce_tweak);
			}
			{
				SmartScriptTable enginePowerMax_tweak(gEnv->pScriptSystem);
				enginePowerMax_tweak->SetValue("name", "enginePowerMax");
				enginePowerMax_tweak->SetValue("value", 2.0999999f);
				enginePowerMax_tweak->SetValue("op", 0);
				player_tweakgroup_tweaks->PushBack(enginePowerMax_tweak);
			}
			player_tweakgroup->SetValue("Tweaks", player_tweakgroup_tweaks);
			movement_tweakgroups->PushBack(player_tweakgroup);
		}
		movement->SetValue("TweakGroups", movement_tweakgroups);

		if (!this->SetMovement("Helicopter", movement))
		{
			return false;
		}
	}

	SmartScriptTable seats(gEnv->pScriptSystem);

	// Seats
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "pilot");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "pilot_enter_pos_2");
		seat->SetValue("sitHelper", "passenger02_helper_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("isPassengerShielded", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("remotelyUseActionsFromSeat", "");
		seat->SetValue("blurMult", 0.0f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			view->SetValue("rotationMin", Vec3(-20, 0, -80));
			view->SetValue("rotationMax", Vec3(10, 0, 80));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, -0.2, 0.12));
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
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
			view_actionthirdperson->SetValue("cameraPosOffset", Vec3(0, 2, 1));
			view_actionthirdperson->SetValue("cameraAimOffset", Vec3(0, 0, 0));
			view_actionthirdperson->SetValue("lagSpeed", 5.0f);
			view_actionthirdperson->SetValue("velocityMult", Vec3(0.0099999998, 0.0099999998, 0.0099999998));
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
				weapon->SetValue("class", "AsianCoaxialGun");
				weapon->SetValue("part", "hull");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("rocketLauncherOut_right2");
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
				weapon->SetValue("class", "Hellfire");
				weapon->SetValue("part", "RocketLauncher1");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("rocketLauncherOut_left1");
				weapon_helpers->PushBack("rocketLauncherOut_right1");
				weapon_helpers->PushBack("rocketLauncherOut_left2");
				weapon_helpers->PushBack("rocketLauncherOut_right2");
				weapon->SetValue("Helpers", weapon_helpers);
				seataction_weapons_weapons->PushBack(weapon);
			}
			seataction_weapons->SetValue("Weapons", seataction_weapons_weapons);
			seataction->SetValue("Weapons", seataction_weapons);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 0.0f);
		seat_sounds->SetValue("mood", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* pilot = new VehicleSeat();
		pilot->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "pilot", pilot });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "gunner");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "gunner_enter_pos_1");
		seat->SetValue("sitHelper", "passenger01_helper_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
		seat->SetValue("blurMult", 0.0f);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "gunnerEyePos");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 15.0f);
			view_thirdperson->SetValue("speed", 0.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "RotateTurret");
			seataction->SetValue("isAvailableRemotely", false);
			SmartScriptTable seataction_rotateturret(gEnv->pScriptSystem);
			seataction_rotateturret->SetValue("pitchPart", "MG_pitch");
			seataction_rotateturret->SetValue("yawPart", "shiten_turret");
			seataction->SetValue("RotateTurret", seataction_rotateturret);
			seat_seatactions->PushBack(seataction);
		}
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "Weapons");
			seataction->SetValue("isAvailableRemotely", false);
			SmartScriptTable seataction_weapons(gEnv->pScriptSystem);
			SmartScriptTable seataction_weapons_weapons(gEnv->pScriptSystem);
			{
				SmartScriptTable weapon(gEnv->pScriptSystem);
				weapon->SetValue("class", "Asian50Cal");
				weapon->SetValue("part", "MG_pitch");
				SmartScriptTable weapon_helpers(gEnv->pScriptSystem);
				weapon_helpers->PushBack("mountedGunOutPos");
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
			seataction_lights->SetValue("sound", 3);
			SmartScriptTable seataction_lights_lightparts(gEnv->pScriptSystem);
			seataction_lights_lightparts->PushBack("searchLight");
			seataction_lights_lightparts->PushBack("searchLightFlare");
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

		VehicleSeat* gunner = new VehicleSeat();
		gunner->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "gunner", gunner });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "backseat1");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger03_helper_pos");
		seat->SetValue("exitHelper", "exitPassenger03_helper_pos");
		seat->SetValue("sitHelper", "passenger03_helper_pos");
		seat->SetValue("isDriver", false);
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
			view_firstperson->SetValue("helper", "");
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
		seat->SetValue("enterHelper", "passenger04_helper_pos");
		seat->SetValue("exitHelper", "exitPassenger04_helper_pos");
		seat->SetValue("sitHelper", "passenger04_helper_pos");
		seat->SetValue("isDriver", false);
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
			view_firstperson->SetValue("helper", "");
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
		seat->SetValue("enterHelper", "passenger05_helper_pos");
		seat->SetValue("exitHelper", "exitPassenger05_helper_pos");
		seat->SetValue("sitHelper", "passenger05_helper_pos");
		seat->SetValue("isDriver", false);
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
			view_firstperson->SetValue("helper", "");
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
		seat->SetValue("enterHelper", "passenger06_helper_pos");
		seat->SetValue("exitHelper", "exitPassenger06_helper_pos");
		seat->SetValue("sitHelper", "passenger06_helper_pos");
		seat->SetValue("isDriver", false);
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
			view_firstperson->SetValue("helper", "");
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
		seat->SetValue("enterHelper", "passenger07_helper_pos");
		seat->SetValue("exitHelper", "exitPassenger07_helper_pos");
		seat->SetValue("sitHelper", "passenger07_helper_pos");
		seat->SetValue("isDriver", false);
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
			view_firstperson->SetValue("helper", "");
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
		seat->SetValue("enterHelper", "passenger08_helper_pos");
		seat->SetValue("exitHelper", "exitPassenger08_helper_pos");
		seat->SetValue("sitHelper", "passenger08_helper_pos");
		seat->SetValue("isDriver", false);
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
			view_firstperson->SetValue("helper", "");
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

	// Inventory
	this->SetAmmoCapacity("helicoptermissile", 28);

	// Paints
	if (m_paintName == "nk")
	{
		this->SetPaintMaterial("objects/vehicles/asian_helicopter/asian_helicopter.mtl");
	}
	else if (m_paintName == "us")
	{
		this->SetPaintMaterial("objects/vehicles/asian_helicopter/asian_helicopter_us.mtl");
	}
	else if (m_paintName == "sp")
	{
		this->SetPaintMaterial("objects/vehicles/asian_helicopter/asian_helicopter_sp.mtl");
	}
	else if (m_paintName == "lb")
	{
		this->SetPaintMaterial("Objects/Vehicles/asian_helicopter_low_budget/asian_helicopter.mtl");
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
			action_landinggears->SetValue("landingDamages", 500.0f);
			action_landinggears->SetValue("altitudeToRetractGears", 8.0f);
			action_landinggears->SetValue("blockPartRotation", "MG_pitch");
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
			if (m_modName == "LowBudget")
			{
				action_automaticdoor->SetValue("animation", "");
			}
			else
			{
				action_automaticdoor->SetValue("animation", "passengerDoors");
			}
			action_automaticdoor->SetValue("timeMax", 2.0f);
			action->SetValue("AutomaticDoor", action_automaticdoor);
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
			if (m_modName == "LowBudget")
			{
				action_automaticdoor->SetValue("animation", "");
			}
			else
			{
				action_automaticdoor->SetValue("animation", "cockpitDoors");
			}
			action_automaticdoor->SetValue("timeMax", 2.0f);
			action->SetValue("AutomaticDoor", action_automaticdoor);
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
		damages->SetValue("groundCollisionMinMult", 2.0f);
		damages->SetValue("groundCollisionMaxMult", 5.0f);
		damages->SetValue("groundCollisionMinSpeed", 3.0f);
		damages->SetValue("groundCollisionMaxSpeed", 60.0f);
		damages->SetValue("vehicleCollisionDestructionSpeed", 4.0f);
		SmartScriptTable damages_damagemultipliers(gEnv->pScriptSystem);
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "bullet");
			if (m_modName == "MP")
			{
				damagemultiplier->SetValue("multiplier", 0.005f);
			}
			else
			{
				damagemultiplier->SetValue("multiplier", 0.02f);
			}
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
			damagemultiplier->SetValue("multiplier", 0.02f);
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
			damagemultiplier->SetValue("multiplier", 0.001f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.35f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "explosion");
			damagemultiplier->SetValue("multiplier", 0.5f);
			damagemultiplier->SetValue("splash", 0.25f);
			damages_damagemultipliers->PushBack(damagemultiplier);
		}
		{
			SmartScriptTable damagemultiplier(gEnv->pScriptSystem);
			damagemultiplier->SetValue("damageType", "law_rocket");
			damagemultiplier->SetValue("multiplier", 1.0f);
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
			damagemultiplier->SetValue("multiplier", 0.25f);
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
			damagemultiplier->SetValue("multiplier", 0.05f);
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
				damagessubgroup->SetValue("delay", 0.0f);
				damagessubgroup->SetValue("randomness", 1.0f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Explosion");
					SmartScriptTable damagebehavior_explosion(gEnv->pScriptSystem);
					damagebehavior_explosion->SetValue("damage", 2500.0f);
					damagebehavior_explosion->SetValue("helper", "engineSmokeOut");
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
					damagebehavior_movementnotification->SetValue("isDamageAlwaysFull", true);
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
