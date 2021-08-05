#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_Particle : public CScriptableBase
{
public:
	ScriptBind_Particle(IScriptSystem *pSS);

	//! <code>Particle.SpawnEffect( effectName, pos, dir )</code>
	//! <param name="effectName">Effect name.</param>
	//! <param name="pos">Position vector.</param>
	//! <param name="dir">Direction vector.</param>
	//! <return>SlotId where the emitter was bound to if a valid EntityId was given.</return>
	//! <description>Spawns an effect.</description>
	int SpawnEffect(IFunctionHandler *pH, const char *effectName, Vec3 pos, Vec3 dir);

	//! <code>Particle.EmitParticle( pos, dir, entityId, slotId )</code>
	//! <param name="pos">Particle position.</param>
	//! <param name="dir">Particle direction.</param>
	//! <param name="entityId">EntityId of the previously associated entity.</param>
	//! <param name="slotId">SlotId where the emitter was bound to.</param>
	//! <description>Emits an individual particle previously associanted with an entity.</description>
	int EmitParticle(IFunctionHandler *pH, Vec3 pos, Vec3 dir, ScriptHandle entityId, int slotId);

	//! <code>Particle.CreateDecal( pos, normal, size, lifeTime, textureName )</code>
	//! <param name="pos">Decal position.</param>
	//! <param name="normal">Decal normal vector.</param>
	//! <param name="size">Decal size.</param>
	//! <param name="lifeTime">Decal life time.</param>
	//! <param name="textureName - Name of the texture.</param>
	//! <description>Creates a decal with the specified parameters.</description>
	int CreateDecal(IFunctionHandler *pH, Vec3 pos, Vec3 normal, float size, float lifeTime, const char *textureName);

	//! <code>Particle.CreateMatDecal( pos, normal, size, lifeTime, materialName )</code>
	//! <param name="pos">Decal position.</param>
	//! <param name="normal">Decal normal vector.</param>
	//! <param name="size">Decal size.</param>
	//! <param name="lifeTime">Decal life time.</param>
	//! <param name="materialName">Name of the Material.</param>
	//! <description>Creates a material decal.</description>
	int CreateMatDecal(IFunctionHandler *pH, Vec3 pos, Vec3 normal, float size, float lifeTime, const char *materialName);
};
