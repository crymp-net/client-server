/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: C++ LAM Implementation

-------------------------------------------------------------------------
History:
- 16:10:2006   16:55 : Created by Benito Gangoso Rodriguez

*************************************************************************/
#ifndef LAM_H
#define LAM_H

# pragma once


#include "CryCommon/CryAction/IItemSystem.h"
#include "Item.h"
#include "ItemSharedParams.h"
#include "ItemParamReader.h"
#include "CryCommon/CryCore/VectorSet.h"

class CLam : public CItem
{

public:

	struct SLAMParams
	{
		SLAMParams()
		{
			Reset();
		}

		void Reset(const IItemParamsNode* params = 0, bool defaultInit = true)
		{
			if (defaultInit)
			{
				light_range[0] = light_range[1] = 0.f;
				light_fov[0] = light_fov[1] = 0.f;
				light_color[0] = light_color[1] = Vec3(1, 1, 1);
				light_diffuse_mul[0] = light_diffuse_mul[1] = 1.f;
				light_hdr_dyn[0] = light_hdr_dyn[1] = 0.f;
				light_dir[0] = light_dir[1] = Vec3(0, 1, 0);
				light_texture[0].clear(); light_texture[1].clear();
				light_material[0].clear(); light_material[1].clear();
				laser_geometry_tp.clear();
				fake_laser_geometry_tp.clear();
				laser_dot[0].clear(); laser_dot[1].clear();
				laser_range[0] = laser_range[1] = 50.0f;
				laser_max_len = 20.0f;
				laser_max_scale = 16.0f;

				//FP LAM stuff...
				isLaser = false;
				isFlashLight = false;
				giveExtraAccessory = false;
				extraAccessoryName.clear();
				isLamRifle = false;
			}

			const IItemParamsNode* fp = params ? params->GetChild("firstperson") : 0;
			const IItemParamsNode* tp = params ? params->GetChild("thirdperson") : 0;
			const IItemParamsNode* special = params ? params->GetChild("special") : 0;

			if (fp)
			{
				fp->GetAttribute("light_range", light_range[0]);
				fp->GetAttribute("light_fov", light_fov[0]);
				fp->GetAttribute("light_color", light_color[0]);
				fp->GetAttribute("light_diffuse_mul", light_diffuse_mul[0]);
				fp->GetAttribute("light_hdr_dyn", light_hdr_dyn[0]);
				fp->GetAttribute("light_dir", light_dir[0]);
				light_texture[0] = fp->GetAttribute("light_texture");
				light_material[0] = fp->GetAttribute("light_material");
				laser_dot[0] = fp->GetAttribute("laser_dot");
				fp->GetAttribute("laser_range", laser_range[0]);
			}

			if (tp)
			{
				tp->GetAttribute("light_range", light_range[1]);
				tp->GetAttribute("light_fov", light_fov[1]);
				tp->GetAttribute("light_color", light_color[1]);
				tp->GetAttribute("light_diffuse_mul", light_diffuse_mul[1]);
				tp->GetAttribute("light_hdr_dyn", light_hdr_dyn[1]);
				tp->GetAttribute("light_dir", light_dir[1]);
				light_texture[1] = tp->GetAttribute("light_texture");
				light_material[1] = tp->GetAttribute("light_material");
				laser_geometry_tp = tp->GetAttribute("laser_geometry_tp");
				fake_laser_geometry_tp = tp->GetAttribute("fake_laser_geometry_tp");
				laser_dot[1] = tp->GetAttribute("laser_dot");
				tp->GetAttribute("laser_range", laser_range[1]);
				tp->GetAttribute("laser_max_scale", laser_max_scale);
				tp->GetAttribute("laser_max_len", laser_max_len);
			}

			if (special)
			{
				int temp = 0;
				special->GetAttribute("isLaser", temp);
				isLaser = temp ? true : false;
				special->GetAttribute("isFlashLight", temp);
				isFlashLight = temp ? true : false;
				special->GetAttribute("giveExtraAccessory", temp);
				giveExtraAccessory = temp ? true : false;
				special->GetAttribute("isLamRifle", temp);
				isLamRifle = temp ? true : false;
				extraAccessoryName = special->GetAttribute("extraAccessoryName");

			}
		}

