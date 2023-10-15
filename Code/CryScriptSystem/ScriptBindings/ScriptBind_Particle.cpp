#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/Cry3DEngine/I3DEngine.h"
#include "CryCommon/Cry3DEngine/CryParticleSpawnInfo.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"

#include "ScriptBind_Particle.h"

ScriptBind_Particle::ScriptBind_Particle(IScriptSystem *pSS)
{
	CScriptableBase::Init(pSS, gEnv->pSystem);
	SetGlobalName("Particle");

	pSS->SetGlobalValue("CRYPARTICLE_RAIN_MODE", CryParticleSpawnInfo::FLAGS_RAIN_MODE);
	pSS->SetGlobalValue("CRYPARTICLE_ONE_TIME_SPAWN", CryParticleSpawnInfo::FLAGS_ONE_TIME_SPAWN);

	pSS->SetGlobalValue("ParticleBlendType_AlphaBased", ParticleBlendType_AlphaBased);
	pSS->SetGlobalValue("ParticleBlendType_ColorBased", ParticleBlendType_ColorBased);
	pSS->SetGlobalValue("ParticleBlendType_Additive", ParticleBlendType_Additive);
	pSS->SetGlobalValue("ParticleBlendType_None", ParticleBlendType_None);

#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_Particle::

	SCRIPT_REG_TEMPLFUNC(SpawnEffect, "effectName, pos, dir, [scale], [entityId], [partId]");
	SCRIPT_REG_TEMPLFUNC(CreateDecal, "pos, normal, size, lifeTime, textureName, [angle], [hitDirection], [entityId], [partid]");
	SCRIPT_REG_TEMPLFUNC(CreateMatDecal, "pos, normal, size, lifeTime, materialName, [angle], [hitDirection], [entityId], [partid]");
}

int ScriptBind_Particle::SpawnEffect(IFunctionHandler *pH, const char *effectName, Vec3 pos, Vec3 dir)
{
	int entitySlot = -1;

	IParticleEffect *pEffect = gEnv->p3DEngine->FindParticleEffect(effectName, "", "Particle.SpawnEffect");
	if (pEffect)
	{
		float scale = 1.0f;
		if ((pH->GetParamCount() > 3) && (pH->GetParamType(4) == svtNumber))
			pH->GetParam(4, scale);

		ScriptHandle entityId(0);
		if ((pH->GetParamCount() > 4) && (pH->GetParamType(5) == svtPointer))
			pH->GetParam(5, entityId);

		int partId = 0;
		if ((pH->GetParamCount() > 5) && (pH->GetParamType(6) == svtNumber))
			pH->GetParam(6, partId);

		IEntity *pEntity = nullptr;
		if (entityId.n)
			pEntity = gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId.n));

		if (pEntity)
			entitySlot = pEntity->LoadParticleEmitter(-1, pEffect);
		else
			pEffect->Spawn(true, IParticleEffect::ParticleLoc(pos, dir, scale));
	}

	return pH->EndFunction(entitySlot);
}

static void SetAdditionalDecalInfo(CryEngineDecalInfo & decal, IFunctionHandler *pH)
{
	if ((pH->GetParamCount() > 5) && (pH->GetParamType(6) != svtNull))
	{
		pH->GetParam(6, decal.fAngle);
	}

	if ((pH->GetParamCount() > 6) && (pH->GetParamType(7) != svtNull))
	{
		pH->GetParam(7, decal.vHitDirection);
	}

	if ((pH->GetParamCount() > 7) && (pH->GetParamType(8) != svtNull))
	{
		ScriptHandle entityId;
		pH->GetParam(8, entityId);

		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId.n));
		if (pEntity)
		{
			IEntityRenderProxy *pProxy = (IEntityRenderProxy*) pEntity->GetProxy(ENTITY_PROXY_RENDER);
			if (pProxy)
			{
				decal.ownerInfo.pRenderNode = pProxy->GetRenderNode();
			}
		}
	}
	else if ((pH->GetParamCount() > 8) && (pH->GetParamType(9) != svtNull))
	{
		ScriptHandle renderNode;
		pH->GetParam(9, renderNode);
		decal.ownerInfo.pRenderNode = (IRenderNode*) renderNode.ptr;
	}

	if ((pH->GetParamCount() > 9) && (pH->GetParamType(10) != svtNull))
	{
		float growTime = 0.0f;
		pH->GetParam(10, growTime);
		decal.fGrowTime = growTime;
	}

	if ((pH->GetParamCount() > 10) && (pH->GetParamType(11) != svtNull))
	{
		bool skip = false;
		pH->GetParam(11, skip);
		decal.bSkipOverlappingTest = skip;
	}
}

int ScriptBind_Particle::CreateDecal(IFunctionHandler *pH, Vec3 pos, Vec3 normal, float size, float lifeTime, const char *textureName)
{
	CryEngineDecalInfo decal;
	decal.vPos = pos;
	decal.vNormal = normal;
	decal.fSize = size;
	decal.fLifeTime = lifeTime;
	decal.vHitDirection = -normal;
	SetAdditionalDecalInfo(decal, pH);

	CryLogWarning("Particle.CreateDecal: Decal material name is not specified");

	gEnv->p3DEngine->CreateDecal(decal);

	return pH->EndFunction();
}

int ScriptBind_Particle::CreateMatDecal(IFunctionHandler *pH, Vec3 pos, Vec3 normal, float size, float lifeTime, const char *materialName)
{
	CryEngineDecalInfo decal;
	decal.vPos = pos;
	decal.vNormal = normal;
	decal.fSize = size;
	decal.fLifeTime = lifeTime;
	decal.vHitDirection = -normal;
	strcpy(decal.szMaterialName, materialName);
	SetAdditionalDecalInfo(decal, pH);

	gEnv->p3DEngine->CreateDecal(decal);

	return pH->EndFunction();
}
