/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a part for vehicles which uses static objects

-------------------------------------------------------------------------
History:
- 23:08:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEPARTSTATIC_H__
#define __VEHICLEPARTSTATIC_H__

#include <map>

class CVehicle;

class CVehiclePartStatic
	: public CVehiclePartBase
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartStatic() { m_typeId = eVPT_Static; }
	~CVehiclePartStatic() {}

	// IVehiclePart
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);
  virtual void InitGeometry();
	virtual void Release();
	virtual void Reset();
	
	virtual void Physicalize();

	virtual void Update(const float frameTime);

  virtual void SetLocalTM(const Matrix34& localTM);

	virtual void GetMemoryStatistics(ICrySizer * s) 
	{ 
		s->Add(*this); 
		GetBaseMemoryStatistics(s);
		s->Add(m_filename);
		s->Add(m_filenameDestroyed);
		s->Add(m_geometry);
	}
	// ~IVehiclePart

protected:

	string m_filename;
	string m_filenameDestroyed;
	string m_geometry;
};

#endif
