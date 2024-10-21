/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action for triggering animations

-------------------------------------------------------------------------
History:
- 20:07:2007: Created by MichaelR

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"

#include "CryCommon/CryAction/IVehicleSystem.h"
#include "VehicleSeatActionAnimation.h"
#include "VehicleCVars.h"


//------------------------------------------------------------------------
CVehicleSeatActionAnimation::CVehicleSeatActionAnimation()
: m_pVehicleAnim(NULL)
, m_pVehicle(NULL)
, m_userId(0)
, m_action(0.f)
, m_prevAction(0.f)
, m_speed(1.f)
, m_manualUpdate(true)
, m_pSound(NULL)
, m_pStopSound(NULL)
, m_pSoundProxy(NULL)
{	
  m_control[0] = eVAI_Attack1;
  m_control[1] = eVAI_Attack2;
}

//------------------------------------------------------------------------
bool CVehicleSeatActionAnimation::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;	
  
	CVehicleParams animTable = table.findChild("Animation");
	if (!animTable)
		return false;

	if (animTable.haveAttr("vehicleAnimation"))
		m_pVehicleAnim = m_pVehicle->GetAnimation(animTable.getAttr("vehicleAnimation"));

  string control = animTable.getAttr("control");
  if (!control.empty())
  {
    if (control == "roll")
    {
      m_control[0] = eVAI_RollLeft;
      m_control[1] = eVAI_RollRight;
    }
  }   

  animTable.getAttr("manualUpdate", m_manualUpdate);
  animTable.getAttr("speed", m_speed);
  
  m_pSoundProxy = (IEntitySoundProxy*)m_pVehicle->GetEntity()->GetProxy(ENTITY_PROXY_SOUND);

  if (m_pSoundProxy)
  {
    if (animTable.haveAttr("sound"))
      m_pSound = gEnv->pSoundSystem->CreateSound(animTable.getAttr("sound"), FLAG_SOUND_DEFAULT_3D);

    if (animTable.haveAttr("stopSound"))
      m_pStopSound = gEnv->pSoundSystem->CreateSound(animTable.getAttr("stopSound"), FLAG_SOUND_DEFAULT_3D);
  }    
    
	return true;
}

//------------------------------------------------------------------------
void CVehicleSeatActionAnimation::Reset()
{
	if (m_userId)
		StopUsing();
}

//------------------------------------------------------------------------
void CVehicleSeatActionAnimation::StartUsing(EntityId passengerId)
{
	m_userId = passengerId;
  m_action = m_prevAction = 0.f;
	
	if (m_pVehicleAnim)
	{
    if (m_manualUpdate)
    {
		  m_pVehicleAnim->StartAnimation();
		  m_pVehicleAnim->ToggleManualUpdate(true);
      
      m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_PassengerUpdate);
    }
	}
}

//------------------------------------------------------------------------
void CVehicleSeatActionAnimation::StopUsing()
{	
	if (m_pVehicleAnim)
	{
		m_pVehicleAnim->StopAnimation();
    
    if (m_manualUpdate)
      m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_NoUpdate);

    if (m_pSound.get())
      m_pSoundProxy->StopSound(m_pSound->GetId());      
	}

	m_userId = 0;
  m_action = m_prevAction = 0.f;
}

//------------------------------------------------------------------------
void CVehicleSeatActionAnimation::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{	
  if (!m_pVehicleAnim)
    return;  
  	
  if (actionId == m_control[0])
  {
    if (!m_manualUpdate)
    {
      m_pVehicleAnim->StartAnimation();
    }
    else
    {
      if (activationMode == eAAM_OnPress || activationMode == eAAM_OnRelease)
        m_action += value*2.f - 1.f;
    }    
  }
  else if (actionId == m_control[1])
  {
    if (!m_manualUpdate)
    {
      m_pVehicleAnim->StopAnimation();
    }
    else
    {
      if (activationMode == eAAM_OnPress || activationMode == eAAM_OnRelease)
        m_action -= value*2.f - 1.f;
    }    
  }      
}

//------------------------------------------------------------------------
void CVehicleSeatActionAnimation::Update(float frameTime)
{
	if (!m_userId || !m_pVehicleAnim)
		return;

  if (m_action != 0.f)
  {
    float currTime = m_pVehicleAnim->GetAnimTime(true);    
    float newTime = currTime + m_action*m_speed*frameTime;
    Limit(newTime, 0.f, 1.f);

    if (newTime != currTime)
    {
      m_pVehicleAnim->SetTime(newTime);

      // starting
      if (m_prevAction == 0.f && m_pSound.get())      
      {
        m_pSoundProxy->PlaySound(m_pSound);      
        m_pSoundProxy->SetStaticSound(m_pSound->GetId(), true);
      }
    }     

    //float color[] = {1,1,1,1};
    //gEnv->pRenderer->Draw2dLabel(200,250,1.5,color,false,"action: %.2f, time: %.2f, new: %.2f", m_action, currTime, newTime);  
  }
  else
  {
    // stopping
    if (m_prevAction != 0.f)
    { 
      if (m_pSound.get())
        m_pSoundProxy->StopSound(m_pSound->GetId());
                  
      if (m_pStopSound.get())
      {
        m_pSoundProxy->PlaySound(m_pStopSound);
        m_pSoundProxy->SetStaticSound(m_pStopSound->GetId(), true);
      }
    }
  }

	m_prevAction = m_action;  
}

//------------------------------------------------------------------------
void CVehicleSeatActionAnimation::Serialize(TSerialize ser, unsigned aspects)
{
	if (ser.GetSerializationTarget() != eST_Network)
	{ 
	}
}


DEFINE_VEHICLEOBJECT(CVehicleSeatActionAnimation);
