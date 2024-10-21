/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat group

-------------------------------------------------------------------------
History:
- 12:03:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLESEATGROUP_H__
#define __VEHICLESEATGROUP_H__

#include "vector"

class CVehicleSeat;

class CVehicleSeatGroup
{
public:

	bool Init(IVehicle* pVehicle, const CVehicleParams& paramsTable);
	void Reset();
	void Release() { delete this; }

	void GetMemoryStatistics(ICrySizer * s);

	unsigned int GetSeatCount() { return m_seats.size(); }
	CVehicleSeat* GetSeatByIndex(unsigned int index);
	CVehicleSeat* GetNextSeat(CVehicleSeat* pCurrentSeat);
	CVehicleSeat* GetNextFreeSeat(CVehicleSeat* pCurrentSeat);

	bool IsGroupEmpty();

	void OnPassengerEnter(CVehicleSeat* pSeat, EntityId passengerId) {}
	void OnPassengerExit(CVehicleSeat* pSeat, EntityId passengerId);
	void OnPassengerChangeSeat(CVehicleSeat* pNewSeat, CVehicleSeat* pOldSeat);

protected:

	CVehicle* m_pVehicle;

	bool m_isKeepingEngineWarm;

	typedef std::vector <CVehicleSeat*> TVehicleSeatVector;
	TVehicleSeatVector m_seats;

	bool m_isSwitchingReverse;

	friend class CVehicleSeat;
};

#endif
