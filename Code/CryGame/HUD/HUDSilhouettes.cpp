/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:
	Silhouettes manager

-------------------------------------------------------------------------
History:
- 20:07:2007: Created by Julien Darre

*************************************************************************/

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "HUDSilhouettes.h"
#include "CryGame/Items/Item.h"
#include "HUD.h"

//-----------------------------------------------------------------------------------------------------

CHUDSilhouettes::CHUDSilhouettes(CHUD *pHUD)
{
	m_pHUD = pHUD;
	m_silhouettesVector.resize(256);
}

//-----------------------------------------------------------------------------------------------------

CHUDSilhouettes::~CHUDSilhouettes()
{
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::SetVisionParams(EntityId uiEntityId,ColorF color)
{
	// When quick loading, entities may have been already destroyed when we do a ShowBinoculars(false)
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(uiEntityId);
	if(!pEntity)
		return;

	// Some actor accessories may not have render proxy
	IEntityRenderProxy *pEntityRenderProxy = static_cast<IEntityRenderProxy *>(pEntity->GetProxy(ENTITY_PROXY_RENDER));
	if(!pEntityRenderProxy)
		return;

	if (pEntityRenderProxy->GetRenderNode()->GetDrawFrame() != gEnv->pRenderer->GetFrameID()) //CryMP: only set visionparams on entities that are drawn
		return;

	pEntityRenderProxy->SetVisionParams(color.r, color.g, color.b, color.a);
	
	//CryMP: 
	//Some silhouette has been drawn this frame, we need to trigger fix in HUDTagNames
	m_pHUD->m_nameTagsNeedFix = true;
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::ResetVisionParams(EntityId uiEntityId)
{
	IEntity* pEntity = gEnv->pEntitySystem->GetEntity(uiEntityId);
	if (!pEntity)
		return;

	IEntityRenderProxy* pEntityRenderProxy = static_cast<IEntityRenderProxy*>(pEntity->GetProxy(ENTITY_PROXY_RENDER));
	if (!pEntityRenderProxy)
		return;

	pEntityRenderProxy->SetVisionParams(0.0f, 0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::SetFlowGraphSilhouette(IEntity *pEntity,ColorF color,float fDuration)
{
	if(!pEntity)
		return;

	if(GetFGSilhouette(pEntity->GetId()) != m_silhouettesFGVector.end())
		return;

	SetSilhouette(pEntity, color, fDuration);
	m_silhouettesFGVector[pEntity->GetId()] = Vec3(color.r,color.g,color.b);
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::ResetFlowGraphSilhouette(EntityId uiEntityId)
{
	std::map<EntityId, Vec3>::iterator it = GetFGSilhouette(uiEntityId);
	if(it != m_silhouettesFGVector.end())
	{
		m_silhouettesFGVector.erase(it);
		ResetSilhouette(uiEntityId);
		return;
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::SetSilhouette(IEntity *pEntity,ColorF color,float fDuration)
{
	if(!pEntity)
		return;

	SSilhouette *pSilhouette = nullptr;

	// First pass: is that Id already in a slot?
	for (SSilhouette &pSil : m_silhouettesVector)
	{
		if(pEntity->GetId() == pSil.uiEntityId)
		{
			pSilhouette = &pSil;
			break;
		}
	}

	if(!pSilhouette)
	{
		// Second pass: try to find a free slot
		for (SSilhouette &pSil : m_silhouettesVector)
		{
			if(!pSil.bValid)
			{
				pSilhouette = &pSil;
				break;
			}
		}
	}

	assert(pSilhouette);  // Vector size should be increased!

	if(pSilhouette)
	{
		pSilhouette->uiEntityId	= pEntity->GetId();
		pSilhouette->fTime = fDuration;
		pSilhouette->bValid			= true;
		pSilhouette->r = color.r;
		pSilhouette->g = color.g;
		pSilhouette->b = color.b;
		pSilhouette->a = color.a;

		SetVisionParams(pEntity->GetId(),color);
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::SetSilhouette(IActor *pActor,ColorF color,float fDuration,bool bHighlightCurrentItem,bool bHighlightAccessories)
{
	if(!pActor)
		return;

	SetSilhouette(pActor->GetEntity(),color,fDuration);

	if(bHighlightCurrentItem)
	{
		SetSilhouette(pActor->GetCurrentItem(),color,fDuration,bHighlightAccessories);
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::SetSilhouette(IItem *pItem,ColorF color,float fDuration,bool bHighlightAccessories)
{
	if(!pItem)
		return;

	SetSilhouette(pItem->GetEntity(),color,fDuration);

	if(bHighlightAccessories)
	{
		const CItem::TAccessoryMap *pAccessoryMap = static_cast<CItem *>(pItem)->GetAttachedAccessories();
		for(CItem::TAccessoryMap::const_iterator iter=pAccessoryMap->begin(); iter!=pAccessoryMap->end(); ++iter)
		{
			SetSilhouette(gEnv->pEntitySystem->GetEntity((*iter).second),color,fDuration);
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::SetSilhouette(IVehicle *pVehicle,ColorF color,float fDuration)
{
	if(!pVehicle)
		return;

	SetSilhouette(pVehicle->GetEntity(),color,fDuration);
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::ResetSilhouette(EntityId uiEntityId)
{
	for (SSilhouette &pSilhouette : m_silhouettesVector)
	{
		if (pSilhouette.uiEntityId == uiEntityId && pSilhouette.bValid)
		{
			std::map<EntityId, Vec3>::iterator it = GetFGSilhouette(pSilhouette.uiEntityId);
			if(it != m_silhouettesFGVector.end())
			{
				Vec3 color = it->second;
				SetVisionParams(uiEntityId, ColorF(color.x, color.y, color.z, 1.0f));
			}
			else
			{
				ResetVisionParams(pSilhouette.uiEntityId);
				pSilhouette.bValid = false;
			}

			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::SetType(int iType)
{
	// Exit of binoculars: we need to reset all silhouettes
	if (!iType)
	{
		for (SSilhouette& pSilhouette : m_silhouettesVector)
		{
			if (pSilhouette.bValid)
			{
				std::map<EntityId, Vec3>::iterator it = GetFGSilhouette(pSilhouette.uiEntityId);
				if(it != m_silhouettesFGVector.end())
				{
					Vec3 color = it->second;
					SetVisionParams(pSilhouette.uiEntityId, ColorF(color.x, color.y, color.z, 1.0f));
				}
				else
				{
					ResetVisionParams(pSilhouette.uiEntityId);
					pSilhouette.bValid = false;
				}
			}

		}
	}

	gEnv->p3DEngine->SetPostEffectParam("CryVision_Type",1-iType);
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::Update(float frameTime)
{
	for (SSilhouette &pSilhouette : m_silhouettesVector)
	{
		if(pSilhouette.bValid && pSilhouette.fTime != -1)
		{
			pSilhouette.fTime -= frameTime;
			if(pSilhouette.fTime < 0.0f)
			{
				ResetVisionParams(pSilhouette.uiEntityId);
				pSilhouette.bValid = false;
				pSilhouette.fTime = 0.0f;
			}
			else if (pSilhouette.fTime < 1.0f)
			{
				// fade out for the last second
				float scale = pSilhouette.fTime ;
				scale *= scale;
				SetVisionParams(pSilhouette.uiEntityId,ColorF(pSilhouette.r*scale,pSilhouette.g*scale,pSilhouette.b*scale,pSilhouette.a*scale));
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDSilhouettes::Serialize(TSerialize &ser)
{
	if(ser.IsReading())
		m_silhouettesFGVector.clear();

	int amount = m_silhouettesFGVector.size();
	ser.Value("amount", amount);
	if(amount)
	{
		EntityId id = 0;
		Vec3 color;
		if(ser.IsWriting())
		{
			std::map<EntityId, Vec3>::iterator it = m_silhouettesFGVector.begin();
			std::map<EntityId, Vec3>::iterator end = m_silhouettesFGVector.end();
			for(; it != end; ++it)
			{
				ser.BeginGroup("FlowGraphSilhouette");
				id = it->first;
				color = it->second;
				ser.Value("id", id);
				ser.Value("color", color);
				ser.EndGroup();
			}
		}
		else if(ser.IsReading())
		{
			for(int i = 0; i < amount; ++i)
			{
				ser.BeginGroup("FlowGraphSilhouette");
				ser.Value("id", id);
				ser.Value("color", color);
				ser.EndGroup();
				if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
					SetFlowGraphSilhouette(pEntity, ColorF(color.x, color.y, color.z, 1.0f), -1.0f);
			}
		}
	}
}

std::map<EntityId, Vec3>::iterator CHUDSilhouettes::GetFGSilhouette(EntityId id)
{
	std::map<EntityId, Vec3>::iterator returnVal = m_silhouettesFGVector.end();

	std::map<EntityId, Vec3>::iterator it = m_silhouettesFGVector.begin();
	std::map<EntityId, Vec3>::iterator end = m_silhouettesFGVector.end();
	for(; it != end; ++it)
	{
		if(it->first == id)
			returnVal = it;
	}
	return returnVal;
}
