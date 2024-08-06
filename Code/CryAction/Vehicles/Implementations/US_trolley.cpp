#include "US_trolley.h"

US_trolley::US_trolley()
{
	// enable US_trolley::Init function
	this->Vehicle::PatchVTable();
}

US_trolley::~US_trolley()
{
}

bool US_trolley::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	m_actionMapName = "landvehicle";

	// Physics
	m_buoyancy.waterDensity = 0.0f;
	m_buoyancy.waterResistance = 0.0f;
	m_physics.maxLoggedCollisions = 2;
	m_physics.maxTimeStep = 0.02f;
	m_physics.minEnergy = 0.002f;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable body_part(gEnv->pScriptSystem);
			body_part->SetValue("class", "Animated");
			body_part->SetValue("disableCollision", false);
			body_part->SetValue("disablePhysics", false);
			body_part->SetValue("mass", 0.0f);
			body_part->SetValue("name", "body");
			SmartScriptTable body_part_parts(gEnv->pScriptSystem);
			{
				SmartScriptTable wheel01_part(gEnv->pScriptSystem);
				wheel01_part->SetValue("class", "SubPartWheel");
				wheel01_part->SetValue("component", "wheel1");
				wheel01_part->SetValue("name", "wheel01");
				SmartScriptTable wheel01_part_subpartwheel(gEnv->pScriptSystem);
				wheel01_part_subpartwheel->SetValue("axle", 0);
				wheel01_part_subpartwheel->SetValue("damping", -0.5f);
				wheel01_part_subpartwheel->SetValue("density", 5000.0f);
				wheel01_part_subpartwheel->SetValue("driving", true);
				wheel01_part_subpartwheel->SetValue("lenMax", 0.125f);
				wheel01_part_subpartwheel->SetValue("maxFriction", 0.92f);
				wheel01_part_subpartwheel->SetValue("minFriction", 0.92f);
				wheel01_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel01_part_subpartwheel->SetValue("suspLength", 0.04f);
				wheel01_part->SetValue("SubPartWheel", wheel01_part_subpartwheel);
				body_part_parts->PushBack(wheel01_part);
			}
			{
				SmartScriptTable wheel03_part(gEnv->pScriptSystem);
				wheel03_part->SetValue("class", "SubPartWheel");
				wheel03_part->SetValue("component", "wheel2");
				wheel03_part->SetValue("name", "wheel03");
				SmartScriptTable wheel03_part_subpartwheel(gEnv->pScriptSystem);
				wheel03_part_subpartwheel->SetValue("axle", 1);
				wheel03_part_subpartwheel->SetValue("damping", -0.5f);
				wheel03_part_subpartwheel->SetValue("density", 5000.0f);
				wheel03_part_subpartwheel->SetValue("driving", true);
				wheel03_part_subpartwheel->SetValue("lenMax", 0.125f);
				wheel03_part_subpartwheel->SetValue("maxFriction", 0.95f);
				wheel03_part_subpartwheel->SetValue("minFriction", 0.95f);
				wheel03_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel03_part_subpartwheel->SetValue("suspLength", 0.04f);
				wheel03_part->SetValue("SubPartWheel", wheel03_part_subpartwheel);
				body_part_parts->PushBack(wheel03_part);
			}
			{
				SmartScriptTable wheel02_part(gEnv->pScriptSystem);
				wheel02_part->SetValue("class", "SubPartWheel");
				wheel02_part->SetValue("component", "wheel3");
				wheel02_part->SetValue("name", "wheel02");
				SmartScriptTable wheel02_part_subpartwheel(gEnv->pScriptSystem);
				wheel02_part_subpartwheel->SetValue("axle", 0);
				wheel02_part_subpartwheel->SetValue("damping", -0.5f);
				wheel02_part_subpartwheel->SetValue("density", 5000.0f);
				wheel02_part_subpartwheel->SetValue("driving", true);
				wheel02_part_subpartwheel->SetValue("lenMax", 0.125f);
				wheel02_part_subpartwheel->SetValue("maxFriction", 0.92f);
				wheel02_part_subpartwheel->SetValue("minFriction", 0.92f);
				wheel02_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel02_part_subpartwheel->SetValue("suspLength", 0.04f);
				wheel02_part->SetValue("SubPartWheel", wheel02_part_subpartwheel);
				body_part_parts->PushBack(wheel02_part);
			}
			{
				SmartScriptTable wheel04_part(gEnv->pScriptSystem);
				wheel04_part->SetValue("class", "SubPartWheel");
				wheel04_part->SetValue("component", "wheel4");
				wheel04_part->SetValue("name", "wheel04");
				SmartScriptTable wheel04_part_subpartwheel(gEnv->pScriptSystem);
				wheel04_part_subpartwheel->SetValue("axle", 1);
				wheel04_part_subpartwheel->SetValue("damping", -0.5f);
				wheel04_part_subpartwheel->SetValue("density", 5000.0f);
				wheel04_part_subpartwheel->SetValue("driving", true);
				wheel04_part_subpartwheel->SetValue("lenMax", 0.125f);
				wheel04_part_subpartwheel->SetValue("maxFriction", 0.95f);
				wheel04_part_subpartwheel->SetValue("minFriction", 0.95f);
				wheel04_part_subpartwheel->SetValue("stiffness", 0.0f);
				wheel04_part_subpartwheel->SetValue("suspLength", 0.04f);
				wheel04_part->SetValue("SubPartWheel", wheel04_part_subpartwheel);
				body_part_parts->PushBack(wheel04_part);
			}
			body_part->SetValue("Parts", body_part_parts);
			SmartScriptTable body_part_helpers(gEnv->pScriptSystem);
			{
				SmartScriptTable viewPos_helper(gEnv->pScriptSystem);
				viewPos_helper->SetValue("direction", Vec3(0, 1, 0));
				viewPos_helper->SetValue("name", "viewPos");
				viewPos_helper->SetValue("position", Vec3(0, -2.596, 1.7));
				body_part_helpers->PushBack(viewPos_helper);
			}
			{
				SmartScriptTable actorPos_helper(gEnv->pScriptSystem);
				actorPos_helper->SetValue("direction", Vec3(0, 1, 0));
				actorPos_helper->SetValue("name", "actorPos");
				actorPos_helper->SetValue("position", Vec3(0, -3.1, 0.3));
				body_part_helpers->PushBack(actorPos_helper);
			}
			{
				SmartScriptTable leftArmPos_helper(gEnv->pScriptSystem);
				leftArmPos_helper->SetValue("direction", Vec3(0, 1, 0));
				leftArmPos_helper->SetValue("name", "leftArmPos");
				leftArmPos_helper->SetValue("position", Vec3(-0.15, -2.46, 1.165));
				body_part_helpers->PushBack(leftArmPos_helper);
			}
			{
				SmartScriptTable rightArmPos_helper(gEnv->pScriptSystem);
				rightArmPos_helper->SetValue("direction", Vec3(0, 1, 0));
				rightArmPos_helper->SetValue("name", "rightArmPos");
				rightArmPos_helper->SetValue("position", Vec3(0.15, -2.46, 1.14));
				body_part_helpers->PushBack(rightArmPos_helper);
			}
			body_part->SetValue("Helpers", body_part_helpers);
			SmartScriptTable body_part_animated(gEnv->pScriptSystem);
			body_part_animated->SetValue("filename", "Objects/Library/Architecture/Aircraftcarrier/props/trolley/bigtrolley_useable.cga");
			body_part->SetValue("Animated", body_part_animated);
			parts->PushBack(body_part);
		}
		{
			SmartScriptTable mass_part(gEnv->pScriptSystem);
			mass_part->SetValue("class", "MassBox");
			mass_part->SetValue("component", "");
			mass_part->SetValue("disableCollision", false);
			mass_part->SetValue("disablePhysics", false);
			mass_part->SetValue("isHidden", false);
			mass_part->SetValue("mass", 3600.0f);
			mass_part->SetValue("name", "mass");
			mass_part->SetValue("position", Vec3(0, 0, 0.4));
			SmartScriptTable mass_part_helpers(gEnv->pScriptSystem);
			mass_part->SetValue("Helpers", mass_part_helpers);
			SmartScriptTable mass_part_massbox(gEnv->pScriptSystem);
			mass_part_massbox->SetValue("size", Vec3(0.2, 0.5, 0.2));
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
		component->SetValue("damageMax", 0.0f);
		component->SetValue("major", false);
		component->SetValue("maxBound", Vec3(-0.24, -2.47, 1.082));
		component->SetValue("minBound", Vec3(0.243, -2.304, 1.198));
		component->SetValue("name", "Handle");
		component->SetValue("useBoundsFromParts", false);
		SmartScriptTable component_damagebehaviors(gEnv->pScriptSystem);
		component->SetValue("DamageBehaviors", component_damagebehaviors);

		VehicleComponent* handle = new VehicleComponent();
		handle->Init(this, component);
		m_components.push_back(handle);
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
		particles_exhaust_enginerunning->SetValue("baseSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("effect", "");
		particles_exhaust_enginerunning->SetValue("boostEffect", "");
		particles_exhaust_enginerunning->SetValue("maxPower", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxPowerSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeed", 20.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPower", 0.0f);
		particles_exhaust_enginerunning->SetValue("minPowerCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPowerSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 1.0f);
		particles_exhaust->SetValue("EngineRunning", particles_exhaust_enginerunning);
		particles->SetValue("Exhaust", particles_exhaust);
		SmartScriptTable particles_damageeffects(gEnv->pScriptSystem);
		particles->SetValue("DamageEffects", particles_damageeffects);

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Particles", particles);
		this->InitParticles(table);
	}

	// MovementParams
	{
		SmartScriptTable movement(gEnv->pScriptSystem);
		movement->SetValue("engineIgnitionTime", 0.0f);
		// Unknown attribute engineSoundPosition="engineSmokeOut"
		movement->SetValue("isBreakingOnIdle", true);
		movement->SetValue("kvSteerMax", 10.0f);
		movement->SetValue("pedalLimitMax", 0.15f);
		movement->SetValue("steerRelaxation", 90.0f);
		movement->SetValue("steerSpeed", 40.0f);
		movement->SetValue("steerSpeedMin", 90.0f);
		movement->SetValue("steerSpeedScale", 0.8f);
		movement->SetValue("steerSpeedScaleMin", 1.0f);
		movement->SetValue("v0SteerMax", 30.0f);
		movement->SetValue("vMaxSteerMax", 20.0f);
		SmartScriptTable movement_wheeled(gEnv->pScriptSystem);
		movement_wheeled->SetValue("axleFriction", 1500);
		movement_wheeled->SetValue("brakeTorque", 1400);
		movement_wheeled->SetValue("clutchSpeed", 0.8f);
		movement_wheeled->SetValue("damping", 0.0f);
		movement_wheeled->SetValue("dynFriction", 1.41f);
		movement_wheeled->SetValue("engineIdleRPM", 0);
		movement_wheeled->SetValue("engineMaxRPM", 1400);
		movement_wheeled->SetValue("engineMinRPM", 0);
		movement_wheeled->SetValue("enginePower", 55);
		movement_wheeled->SetValue("engineShiftDownRPM", 400);
		movement_wheeled->SetValue("engineShiftUpRPM", 1050);
		movement_wheeled->SetValue("engineStartRPM", 100);
		movement_wheeled->SetValue("gearDirSwitchRPM", 1500);
		movement_wheeled->SetValue("maxSteer", 0.52f);
		movement_wheeled->SetValue("maxTimeStep", 0.02f);
		movement_wheeled->SetValue("minEnergy", 0.012f);
		movement_wheeled->SetValue("slipThreshold", 0.1f);
		movement_wheeled->SetValue("stabilizer", 1.0f);
		SmartScriptTable movement_wheeled_gearratios(gEnv->pScriptSystem);
		movement_wheeled_gearratios->PushBack(-10.0f);
		movement_wheeled_gearratios->PushBack(0.0f);
		movement_wheeled_gearratios->PushBack(9.0f);
		movement_wheeled_gearratios->PushBack(7.0f);
		movement_wheeled_gearratios->PushBack(5.5f);
		movement_wheeled->SetValue("gearRatios", movement_wheeled_gearratios);
		movement->SetValue("Wheeled", movement_wheeled);
		SmartScriptTable movement_soundparams(gEnv->pScriptSystem);
		movement_soundparams->SetValue("engineSoundPosition", "");
		movement_soundparams->SetValue("runSoundDelay", 0.1f);
		movement->SetValue("SoundParams", movement_soundparams);

		if (!this->SetMovement("StdWheeled", movement))
		{
			return false;
		}
	}

	SmartScriptTable seats(gEnv->pScriptSystem);

	// Seats
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("AimPart", "");
		seat->SetValue("enterHelper", "actorPos");
		seat->SetValue("isDriver", true);
		seat->SetValue("movePassengerOnExit", false);
		seat->SetValue("name", "user");
		seat->SetValue("part", "");
		seat->SetValue("sitHelper", "actorPos");
		seat->SetValue("useBoundsForEntering", true);
		seat->SetValue("disableStopAllAnimationsOnEnter", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("canRotate", true);
			view->SetValue("class", "FirstPerson");
			view->SetValue("rotationMax", Vec3(22, 0, 20));
			view->SetValue("rotationInit", Vec3(-20, 0, 0));
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "viewPos");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		SmartScriptTable seat_seatactions(gEnv->pScriptSystem);
		{
			SmartScriptTable seataction(gEnv->pScriptSystem);
			seataction->SetValue("class", "PassengerIK");
			SmartScriptTable seataction_passengerik(gEnv->pScriptSystem);
			seataction_passengerik->SetValue("waitShortlyBeforeStarting", 1.0f);
			SmartScriptTable seataction_passengerik_limbs(gEnv->pScriptSystem);
			{
				SmartScriptTable limb(gEnv->pScriptSystem);
				limb->SetValue("helper", "leftArmPos");
				limb->SetValue("limb", "leftArm");
				seataction_passengerik_limbs->PushBack(limb);
			}
			{
				SmartScriptTable limb(gEnv->pScriptSystem);
				limb->SetValue("helper", "rightArmPos");
				limb->SetValue("limb", "rightArm");
				seataction_passengerik_limbs->PushBack(limb);
			}
			seataction_passengerik->SetValue("Limbs", seataction_passengerik_limbs);
			seataction->SetValue("PassengerIK", seataction_passengerik);
			seat_seatactions->PushBack(seataction);
		}
		seat->SetValue("SeatActions", seat_seatactions);
		SmartScriptTable seat_sounds(gEnv->pScriptSystem);
		seat_sounds->SetValue("inout", 1.0f);
		seat->SetValue("Sounds", seat_sounds);
		seats->PushBack(seat);

		VehicleSeat* user = new VehicleSeat();
		user->Init(this, static_cast<TVehicleSeatId>(m_seats.size() + 1), seat);
		m_seats.push_back({ "user", user });
	}

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetValue("Seats", seats);
	}

	// SeatTransitions
	this->InitSeatTransitions();

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
				activation->SetValue("param2", "Handle");
				action_activations->PushBack(activation);
			}
			action->SetValue("Activations", action_activations);
			SmartScriptTable action_enter(gEnv->pScriptSystem);
			SmartScriptTable action_enter_seats(gEnv->pScriptSystem);
			action_enter_seats->PushBack("user");
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
