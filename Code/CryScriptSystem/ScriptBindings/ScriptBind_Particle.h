#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_Particle : public CScriptableBase
{
public:
	ScriptBind_Particle(IScriptSystem *pSS);

	int SpawnEffect(IFunctionHandler *pH, const char *effectName, Vec3 pos, Vec3 dir);
	int CreateDecal(IFunctionHandler *pH, Vec3 pos, Vec3 normal, float size, float lifeTime, const char *textureName);
	int CreateMatDecal(IFunctionHandler *pH, Vec3 pos, Vec3 normal, float size, float lifeTime, const char *materialName);
};
