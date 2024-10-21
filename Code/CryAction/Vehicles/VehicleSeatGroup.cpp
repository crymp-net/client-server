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
#include "StdAfx.h"
#include "CryAction.h"
#include "GameObjects/GameObject.h"
#include "IActorSystem.h"
#include "IAnimatedCharacter.h"
#include "IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleSeat.h"
#include "VehicleSeatGroup.h"

//------------------------------------------------------------------------
bool CVehicleSeatGroup::Init(IVehicle* pVehicle, const CVehicleParams& paramsTable)
{
	m_pVehicle = static_cast<CVehicle*>(pVehicle);
	m_isSwitchingReverse = false;

	if (CVehicleParams seatsTable = paramsTable.findChild("Seats"))
	{
		int i = 0;
		int c = seatsTable.getChildCount();
		m_seats.reserve(c);

		for (; i < c; i++)
		{
			string seatName = seatsTable.getChild(i).getAttr("value");
			if (!seatName.empty())
			{
				TVehicleSeatId seatId = m_pVehicle->GetSeatId(seatName);
				if (CVehicleSeat* pSeat = (CVehicleSeat*)m_pVehicle->GetSeatById(seatId))
				{
					pSeat->m_pSeatGroup = this;
					m_seats.push_back(pSeat);
				}
			}
		}
	}

	if (!paramsTable.getAttr("keepEngineWarm", m_isKeepingEngineWarm))
		m_isKeepingEngineWarm = false;

	return (m_seats.size() > 0);
}

//------------------------------------------------------------------------
void CVehicleSeatGroup::Reset()
{
	m_isSwitchingReverse = false;
}

//------------------------------------------------------------------------
CVehicleSeat* CVehicleSeatGroup::GetSeatByIndex(unsigned int index)
{
	if (index >= 0 && index <= m_seats.size())
		return m_seats[index];

	return NULL;
}

//------------------------------------------------------------------------
CVehicleSeat* CVehicleSeatGroup::GetNextSeat(CVehicleSeat* pCurrentSeat)
{
	for (TVehicleSeatVector::iterator ite = m_seats.begin(); ite != m_seats.end(); ++ite)
	{
		CVehicleSeat* pSeat = *ite;
		if (pSeat == pCurrentSeat)
		{
			if (m_isSwitchingReverse)
			{
				if (ite == m_seats.begin())
				{
					m_isSwitchingReverse = false;
					++ite;
				}
				else
					--ite;
			}
			else
			{
				++ite;

				if (ite == m_seats.end())
				{
					m_isSwitchingReverse = true;
					--ite;
					--ite;
				}
			}

			return *ite;
		}
	}

	return NULL;
}

//------------------------------------------------------------------------
CVehicleSeat* CVehicleSeatGroup::GetNextFreeSeat(CVehicleSeat* pCurrentSeat)
{ 
  TVehicleSeatVector::iterator itCurrent = std::find(m_seats.begin(), m_seats.end(), pCurrentSeat);
  
  if (itCurrent == m_seats.end())
    return NULL;
  
  TVehicleSeatVector::iterator itFwd = itCurrent, itBack = itCurrent;

  while (!(itFwd == m_seats.end() && itBack == m_seats.begin()))
  {
    if (m_isSwitchingReverse)
    {
      if (itBack != m_seats.begin())    
      {
        --itBack;

        if ((*itBack)->IsFree())
          return *itBack;          
      }
      else
      {
        m_isSwitchingReverse = false;
      }
    }
    else 
    {
      if (itFwd != m_seats.end())
      {
        ++itFwd;

        if (itFwd == m_seats.end())
        {
          m_isSwitchingReverse = true;
        }
        else
        { 
          if ((*itFwd)->IsFree())
            return *itFwd;    
        }
      }
      else
      {
        m_isSwitchingReverse = true;
      } 
    }
  }

	return NULL;
}

//------------------------------------------------------------------------
bool CVehicleSeatGroup::IsGroupEmpty()
{
	const TVehicleSeatVector::const_iterator seatsEnd = m_seats.end();
	for (TVehicleSeatVector::const_iterator ite = m_seats.begin(); ite != seatsEnd; ++ite)
	{
		CVehicleSeat* pSeat = *ite;
		if (!pSeat->IsFree())
			return false;
	}

	return true;
}

//------------------------------------------------------------------------
void CVehicleSeatGroup::OnPassengerExit(CVehicleSeat* pSeat, EntityId passengerId)
{

}

//------------------------------------------------------------------------
void CVehicleSeatGroup::OnPassengerChangeSeat(CVehicleSeat* pNewSeat, CVehicleSeat* pOldSeat)
{

}

void CVehicleSeatGroup::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->AddContainer(m_seats);
}