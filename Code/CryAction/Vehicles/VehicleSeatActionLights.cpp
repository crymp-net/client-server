/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action to head/spot lights

-------------------------------------------------------------------------
History:
- 01:03:2006: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"
#include "CryAction.h"
#include "IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleSeat.h"
#include "VehiclePartLight.h"
#include "VehicleSeatActionLights.h"


static const char* activationSounds[] = 
{
  "sounds/vehicles:vehicle_accessories:light",
  "sounds/vehicles:vehicle_accessories:flashlight",
  "sounds/vehicles:vehicle_accessories:spotlight",
};

//------------------------------------------------------------------------
bool CVehicleSeatActionLights::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table)
{
  m_pVehicle = pVehicle;
	m_seatId = seatId;
  m_pSound = 0;

	CVehicleParams lightsTable = table.findChild("Lights");
	if (!lightsTable)
		return false;

	string activation = lightsTable.getAttr("activation");

	if (activation == "brake")
		m_activation = eLA_Brake;
	else if(activation == "reverse")
		m_activation = eLA_Reversing;
	else
		m_activation = eLA_Toggle;

	if (m_activation == eLA_Toggle && gEnv->pSoundSystem)
	{ 
		int sound = 1;

		if (!lightsTable.getAttr("sound", sound) || !sound)
		{ // try to get it as a string
			string xString;
			if (xString = lightsTable.getAttr("sound") && !xString.empty())
				m_pSound = gEnv->pSoundSystem->CreateSound(xString, FLAG_SOUND_DEFAULT_3D);
		}
		else if (sound > 0 && sound <= sizeof(activationSounds)/sizeof(activationSounds[0]))
			m_pSound = gEnv->pSoundSystem->CreateSound(activationSounds[sound-1], FLAG_SOUND_DEFAULT_3D);
	}

	CVehicleParams lightsPartsArray = lightsTable.findChild("LightParts");
	if (lightsPartsArray)
	{
		int i = 0;
		int c = lightsPartsArray.getChildCount();
		m_lightParts.reserve(c);

		for (; i < c; i++)
		{
			if (CVehicleParams lightPartRef = lightsPartsArray.getChild(i))
			{	  
				if (IVehiclePart* pPart = pVehicle->GetPart(lightPartRef.getAttr("value")))
				{
					if (CVehiclePartLight* pPartLight = CAST_VEHICLEOBJECT(CVehiclePartLight, pPart))
					{ 
						SLightPart light;
						light.pPart = pPartLight;
						m_lightParts.push_back(light);
					}
				}
			}
		}
	}

	m_enabled = false;
	return !m_lightParts.empty();
}

//------------------------------------------------------------------------
void CVehicleSeatActionLights::Reset()
{
  m_enabled = false;
}


//------------------------------------------------------------------------
void CVehicleSeatActionLights::Serialize(TSerialize ser, EEntityAspects aspects)
{
  if (ser.GetSerializationTarget() != eST_Network)
  {
    ser.Value("enabled", m_enabled);
  }
	else
  {
    if (aspects&CVehicle::ASPECT_SEAT_ACTION)
	  {
		  bool enabled=m_enabled;
		  ser.Value("enabled", enabled, 'bool');

      if (ser.IsReading() && enabled!=m_enabled)
      {
        ToggleLights(enabled);  
      }
	  }
  }
}

//------------------------------------------------------------------------
void CVehicleSeatActionLights::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{ 
  if (actionId == eVAI_ToggleLights && m_activation == eLA_Toggle)
  {
    if (eAAM_OnPress == activationMode)
    { 
      ToggleLights(value==0.f ? false : !m_enabled);

      CVehicleSeat *pSeat=static_cast<CVehicleSeat *>(m_pVehicle->GetSeatById(m_seatId));
      if (pSeat)
        pSeat->ChangedNetworkState(CVehicle::ASPECT_SEAT_ACTION);	
    }
  }
}

//---------------------------------------------------------------------------
void CVehicleSeatActionLights::ToggleLights(bool enable)
{
  for (TVehiclePartLightVector::iterator ite=m_lightParts.begin(), end=m_lightParts.end(); ite!=end; ++ite)	  
    ite->pPart->ToggleLight(enable);    	
  
  if (m_pSound)
  {
    IEntitySoundProxy* pSoundProxy = (IEntitySoundProxy*)m_pVehicle->GetEntity()->CreateProxy(ENTITY_PROXY_SOUND);
    if (pSoundProxy)
    {
      pSoundProxy->PlaySound(m_pSound);
    }
  }

  m_enabled = enable;
}

//---------------------------------------------------------------------------
void CVehicleSeatActionLights::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
  switch (event)
  {
  case eVE_Brake:
	case eVE_Reversing:
    {
      if ((event == eVE_Brake && eLA_Brake == m_activation) 
				|| (event == eVE_Reversing && eLA_Reversing == m_activation))
      {
        bool toggle = true;

        if (params.bParam)
        {
          // only enable brake lights if engine is powered, and driver is still inside
          if (!m_pVehicle->GetMovement()->IsPowered())
            toggle = false;
          else
          {
            IVehicleSeat* pSeat = m_pVehicle->GetSeatById(1);
            if (pSeat && (!pSeat->GetPassenger() || pSeat->GetCurrentTransition()==IVehicleSeat::eVT_Exiting))
              toggle = false;
          }   
        }
        
        if (toggle)
          ToggleLights(params.bParam);        
      }
    }
    break;

  case eVE_EngineStopped:
    {
      if (eLA_Brake == m_activation)
        ToggleLights(false);
    }
    break;
  }
  
}


//---------------------------------------------------------------------------
void CVehicleSeatActionLights::StopUsing()
{ 
  //ToggleLights(false);
}

void CVehicleSeatActionLights::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->AddContainer(m_lightParts);
}


DEFINE_VEHICLEOBJECT(CVehicleSeatActionLights);
