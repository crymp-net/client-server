/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which send an AI signal

-------------------------------------------------------------------------
History:
- 23:11:2006: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAISystem/IAgent.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "VehicleDamageBehaviorAISignal.h"

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorAISignal::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;

	CVehicleParams signalTable = table.findChild("AISignal");
	if (!signalTable)
		return false;

	if (!signalTable.getAttr("signalId", m_signalId))
		return false;
	
	m_signalText = signalTable.getAttr("signalText");

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorAISignal::OnDamageEvent(EVehicleDamageBehaviorEvent event, 
																									 const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if (event != eVDBE_Hit)
		return;

	IEntity* pEntity = m_pVehicle->GetEntity();
	if (!pEntity || !pEntity->GetAI())
		return;

	IAISystem* pAISystem = gEnv->pAISystem;
	assert(pAISystem);

	IAISignalExtraData* pExtraData = pAISystem->CreateSignalExtraData();
	assert(pExtraData);

	if (pExtraData)
		pExtraData->iValue = behaviorParams.shooterId;

	pAISystem->SendSignal(SIGNALFILTER_SENDER, m_signalId, m_signalText.c_str(), pEntity->GetAI(), pExtraData);
}

void CVehicleDamageBehaviorAISignal::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->Add(m_signalText);
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorAISignal);
