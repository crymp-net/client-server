/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a attachment socket for vehicle entity attachments

-------------------------------------------------------------------------
History:
- 09:06:2006: Created by MichaelR

*************************************************************************/
#ifndef __VEHICLEPARTATTACHMENT_H__
#define __VEHICLEPARTATTACHMENT_H__

#include "VehiclePartBase.h"

class CVehicle;


class CVehiclePartEntityAttachment
  : public CVehiclePartBase
{
  IMPLEMENT_VEHICLEOBJECT
public:

  // IVehiclePartAttachment
  virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);
  //virtual void InitGeometry();
  virtual void PostInit();
  
  virtual void Update(const float frameTime);  

  virtual const Matrix34& GetLocalTM(bool relativeToParentPart);
  virtual const Matrix34& GetWorldTM();

  virtual void SetLocalTM(const Matrix34& localTM);
  virtual const AABB& GetLocalBounds();

	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); GetBaseMemoryStatistics(s); }
	// ~IVehiclePartAttachment
  
  void SetAttachmentEntity(EntityId entityId);
  IEntity* GetAttachmentEntity();

protected:
  void UpdateAttachment();
  void DetachAttachment();

  EntityId m_attachmentId;
  
  Matrix34 m_localTM;
  Matrix34 m_worldTM;  
};

#endif
