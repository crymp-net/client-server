/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a simple box-shaped part for mass distribution

-------------------------------------------------------------------------
History:
- 17:10:2005: Created by MichaelR

*************************************************************************/
#ifndef __VEHICLEPARTMASSBOX_H__
#define __VEHICLEPARTMASSBOX_H__

#include <map>
#include "VehiclePartBase.h"

class CVehicle;


class CVehiclePartMassBox
	: public CVehiclePartBase
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartMassBox();
	~CVehiclePartMassBox();

  // IVehiclePart
  virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);
  virtual void Reset();
	
  virtual void Physicalize();
  
  virtual const Matrix34& GetLocalTM(bool relativeToParentPart);
  virtual const Matrix34& GetWorldTM();
  virtual const AABB& GetLocalBounds();
	  
  virtual void Update(const float frameTime);
  virtual void Serialize(TSerialize ser, EEntityAspects aspects);

	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); GetBaseMemoryStatistics(s); }
  // ~IVehiclePart

protected:  

	Matrix34 m_localTM;
  Matrix34 m_worldTM;  
  Vec3 m_size;  
  float m_drivingOffset;  

private:
	enum EMassBoxDrivingType
	{
		eMASSBOXDRIVING_DEFAULT		=   1<<0,
		eMASSBOXDRIVING_NORMAL		=   1<<1,
		eMASSBOXDRIVING_INTHEAIR	=   1<<2,
		eMASSBOXDRIVING_INTHEWATER	=	1<<3,
		eMASSBOXDRIVING_DESTOYED	=	1<<4,
	};
	void SetupDriving(EMassBoxDrivingType drive);
	EMassBoxDrivingType m_Driving; //0 default 1 driving 2 in the air
};

#endif
