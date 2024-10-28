/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 

-------------------------------------------------------------------------
History:
- 03:04:2006: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "VehicleHelper.h"
//------------------------------------------------------------------------
const Matrix34& CVehicleHelper::GetVehicleTM()
{
	if (!m_isVehicleUpdated)
	{
		m_vehicleTM = m_localTM;

		IVehiclePart* pParent = m_pParentPart;
		while (pParent)
		{
			m_vehicleTM = pParent->GetLocalTM(true) * m_vehicleTM;
			pParent = pParent->GetParent();
		}

		m_isVehicleUpdated = true;
	}

	return m_vehicleTM;
}

//------------------------------------------------------------------------
const Matrix34& CVehicleHelper::GetWorldTM()
{
    FUNCTION_PROFILER( gEnv->pSystem, PROFILE_GAME);

	if (!m_isWorldUpdated)
	{
		//m_worldTM = m_pParentPart->GetWorldTM() * GetLocalTM(true);

		const Matrix34& partWorldTM = m_pParentPart->GetWorldTM();

		m_worldTM = Matrix34(Matrix33(partWorldTM) * Matrix33(m_localTM));
		m_worldTM.SetTranslation((partWorldTM * m_localTM).GetTranslation());
				
		m_isWorldUpdated = true;
	}

  return m_worldTM;
}

//------------------------------------------------------------------------
const Matrix34& CVehicleHelper::GetReflectedWorldTM()
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_GAME);

	Matrix34 tempMatrix = m_localTM;
	tempMatrix.m03 = -tempMatrix.m03;	// negate x coord of translation

	const Matrix34& partWorldTM = m_pParentPart->GetWorldTM();

	m_reflectedWorldTM = Matrix34(Matrix33(partWorldTM) * Matrix33(tempMatrix));
	m_reflectedWorldTM.SetTranslation((partWorldTM * tempMatrix).GetTranslation());


	return m_reflectedWorldTM;
}

//------------------------------------------------------------------------
IVehiclePart* CVehicleHelper::GetParentPart()
{
	return m_pParentPart;
}

//#include UNIQUE_VIRTUAL_WRAPPER(IVehicleHelper)