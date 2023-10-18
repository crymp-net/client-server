#include "Civ_car1.h"

Civ_car1::Civ_car1()
{
	// enable Civ_car1::Init function
	this->Vehicle::PatchVTable();
}

Civ_car1::~Civ_car1()
{
}

bool Civ_car1::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_physicsParams.flagsAND = ~pef_pushable_by_players;
	m_buoyancy.waterDensity = 50.0f;
	m_buoyancy.waterResistance = 0.0f;
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
			body_part->SetValue("component", "Hull");
			body_part->SetValue("mass", 0.0f);
			body_part->SetValue("disablePhysics", false);
			body_part->SetValue("isHidden", false);
			SmartScriptTable body_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable door01_part(gEnv->pScriptSystem);
				door01_part->SetValue("name", "door01");
				door01_part->SetValue("class", "AnimatedJoint");
				door01_part->SetValue("component", "Hull");
				door01_part->SetValue("mass", 50.0f);
				SmartScriptTable door01_part_animatedjoint(gEnv->pScriptSystem);
				door01_part_animatedjoint->SetValue("detachBaseForce", Vec3(-1, 0, 0));
				door01_part->SetValue("AnimatedJoint", door01_part_animatedjoint);
				body_part_parts->PushBack(door01_part);
			}
			{
				SmartScriptTable door02_part(gEnv->pScriptSystem);
				door02_part->SetValue("name", "door02");
				door02_part->SetValue("class", "AnimatedJoint");
				door02_part->SetValue("component", "Hull");
				door02_part->SetValue("mass", 50.0f);
				SmartScriptTable door02_part_animatedjoint(gEnv->pScriptSystem);
				door02_part_animatedjoint->SetValue("detachBaseForce", Vec3(1, 0, 0));
				door02_part->SetValue("AnimatedJoint", door02_part_animatedjoint);
				body_part_parts->PushBack(door02_part);
			}
			{
				SmartScriptTable back_door_part(gEnv->pScriptSystem);
				back_door_part->SetValue("name", "back_door");
				back_door_part->SetValue("class", "AnimatedJoint");
				back_door_part->SetValue("component", "Hull");
				back_door_part->SetValue("mass", 50.0f);
				SmartScriptTable back_door_part_animatedjoint(gEnv->pScriptSystem);
				back_door_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, -1, 0));
				back_door_part->SetValue("AnimatedJoint", back_door_part_animatedjoint);
				body_part_parts->PushBack(back_door_part);
			}
			{
				SmartScriptTable hood_part(gEnv->pScriptSystem);
				hood_part->SetValue("name", "hood");
				hood_part->SetValue("class", "AnimatedJoint");
				hood_part->SetValue("component", "Hull");
				hood_part->SetValue("mass", 50.0f);
				SmartScriptTable hood_part_animatedjoint(gEnv->pScriptSystem);
				hood_part_animatedjoint->SetValue("detachBaseForce", Vec3(0, 1, 1));
				hood_part->SetValue("AnimatedJoint", hood_part_animatedjoint);
				body_part_parts->PushBack(hood_part);
			}
			{
				SmartScriptTable steeringWheel_part(gEnv->pScriptSystem);
				steeringWheel_part->SetValue("name", "steeringWheel");
				steeringWheel_part->SetValue("class", "AnimatedJoint");
				steeringWheel_part->SetValue("mass", 0.0f);
				body_part_parts->PushBack(steeringWheel_part);
			}
			{
				SmartScriptTable dials_speedometer_part(gEnv->pScriptSystem);
				dials_speedometer_part->SetValue("name", "dials_speedometer");
				dials_speedometer_part->SetValue("class", "AnimatedJoint");
				dials_speedometer_part->SetValue("component", "Hull");
				dials_speedometer_part->SetValue("mass", 0.0f);
				SmartScriptTable dials_speedometer_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable dials_speedometer_part_animatedjoint_dials(gEnv->pScriptSystem);
				dials_speedometer_part_animatedjoint_dials->SetValue("rotationMax", 130.0f);
				dials_speedometer_part_animatedjoint->SetValue("Dials", dials_speedometer_part_animatedjoint_dials);
				dials_speedometer_part->SetValue("AnimatedJoint", dials_speedometer_part_animatedjoint);
				body_part_parts->PushBack(dials_speedometer_part);
			}
			{
				SmartScriptTable dials_revometer_part(gEnv->pScriptSystem);
				dials_revometer_part->SetValue("name", "dials_revometer");
				dials_revometer_part->SetValue("class", "AnimatedJoint");
				dials_revometer_part->SetValue("component", "Hull");
				dials_revometer_part->SetValue("mass", 0.0f);
				SmartScriptTable dials_revometer_part_animatedjoint(gEnv->pScriptSystem);
				SmartScriptTable dials_revometer_part_animatedjoint_dials(gEnv->pScriptSystem);
				dials_revometer_part_animatedjoint_dials->SetValue("rotationMax", 270.0f);
				dials_revometer_part_animatedjoint->SetValue("Dials", dials_revometer_part_animatedjoint_dials);
				dials_revometer_part->SetValue("AnimatedJoint", dials_revometer_part_animatedjoint);
				body_part_parts->PushBack(dials_revometer_part);
			}
			{
				SmartScriptTable wheel01_part(gEnv->pScriptSystem);
				wheel01_part->SetValue("name", "wheel01");
				wheel01_part->SetValue("class", "SubPartWheel");
				wheel01_part->SetValue("component", "wheel1");
				wheel01_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel01_part_subpartwheel(gEnv->pScriptSystem);
				wheel01_part_subpartwheel->SetValue("axle", 0);
				wheel01_part_subpartwheel->SetValue("density", 0.0f);
				wheel01_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel01_part_subpartwheel->SetValue("driving", true);
				wheel01_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel01_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel01_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel01_part_subpartwheel->SetValue("slipFrictionMod", 0.40000001f);
				wheel01_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel01_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel01_part_subpartwheel->SetValue("rimRadius", 0.25f);
				wheel01_part_subpartwheel->SetValue("torqueScale", 1.0f);
				wheel01_part->SetValue("SubPartWheel", wheel01_part_subpartwheel);
				body_part_parts->PushBack(wheel01_part);
			}
			{
				SmartScriptTable wheel02_part(gEnv->pScriptSystem);
				wheel02_part->SetValue("name", "wheel02");
				wheel02_part->SetValue("class", "SubPartWheel");
				wheel02_part->SetValue("component", "wheel2");
				wheel02_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel02_part_subpartwheel(gEnv->pScriptSystem);
				wheel02_part_subpartwheel->SetValue("axle", 1);
				wheel02_part_subpartwheel->SetValue("density", 0.0f);
				wheel02_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel02_part_subpartwheel->SetValue("driving", true);
				wheel02_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel02_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel02_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel02_part_subpartwheel->SetValue("slipFrictionMod", 0.40000001f);
				wheel02_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel02_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel02_part_subpartwheel->SetValue("rimRadius", 0.25f);
				wheel02_part_subpartwheel->SetValue("torqueScale", 1.0f);
				wheel02_part->SetValue("SubPartWheel", wheel02_part_subpartwheel);
				body_part_parts->PushBack(wheel02_part);
			}
			{
				SmartScriptTable wheel03_part(gEnv->pScriptSystem);
				wheel03_part->SetValue("name", "wheel03");
				wheel03_part->SetValue("class", "SubPartWheel");
				wheel03_part->SetValue("component", "wheel3");
				wheel03_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel03_part_subpartwheel(gEnv->pScriptSystem);
				wheel03_part_subpartwheel->SetValue("axle", 0);
				wheel03_part_subpartwheel->SetValue("density", 0.0f);
				wheel03_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel03_part_subpartwheel->SetValue("driving", true);
				wheel03_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel03_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel03_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel03_part_subpartwheel->SetValue("slipFrictionMod", 0.40000001f);
				wheel03_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel03_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel03_part_subpartwheel->SetValue("rimRadius", 0.25f);
				wheel03_part_subpartwheel->SetValue("torqueScale", 1.0f);
				wheel03_part->SetValue("SubPartWheel", wheel03_part_subpartwheel);
				body_part_parts->PushBack(wheel03_part);
			}
			{
				SmartScriptTable wheel04_part(gEnv->pScriptSystem);
				wheel04_part->SetValue("name", "wheel04");
				wheel04_part->SetValue("class", "SubPartWheel");
				wheel04_part->SetValue("component", "wheel4");
				wheel04_part->SetValue("mass", 80.0f);
				SmartScriptTable wheel04_part_subpartwheel(gEnv->pScriptSystem);
				wheel04_part_subpartwheel->SetValue("axle", 1);
				wheel04_part_subpartwheel->SetValue("density", 0.0f);
				wheel04_part_subpartwheel->SetValue("damping", -0.69999999f);
				wheel04_part_subpartwheel->SetValue("driving", true);
				wheel04_part_subpartwheel->SetValue("lenMax", 0.40000001f);
				wheel04_part_subpartwheel->SetValue("maxFriction", 1.0f);
				wheel04_part_subpartwheel->SetValue("minFriction", 1.0f);
				wheel04_part_subpartwheel->SetValue("slipFrictionMod", 0.40000001f);
				wheel04_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel04_part_subpartwheel->SetValue("suspLength", 0.25f);
				wheel04_part_subpartwheel->SetValue("rimRadius", 0.25f);
				wheel04_part_subpartwheel->SetValue("torqueScale", 1.0f);
				wheel04_part->SetValue("SubPartWheel", wheel04_part_subpartwheel);
				body_part_parts->PushBack(wheel04_part);
			}
			body_part->SetValue("Parts", body_part_parts);
			SmartScriptTable body_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable exhaust_pos_helper(gEnv->pScriptSystem);
				exhaust_pos_helper->SetValue("name", "exhaust_pos");
				exhaust_pos_helper->SetValue("position", Vec3(0.56879997, -2.7351, 0.62400001));
				exhaust_pos_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(exhaust_pos_helper);
			}
			{
				SmartScriptTable engineSmokeOut_helper(gEnv->pScriptSystem);
				engineSmokeOut_helper->SetValue("name", "engineSmokeOut");
				engineSmokeOut_helper->SetValue("position", Vec3(0.0339, 2.2307, 1.1449));
				engineSmokeOut_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(engineSmokeOut_helper);
			}
			{
				SmartScriptTable exhaust_posL_helper(gEnv->pScriptSystem);
				exhaust_posL_helper->SetValue("name", "exhaust_posL");
				exhaust_posL_helper->SetValue("position", Vec3(-0.56840003, -2.7560999, 0.62400001));
				exhaust_posL_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(exhaust_posL_helper);
			}
			{
				SmartScriptTable headLightLeft_helper(gEnv->pScriptSystem);
				headLightLeft_helper->SetValue("name", "headLightLeft");
				headLightLeft_helper->SetValue("position", Vec3(-0.69480002, 2.7725, 1.0477));
				headLightLeft_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightLeft_helper);
			}
			{
				SmartScriptTable headLightRight_helper(gEnv->pScriptSystem);
				headLightRight_helper->SetValue("name", "headLightRight");
				headLightRight_helper->SetValue("position", Vec3(0.71700001, 2.7809999, 1.0446));
				headLightRight_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightRight_helper);
			}
			{
				SmartScriptTable headLightCenter_helper(gEnv->pScriptSystem);
				headLightCenter_helper->SetValue("name", "headLightCenter");
				headLightCenter_helper->SetValue("position", Vec3(-0.0031999999, 2.9233, 1.0583));
				headLightCenter_helper->SetValue("direction", Vec3(0, 0.99479997, -0.1021));
				body_part_helpers->PushBack(headLightCenter_helper);
			}
			{
				SmartScriptTable rearLightLeft_helper(gEnv->pScriptSystem);
				rearLightLeft_helper->SetValue("name", "rearLightLeft");
				rearLightLeft_helper->SetValue("position", Vec3(-0.926, -2.6499, 1.331));
				rearLightLeft_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightLeft_helper);
			}
			{
				SmartScriptTable rearLightRight_helper(gEnv->pScriptSystem);
				rearLightRight_helper->SetValue("name", "rearLightRight");
				rearLightRight_helper->SetValue("position", Vec3(0.90549999, -2.6559999, 1.3150001));
				rearLightRight_helper->SetValue("direction", Vec3(0, -1, 0));
				body_part_helpers->PushBack(rearLightRight_helper);
			}
			{
				SmartScriptTable burningPos_helper(gEnv->pScriptSystem);
				burningPos_helper->SetValue("name", "burningPos");
				burningPos_helper->SetValue("position", Vec3(0, 0, 0.89160001));
				burningPos_helper->SetValue("direction", Vec3(0, 1, 0));
				body_part_helpers->PushBack(burningPos_helper);
			}
			body_part->SetValue("Helpers", body_part_helpers);
			SmartScriptTable body_part_animated(gEnv->pScriptSystem);
			body_part_animated->SetValue("filename", "Objects/Vehicles/Civ_car1/Civ_car.cga");
			body_part_animated->SetValue("filenameDestroyed", "Objects/Vehicles/Civ_car1/Civ_car_damaged.cga");
			body_part->SetValue("Animated", body_part_animated);
			parts->PushBack(body_part);
		}
		{
			SmartScriptTable massBox_part(gEnv->pScriptSystem);
			massBox_part->SetValue("name", "massBox");
			massBox_part->SetValue("class", "MassBox");
			massBox_part->SetValue("mass", 1500.0f);
			massBox_part->SetValue("position", Vec3(-0.001, 0.0017, 0.95480001));
			massBox_part->SetValue("disablePhysics", false);
			massBox_part->SetValue("disableCollision", false);
			massBox_part->SetValue("isHidden", false);
			SmartScriptTable massBox_part_helpers(gEnv->pScriptSystem);
			massBox_part->SetValue("Helpers", massBox_part_helpers);
			SmartScriptTable massBox_part_massbox(gEnv->pScriptSystem);
			massBox_part_massbox->SetValue("size", Vec3(1, 2, 0.67799997));
			massBox_part_massbox->SetValue("drivingOffset", -0.80000001f);
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
		component->SetValue("damageMax", 150.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(0, 0, 0));
		component->SetValue("maxBound", Vec3(0, 0, 0));
		component->SetValue("useBoundsFromParts", true);
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
			damagemultiplier->SetValue("multiplier", 0.25f);
			damagemultiplier->SetValue("splash", 0.5f);
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
		component->SetValue("damageMax", 100.0f);
		component->SetValue("major", true);
		component->SetValue("minBound", Vec3(-0.7, 1.4739, 0.58399999));
		component->SetValue("maxBound", Vec3(0.7, 2.75, 1.5));
		component->SetValue("useBoundsFromParts", false);
		component->SetValue("hullAffection", 0.5f);
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
			damagemultiplier->SetValue("multiplier", 0.25f);
			damagemultiplier->SetValue("splash", 0.5f);
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
			damagemultiplier->SetValue("damageType", "collision");
			damagemultiplier->SetValue("multiplier", 0.2f);
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
			SmartScriptTable damagebehavior_effect(gEnv->pScriptSystem);
			damagebehavior_effect->SetValue("effect", "EngineDamaged25");
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
			damagebehavior->SetValue("class", "Group");
			damagebehavior->SetValue("damageRatioMin", 1.0f);
			SmartScriptTable damagebehavior_Destroy_group(gEnv->pScriptSystem);
			damagebehavior_Destroy_group->SetValue("name", "Destroy");
			damagebehavior->SetValue("Group", damagebehavior_Destroy_group);
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
		component->SetValue("damageMax", 80.0f);
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
		component->SetValue("damageMax", 80.0f);
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
		component->SetValue("damageMax", 80.0f);
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
		component->SetValue("damageMax", 80.0f);
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
		component->SetValue("name", "headLightLeft");
		component->SetValue("damageMax", 45.0f);
		component->SetValue("major", false);
		component->SetValue("minBound", Vec3(-0.85, 2.55, 0.95));
		component->SetValue("maxBound", Vec3(-0.5, 2.85, 1.2));
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
		component->SetValue("minBound", Vec3(0.5, 2.55, 0.95));
		component->SetValue("maxBound", Vec3(0.85, 2.85, 1.2));
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
		component->SetValue("minBound", Vec3(-1.0, -2.7, 1.1));
		component->SetValue("maxBound", Vec3(-0.85, -2.45, 1.5));
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
		component->SetValue("minBound", Vec3(0.85, -2.7, 1.1));
		component->SetValue("maxBound", Vec3(1.0, -2.45, 1.5));
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
		particles_exhaust_helpers->PushBack("exhaust_pos");
		particles_exhaust_helpers->PushBack("exhaust_posL");
		particles_exhaust->SetValue("Helpers", particles_exhaust_helpers);
		SmartScriptTable particles_exhaust_enginestart(gEnv->pScriptSystem);
		particles_exhaust_enginestart->SetValue("effect", "vehicle_fx.vehicle_exhaust.CIV_Start");
		particles_exhaust->SetValue("EngineStart", particles_exhaust_enginestart);
		SmartScriptTable particles_exhaust_enginestop(gEnv->pScriptSystem);
		particles_exhaust_enginestop->SetValue("effect", "vehicle_fx.vehicle_exhaust.CIV_Start");
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
			SmartScriptTable HullDamaged25_damageeffect(gEnv->pScriptSystem);
			HullDamaged25_damageeffect->SetValue("name", "HullDamaged25");
			HullDamaged25_damageeffect->SetValue("helper", "burningPos");
			HullDamaged25_damageeffect->SetValue("effect", "vehicle_fx.Damages_Land.CivCar_hull_smoke_a");
			particles_damageeffects->PushBack(HullDamaged25_damageeffect);
		}
		{
			SmartScriptTable HullDamaged75_damageeffect(gEnv->pScriptSystem);
			HullDamaged75_damageeffect->SetValue("name", "HullDamaged75");
			HullDamaged75_damageeffect->SetValue("helper", "burningPos");
			HullDamaged75_damageeffect->SetValue("effect", "vehicle_fx.Damages_Land.CivCar_hull_smoke_b");
			particles_damageeffects->PushBack(HullDamaged75_damageeffect);
		}
		{
			SmartScriptTable VehicleDestroyed_damageeffect(gEnv->pScriptSystem);
			VehicleDestroyed_damageeffect->SetValue("name", "VehicleDestroyed");
			VehicleDestroyed_damageeffect->SetValue("helper", "burningPos");
			VehicleDestroyed_damageeffect->SetValue("effect", "explosions.CIV_explosion.a");
			VehicleDestroyed_damageeffect->SetValue("scaleMax", 0.30000001f);
			VehicleDestroyed_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			particles_damageeffects->PushBack(VehicleDestroyed_damageeffect);
		}
		{
			SmartScriptTable VehicleFlipped_damageeffect(gEnv->pScriptSystem);
			VehicleFlipped_damageeffect->SetValue("name", "VehicleFlipped");
			VehicleFlipped_damageeffect->SetValue("helper", "burningPos");
			VehicleFlipped_damageeffect->SetValue("effect", "smoke_and_fire.Jeep.flipped_jeep");
			particles_damageeffects->PushBack(VehicleFlipped_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged25_damageeffect(gEnv->pScriptSystem);
			EngineDamaged25_damageeffect->SetValue("name", "EngineDamaged25");
			EngineDamaged25_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged25_damageeffect->SetValue("effect", "vehicle_fx.Damages_Land.CivCar_engine_smoke_a");
			EngineDamaged25_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged25_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged25_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged25_damageeffect);
		}
		{
			SmartScriptTable EngineDamaged75_damageeffect(gEnv->pScriptSystem);
			EngineDamaged75_damageeffect->SetValue("name", "EngineDamaged75");
			EngineDamaged75_damageeffect->SetValue("helper", "engineSmokeOut");
			EngineDamaged75_damageeffect->SetValue("effect", "vehicle_fx.Damages_Land.CivCar_engine_smoke_b");
			EngineDamaged75_damageeffect->SetValue("scaleMax", 1.0f);
			EngineDamaged75_damageeffect->SetValue("gravityDirection", Vec3(0, 0, 1));
			EngineDamaged75_damageeffect->SetValue("pulsePeriod", 0.0f);
			particles_damageeffects->PushBack(EngineDamaged75_damageeffect);
		}
		particles->SetValue("DamageEffects", particles_damageeffects);
		SmartScriptTable particles_environmentlayers(gEnv->pScriptSystem);
		{
			SmartScriptTable environmentlayer(gEnv->pScriptSystem);
			environmentlayer->SetValue("minSpeed", 0.5f);
			environmentlayer->SetValue("minSpeedSizeScale", 1.0f);
			environmentlayer->SetValue("minSpeedCountScale", 1.0f);
			environmentlayer->SetValue("minSpeedSpeedScale", 1.0f);
			environmentlayer->SetValue("maxSpeed", 25.0f);
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
					wheels->PushBack(4);
					wheelgroup->PushBack(wheels);
				}
				environmentlayer_wheels_wheelgroups->PushBack(wheelgroup);
			}
			environmentlayer_wheels->SetValue("WheelGroups", environmentlayer_wheels_wheelgroups);
			environmentlayer->SetValue("Wheels", environmentlayer_wheels);
			particles_environmentlayers->PushBack(environmentlayer);
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
			_state->SetValue("animation", "car_steeringWheel");
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
		movement->SetValue("steerSpeed", 60.0f);
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
		movement_wheeled->SetValue("engineMaxRPM", 3800);
		movement_wheeled->SetValue("engineMinRPM", 100);
		movement_wheeled->SetValue("enginePower", 160);
		movement_wheeled->SetValue("engineShiftDownRPM", 1300);
		movement_wheeled->SetValue("engineShiftUpRPM", 2000);
		movement_wheeled->SetValue("engineStartRPM", 500);
		movement_wheeled->SetValue("stabilizer", 0.2f);
		movement_wheeled->SetValue("maxBrakingFriction", 0.60000002f);
		movement_wheeled->SetValue("rearWheelBrakingFrictionMult", 0.9f);
		movement_wheeled->SetValue("maxSteer", 0.44999999f);
		movement_wheeled->SetValue("maxSpeed", 23.0f);
		movement_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_wheeled->SetValue("minEnergy", 0.012f);
		movement_wheeled->SetValue("slipThreshold", 0.02f);
		movement_wheeled->SetValue("suspDampingMin", -0.18000001f);
		movement_wheeled->SetValue("suspDampingMax", -0.5f);
		movement_wheeled->SetValue("suspDampingMaxSpeed", 13.0f);
		movement_wheeled->SetValue("gearDirSwitchRPM", 1000);
		movement_wheeled->SetValue("dynFriction", 1.1f);
		SmartScriptTable movement_wheeled_gearratios(gEnv->pScriptSystem);
		movement_wheeled_gearratios->PushBack(-20.0f);
		movement_wheeled_gearratios->PushBack(0.0f);
		movement_wheeled_gearratios->PushBack(8.5f);
		movement_wheeled_gearratios->PushBack(6.0f);
		movement_wheeled_gearratios->PushBack(5.0f);
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
				SmartScriptTable maxSpeed_tweak(gEnv->pScriptSystem);
				maxSpeed_tweak->SetValue("name", "maxSpeed");
				maxSpeed_tweak->SetValue("value", 1.0f);
				maxSpeed_tweak->SetValue("op", 1);
				ai_tweakgroup_tweaks->PushBack(maxSpeed_tweak);
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
		seat->SetValue("enterHelper", "passenger01_enter_pos");
		seat->SetValue("sitHelper", "passenger01_seat_pos");
		seat->SetValue("isDriver", true);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 1);
		seat->SetValue("agVehicleName", "US_ltv");
		seat->SetValue("agSeatNumber", 1);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-20, 0, -100));
			view->SetValue("rotationMax", Vec3(5, 0, 100));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0.050000001, 0.050000001));
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
			if (m_modName == "PoliceCar")
			{
				seataction_sound->SetValue("sound", "sounds/vehicles:civ_car1:police_horn");
			}
			else
			{
				seataction_sound->SetValue("sound", "sounds/vehicles:civ_car1:horn");
			}
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
			seataction_lights_lightparts->PushBack("headLightRight");
			seataction_lights_lightparts->PushBack("headLightLeftFlare");
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
		seat_sounds->SetValue("mood", 0.5f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* driver = new VehicleSeat();
		driver->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "driver", driver });
	}
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("name", "passenger");
		seat->SetValue("part", "");
		seat->SetValue("enterHelper", "passenger02_enter_pos");
		seat->SetValue("sitHelper", "passenger02_seat_pos");
		seat->SetValue("isDriver", false);
		seat->SetValue("AimPart", "");
		seat->SetValue("seatGroupIndex", 2);
		seat->SetValue("agVehicleName", "US_ltv");
		seat->SetValue("agSeatNumber", 3);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", true);
			view->SetValue("rotationMin", Vec3(-12, 0, -110));
			view->SetValue("rotationMax", Vec3(5, 0, 110));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("offset", Vec3(0, 0, 0.1));
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
		seat_sounds->SetValue("mood", 0.5f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* passenger = new VehicleSeat();
		passenger->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "passenger", passenger });
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
		seatGroup_seats->PushBack("passenger");
		seatGroup->SetValue("Seats", seatGroup_seats);

		VehicleSeatGroup* cabin = new VehicleSeatGroup();
		cabin->Init(this, seatGroup);
		m_seatGroups.push_back(cabin);
	}

	// Paints
	if (m_paintName == "black")
	{
		this->SetPaintMaterial("objects/vehicles/civ_car1/civ_car_color_variations_black.mtl");
	}
	else if (m_paintName == "blue")
	{
		this->SetPaintMaterial("objects/vehicles/civ_car1/civ_car_color_variations_blue.mtl");
	}
	else if (m_paintName == "green")
	{
		this->SetPaintMaterial("objects/vehicles/civ_car1/civ_car_color_variations_green.mtl");
	}
	else if (m_paintName == "police")
	{
		this->SetPaintMaterial("objects/vehicles/civ_car1/civ_car_color_variations_police.mtl");
	}
	else if (m_paintName == "red")
	{
		this->SetPaintMaterial("objects/vehicles/civ_car1/civ_car_color_variations_red.mtl");
	}
	else if (m_paintName == "silver")
	{
		this->SetPaintMaterial("objects/vehicles/civ_car1/civ_car_color_variations_silver.mtl");
	}
	else if (m_paintName == "utility")
	{
		this->SetPaintMaterial("objects/vehicles/civ_car1/civ_car_color_variations_utility.mtl");
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
				activation->SetValue("param2", "door01");
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
				activation->SetValue("param2", "door02");
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
			damagemultiplier->SetValue("multiplier", 0.1f);
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
				damagessubgroup->SetValue("delay", 0.2f);
				damagessubgroup->SetValue("randomness", 0.5f);
				SmartScriptTable damagessubgroup_damagebehaviors(gEnv->pScriptSystem);
				{
					SmartScriptTable damagebehavior(gEnv->pScriptSystem);
					damagebehavior->SetValue("class", "Impulse");
					damagebehavior->SetValue("damageRatioMin", 1.0f);
					damagebehavior->SetValue("ignoreVehicleDestruction", true);
					SmartScriptTable damagebehavior_impulse(gEnv->pScriptSystem);
					damagebehavior_impulse->SetValue("forceMin", 4.5f);
					damagebehavior_impulse->SetValue("forceMax", 6.0f);
					damagebehavior_impulse->SetValue("direction", Vec3(0, 1, 1));
					damagebehavior_impulse->SetValue("momentum", Vec3(2.95, -0.15000001, 0));
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
