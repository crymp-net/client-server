/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements an entity class which can serialize vehicle parts

-------------------------------------------------------------------------
History:
- 16:09:2005: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "VehicleSeatSerializer.h"
#include "Vehicle.h"
#include "VehicleSeat.h"


//------------------------------------------------------------------------
CVehicleSeatSerializer::CVehicleSeatSerializer()
: m_pVehicle(0),
	m_pSeat(0)
{
}

//------------------------------------------------------------------------
CVehicleSeatSerializer::~CVehicleSeatSerializer()
{
}

//------------------------------------------------------------------------
bool CVehicleSeatSerializer::Init(IGameObject * pGameObject)
{
	SetGameObject(pGameObject);
	
	if (m_pSeat) // needs to be done here since anywhere earlier, EntityId is not known
		m_pSeat->SetSerializer(this);
	else if (!gEnv->bServer)
		return false;

	if (0 == (GetEntity()->GetFlags() & (ENTITY_FLAG_CLIENT_ONLY | ENTITY_FLAG_SERVER_ONLY)))
		if (!GetGameObject()->BindToNetwork())
			return false;

	GetEntity()->Activate(0);
	GetEntity()->Hide(true);

	if (gEnv->bServer && !IsDemoPlayback())
	{
		CVehicleSeat *pSeat=static_cast<CVehicleSystem *>(gEnv->pGame->GetIGameFramework()->GetIVehicleSystem())->GetInitializingSeat();
		assert(pSeat);

		pSeat->SetSerializer(this);
		SetSeat(pSeat);
		SetVehicle(static_cast<CVehicle *>(pSeat->GetVehicle()));
	}

	return true;
}

//------------------------------------------------------------------------
void CVehicleSeatSerializer::InitClient(int channelId)
{
}

//------------------------------------------------------------------------
void CVehicleSeatSerializer::FullSerialize( TSerialize ser )
{
}

//------------------------------------------------------------------------
bool CVehicleSeatSerializer::NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags )
{
	if (m_pSeat)
		m_pSeat->SerializeActions(ser, aspect);
	return true;
}

//------------------------------------------------------------------------
void CVehicleSeatSerializer::Update(SEntityUpdateContext& ctx, int)
{
}

//------------------------------------------------------------------------
void CVehicleSeatSerializer::HandleEvent(const SGameObjectEvent& event)
{
}

//------------------------------------------------------------------------
void CVehicleSeatSerializer::SerializeSpawnInfo( TSerialize ser )
{
	EntityId vehicle;
	TVehicleSeatId seatId;

	ser.Value("vehicle", vehicle, 'eid');
	ser.Value("seat", seatId, 'seat');

	assert(ser.IsReading());

	// warning GameObject not set at this point
	// GetGameObject calls will fail miserably

	m_pVehicle = static_cast<CVehicle *>(gEnv->pGame->GetIGameFramework()->GetIVehicleSystem()->GetVehicle(vehicle));
	if (!m_pVehicle)
		return;

	CVehicleSeat *pSeat=static_cast<CVehicleSeat *>(m_pVehicle->GetSeatById(seatId));
	m_pSeat=pSeat;

	// the serializer is set on the seat in ::Init
}

//------------------------------------------------------------------------
ISerializableInfoPtr CVehicleSeatSerializer::GetSpawnInfo()
{
	struct SInfo : public ISerializableInfo
	{
		EntityId vehicle;
		TVehicleSeatId seatId;
		void SerializeWith( TSerialize ser )
		{
			ser.Value("vehicle", vehicle, 'eid');
			ser.Value("seat", seatId, 'seat');
		}
	};

	SInfo * p = new SInfo;
	p->vehicle = m_pVehicle->GetEntityId();
	p->seatId = m_pSeat->GetSeatId();
	return p;
}

//------------------------------------------------------------------------
void CVehicleSeatSerializer::SetVehicle(CVehicle *pVehicle)
{
	m_pVehicle = pVehicle;

	if (pVehicle)
		GetGameObject()->SetNetworkParent( pVehicle->GetEntityId() );
	else
		GetGameObject()->SetNetworkParent( 0 );
}

//------------------------------------------------------------------------
void CVehicleSeatSerializer::SetSeat(CVehicleSeat *pSeat)
{
	m_pSeat = pSeat;
}
