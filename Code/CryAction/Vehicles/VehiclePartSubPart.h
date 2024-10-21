/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a part for vehicles which is the an attachment 
	of a parent Animated part

-------------------------------------------------------------------------
History:
- 25:08:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEPARTSUBPART_H__
#define __VEHICLEPARTSUBPART_H__

#include <map>
#include "VehiclePartBase.h"

class CVehicle;

class CVehiclePartSubPart
	: public CVehiclePartBase
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartSubPart() { m_typeId = eVPT_SubPart; }
	virtual ~CVehiclePartSubPart() {}

	// IVehiclePart
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);  
	virtual void Release();
	virtual void Reset();

	virtual void OnEvent(const SVehiclePartEvent& event);

	virtual bool ChangeState(EVehiclePartState state, int flags=0);
	virtual void Physicalize();

	virtual void Update(const float frameTime);  
	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); GetBaseMemoryStatistics(s); }

	virtual const Matrix34& GetLocalInitialTM() { return m_savedTM; }
	// ~IVehiclePart

protected:
  virtual void InitGeometry();

  Matrix34 m_savedTM;

};

#endif
