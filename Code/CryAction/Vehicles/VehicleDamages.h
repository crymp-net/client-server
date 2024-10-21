/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements the base of the vehicle damages

-------------------------------------------------------------------------
History:
- 23:02:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGES_H__
#define __VEHICLEDAMAGES_H__

#include <map>
#include <vector>

#include "VehicleDamagesGroup.h"

class CVehicle;

class CVehicleDamages
{
public:

  CVehicleDamages() : m_pVehicle(NULL) {}
  
  struct SDamageMultiplier
  {
    float mult;
    float splash;

    SDamageMultiplier() : mult(1.f), splash(1.f) {}    
  };
  
	void InitDamages(CVehicle* pVehicle, const CVehicleParams& table);
	void ReleaseDamages();
	void ResetDamages();
	void UpdateDamages(float frameTime);

	void GetDamagesMemoryStatistics(ICrySizer* pSizer);

	bool ProcessHit(float& damage, const char* hitClass, bool splash);
  CVehicleDamagesGroup* GetDamagesGroup(const char* groupName);
      
	typedef std::map <string, SDamageMultiplier> TDamageMultipliers;
	static void ParseDamageMultipliers(TDamageMultipliers& multipliers, const CVehicleParams& table);

protected:

	CVehicle* m_pVehicle;

	typedef std::vector <CVehicleDamagesGroup*> TVehicleDamagesGroupVector;
	TVehicleDamagesGroupVector m_damagesGroups;
  
  SVehicleDamageParams m_damageParams;  

	TDamageMultipliers m_damageMultipliers;
};

#endif
