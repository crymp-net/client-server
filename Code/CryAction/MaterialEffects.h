#pragma once

#include "CryCommon/CryAction/IMaterialEffects.h"

class MaterialEffects : public IMaterialEffects
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x120 - 0x8] = {};
#else
	unsigned char m_data[0x94 - 0x4] = {};
#endif

public:
	MaterialEffects();

	bool Load(const char* fileName);
	bool LoadFlowGraphLibs();

	void Update(float frameTime);

	////////////////////////////////////////////////////////////////////////////////
	// IMaterialEffects
	////////////////////////////////////////////////////////////////////////////////

	void Reset() override;

	TMFXEffectId GetEffectIdByName(const char* libName, const char* effectName) override;
	TMFXEffectId GetEffectId(int surfaceIndex1, int surfaceIndex2) override;
	TMFXEffectId GetEffectId(const char* customName, int surfaceIndex2) override;
	TMFXEffectId GetEffectId(IEntityClass* pEntityClass, int surfaceIndex2) override;
	SMFXResourceListPtr GetResources(TMFXEffectId effectId) override;

	void PreLoadAssets() override;

	bool ExecuteEffect(TMFXEffectId effectId, SMFXRunTimeEffectParams& runtimeParams) override;
	void StopEffect(TMFXEffectId effectId) override;

	int GetDefaultSurfaceIndex() override;
	int GetDefaultCanopyIndex() override;

	bool PlayBreakageEffect(ISurfaceType* pSurfaceType, const char* breakageType, const SMFXBreakageParams& mfxBreakageParams) override;

	////////////////////////////////////////////////////////////////////////////////
};
