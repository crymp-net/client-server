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
#ifndef __VEHICLEHELPER_H__
#define __VEHICLEHELPER_H__

class CVehicle;

class CVehicleHelper
	: public IVehicleHelper
{
public:

	// IVehicleHelper
	virtual void Release() { delete this; }

	VIRTUAL const Matrix34& GetLocalTM() { return m_localTM; }
	VIRTUAL const Matrix34& GetVehicleTM();
	VIRTUAL const Matrix34& GetWorldTM();
	VIRTUAL const Matrix34& GetReflectedWorldTM();

	VIRTUAL IVehiclePart* GetParentPart();

	VIRTUAL void Invalidate()
	{
		m_isWorldUpdated = false;
		m_isVehicleUpdated = false;
	}
	// ~IVehicleHelper

protected:

	IVehiclePart* m_pParentPart;

	Matrix34 m_localTM;
  Matrix34 m_vehicleTM;
	Matrix34 m_worldTM;
	Matrix34 m_reflectedWorldTM;

	bool m_isWorldUpdated;
	bool m_isVehicleUpdated;

	friend class CVehicle;
};

#endif
