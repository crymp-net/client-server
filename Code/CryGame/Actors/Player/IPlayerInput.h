// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IPLAYERINPUT_H__
#define __IPLAYERINPUT_H__

#pragma once

#include "CryCommon/CryAISystem/IAgent.h" // for EStance
#include "CryCommon/CryNetwork/ISerialize.h"
#include "CryCommon/CryAction/IGameObject.h"

struct SSerializedPlayerInput
{
	uint8 stance = STANCE_STAND;
	uint8 bodystate = 0;
	Vec3 deltaMovement = Vec3(ZERO);
	Vec3 lookDirection = Vec3(FORWARD_DIRECTION);
	Vec3 bodyDirection = Vec3(FORWARD_DIRECTION);
	Vec3 position = Vec3(ZERO);
	bool sprint = false;
	bool aiming = false;
	bool usinglookik = false;
	bool allowStrafing = false;
	bool isDirty = false;
	bool inAir = false;
	float pseudoSpeed = false;
	uint8 physCounter = 0;
	bool leanl = false;
	bool leanr = false;
	EntityId standingOn = 0;

	void Serialize_Default(TSerialize ser)
	{
		ser.Value("stance",			stance,			'stnc');
		ser.Value("deltaMovement",  deltaMovement,  'pMov');
		ser.Value("lookDirection",  lookDirection,  'dir0');
		ser.Value("sprint",			sprint,			'bool');
		ser.Value("leanl",			leanl,			'bool');
		ser.Value("leanr",			leanr,			'bool');
	}

	void Serialize_CryMP(TSerialize ser)
	{
		ser.Value("stance",			stance,			'stnc');
		ser.Value("deltaMovement",  deltaMovement,  'pMov');
		ser.Value("lookDirection",  lookDirection,  'dir3');
		ser.Value("sprint",			sprint,			'bool');
		ser.Value("leanl",			leanl,			'bool');
		ser.Value("leanr",			leanr,			'bool');
	}


	/*
	==================================================================================================================
		SSerializedPlayerInput
	==================================================================================================================
	*/

	#define PLAYER_INPUT_STANDING_ON_OFFSET Vec3(20.f)

	static void SerialiseRelativePosition(TSerialize ser, Vec3& position, EntityId standingOn)
	{
		// Convert position to local space and offset it since policy
		// doesn't allow negative numbers
		IEntity* pGroundEntity = gEnv->pEntitySystem->GetEntity(standingOn);
		if (ser.IsReading())
		{
			ser.Value("position", position, 'wrld');
			if (pGroundEntity)
				position -= PLAYER_INPUT_STANDING_ON_OFFSET;
		}
		else
		{
			Vec3 serPosition = position;
			if (pGroundEntity != NULL)
				serPosition = (pGroundEntity->GetWorldTM().GetInverted() * position) + PLAYER_INPUT_STANDING_ON_OFFSET;
			ser.Value("position", serPosition, 'wrld');
		}
	}

	void Serialize_CryMP_2(TSerialize ser)
	{

		ser.Value("stance", stance, 'stnc');
		ser.Value("deltaMovement", deltaMovement, 'pMov');
		ser.Value("lookDirection", lookDirection, 'dir3');
		ser.Value("sprint", sprint, 'bool');
		ser.Value("inAir", inAir, 'bool');
		ser.Value("physcounter", physCounter, 'ui2');
		bool bIsEntity = standingOn != 0;
		ser.Value("bIsEntity", bIsEntity, 'bool');
		ser.Value("standingOn", standingOn, 'eid');

		// Did we get a valid entity? This is a failsafe for late joiners to get CryNetwork to send the data again!
		if (ser.IsReading() && standingOn == 0 && bIsEntity)
			ser.FlagPartialRead();
	

		// Always serialise the position
		SerialiseRelativePosition(ser, position, standingOn);
	}
};

struct IPlayerInput
{
	static const EEntityAspects INPUT_ASPECT = eEA_GameClientDynamic;

	enum EInputType
	{
		PLAYER_INPUT,
		NETPLAYER_INPUT,
		NETPLAYERENHANCED_INPUT,
		DEDICATED_INPUT,
	};

	virtual ~IPlayerInput() {};

	virtual void PreUpdate() = 0;
	virtual void Update() = 0;
	virtual void PostUpdate() = 0;

	virtual void OnAction( const ActionId& action, int activationMode, float value ) = 0;

	virtual void SetState( const SSerializedPlayerInput& input ) = 0;
	virtual void GetState( SSerializedPlayerInput& input ) = 0;

	virtual void Reset() = 0;
	virtual void DisableXI(bool disabled) = 0;

	virtual EInputType GetType() const = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) = 0;

	virtual uint32 GetMoveButtonsState() const = 0;
	virtual uint32 GetActions() const = 0;
};

#endif
