#include "DefaultVehicle.h"

DefaultVehicle::DefaultVehicle()
{
	// enable DefaultVehicle::Init function
	this->Vehicle::PatchVTable();
}

DefaultVehicle::~DefaultVehicle()
{
}

bool DefaultVehicle::Init(IGameObject* pGameObject)
{
	if (!this->Vehicle::Init(pGameObject))
	{
		return false;
	}

	// Physics
	// Unknown attribute collDamageThreshold="0"
	m_buoyancy.waterDensity = 0.0f;
	m_buoyancy.waterResistance = 0.0f;

	Vehicle::ComponentParts componentParts;

	// Parts
	{
		SmartScriptTable parts(gEnv->pScriptSystem);
		{
			SmartScriptTable body_part(gEnv->pScriptSystem);
			body_part->SetValue("class", "Static");
			body_part->SetValue("disableCollision", false);
			body_part->SetValue("mass", 100.0f);
			body_part->SetValue("name", "body");
			SmartScriptTable body_part_static(gEnv->pScriptSystem);
			body_part_static->SetValue("filename", "Objects/Default.cgf");
			body_part_static->SetValue("geometry", "");
			body_part->SetValue("Static", body_part_static);
			SmartScriptTable body_part_helpers(gEnv->pScriptSystem);
			body_part->SetValue("Helpers", body_part_helpers);
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
		particles_exhaust_enginerunning->SetValue("minPower", 0.0f);
		particles_exhaust_enginerunning->SetValue("minPowerCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minPowerSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeed", 0.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("minSpeedSizeScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedCountScale", 1.0f);
		particles_exhaust_enginerunning->SetValue("maxSpeedSizeScale", 1.0f);
		particles_exhaust->SetValue("EngineRunning", particles_exhaust_enginerunning);
		particles->SetValue("Exhaust", particles_exhaust);
		SmartScriptTable particles_damageeffects(gEnv->pScriptSystem);
		particles->SetValue("DamageEffects", particles_damageeffects);

		SmartScriptTable table(gEnv->pScriptSystem);
		table->SetValue("Particles", particles);
		this->InitParticles(table);
	}

	// MovementParams

	SmartScriptTable seats(gEnv->pScriptSystem);

	// Seats
	{
		SmartScriptTable seat(gEnv->pScriptSystem);
		seat->SetValue("AimPart", "");
		seat->SetValue("enterHelper", "");
		seat->SetValue("isDriver", true);
		seat->SetValue("name", "driver");
		seat->SetValue("part", "");
		seat->SetValue("sitHelper", "");
		seat->SetValue("useBoundsForEntering", true);
		SmartScriptTable seat_views(gEnv->pScriptSystem);
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "FirstPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_firstperson(gEnv->pScriptSystem);
			view_firstperson->SetValue("helper", "");
			view->SetValue("FirstPerson", view_firstperson);
			seat_views->PushBack(view);
		}
		{
			SmartScriptTable view(gEnv->pScriptSystem);
			view->SetValue("class", "ThirdPerson");
			view->SetValue("canRotate", false);
			SmartScriptTable view_thirdperson(gEnv->pScriptSystem);
			view_thirdperson->SetValue("distance", 5.0f);
			view_thirdperson->SetValue("heightOffset", 1.0f);
			view_thirdperson->SetValue("speed", 5.0f);
			view->SetValue("ThirdPerson", view_thirdperson);
			seat_views->PushBack(view);
		}
		seat->SetValue("Views", seat_views);
		// Unknown element "Weapons"
		// Unknown element "ActionParts"
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

	this->InitMaxDamage();
	this->AttachScriptBindToSeats();

	return this->BindVehicleToNetwork();
}