		void GetMemoryStatistics(ICrySizer* s)
		{
			s->Add(light_texture[0]);s->Add(light_texture[1]);
			s->Add(light_material[0]);s->Add(light_material[1]);
			s->Add(laser_dot[0]);s->Add(laser_dot[1]);
			s->Add(laser_geometry_tp);
		}

		Vec3  light_color[2];
		Vec3  light_dir[2];
		float light_diffuse_mul[2];
		float light_range[2];
		float light_fov[2];
		float light_hdr_dyn[2];
		ItemString light_texture[2];
		ItemString light_material[2];
		ItemString laser_geometry_tp;
		ItemString fake_laser_geometry_tp;
		ItemString laser_dot[2];
		float			 laser_range[2];
		float			 laser_max_scale;
		float			 laser_max_len;

		//Special stuff for supporting flashlight/laser separately for the player
		bool	isLaser;
		bool  isFlashLight;
		bool  giveExtraAccessory;
		bool  isLamRifle;
		ItemString extraAccessoryName;
	};

	CLam();
	virtual			~CLam();

	virtual bool Init(IGameObject* pGameObject);
	virtual bool ReadItemParams(const IItemParamsNode* root);
	virtual void Reset();
	virtual void ActivateLaser(bool activate, bool aiRequest = false);
	virtual void ActivateLight(bool activate, bool aiRequest = false);
	virtual void OnAttach(bool attach);
	//virtual void SetViewMode(int mode);
	void OnEnterFirstPerson();
	void OnEnterThirdPerson();
	virtual void Update(SEntityUpdateContext& ctx, int);
	virtual void OnSelect(bool select);
	virtual void OnParentSelect(bool select);

	virtual void PickUp(EntityId pickerId, bool sound, bool select/* =true */, bool keepHistory/* =true */);

	virtual void GetMemoryStatistics(ICrySizer* s);

	virtual void FullSerialize(TSerialize ser);
	virtual void PostSerialize();

	void SetParentId(EntityId parentId) override;

	static  inline  unsigned int GetNumLightsActivated() { return s_lightCount; }

	inline bool IsLaserActivated() const { return m_laserActivated; }
	inline bool IsLightActivated() const { return m_lightActivated; }

protected:

	virtual void AttachLAMLight(bool attach, CItem* pLightAttach, eGeometrySlot slot);
	virtual void AttachLAMLaser(bool attach, eGeometrySlot slot);

	virtual void SaveLastState();

	void DestroyLaserEntity();
	void UpdateFPLaser(float frameTime, CItem* parent);
	void UpdateTPLaser(float frameTime, CItem* parent);

	void ActivateTPLaser(bool activate);
	void ActivateLaserDot(bool activate, bool fp);

	void AdjustLaserFPDirection(CItem* parent, Vec3& dir, Vec3& pos);

	void UpdateAILightAndLaser(const Vec3& pos, const Vec3& dir, float lightRange, float fov, float laserRange);

	void SetAllowUpdate() { m_allowUpdate = true; }

	bool CanUpdate(CActor* pActor);

private:

	SLAMParams m_lamparams;

	//Laser
	bool		m_laserActivated;
	EntityId		m_pLaserEntityId;
	int			m_dotEffectSlot;
	int			m_laserEffectSlot;
	string  m_laserHelperFP;
	//bool    m_lamRifle;				//To fix different orientations for pistol/rifles... (asset issue)

	//Flashlight
	bool    m_lightActivated;
	unsigned int		m_lightID[2];
	tSoundID m_lightSoundId;

	//Track previous state (after select/deselect)
	bool    m_lightWasOn;
	bool    m_laserWasOn;
	Vec3		m_lastLaserHitPt;
	bool		m_lastLaserHitSolid;
	bool    m_lastLaserHitViewPlane;
	bool		m_allowUpdate;
	float		m_updateTime;
	float		m_smoothLaserLength;
	float   m_lastZPos;


	CItem* m_pParent;
	bool	m_DotHidden;
	bool	m_FPMode;
	float   m_lastUpdate;

	bool m_lightActiveSerialize, m_laserActiveSerialize;

	static unsigned int s_lightCount;
};

#endif
