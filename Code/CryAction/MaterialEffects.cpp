#include <cstdint>

#include "MaterialEffects.h"

MaterialEffects::MaterialEffects()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30764160;
#else
	std::uintptr_t ctor = 0x306a0620;
#endif

	(this->*reinterpret_cast<void(MaterialEffects::*&)()>(ctor))();
}

bool MaterialEffects::Load(const char* fileName)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x307654f0;
#else
	std::uintptr_t func = 0x306a0f50;
#endif

	return (this->*reinterpret_cast<bool(MaterialEffects::*&)(const char*)>(func))(fileName);
}

bool MaterialEffects::LoadFlowGraphLibs()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x3075e8a0;
#else
	std::uintptr_t func = 0x3069f140;
#endif

	return (this->*reinterpret_cast<bool(MaterialEffects::*&)()>(func))();
}

void MaterialEffects::Update(float frameTime)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x30762890;
#else
	std::uintptr_t func = 0x306a0240;
#endif

	(this->*reinterpret_cast<void(MaterialEffects::*&)(float)>(func))(frameTime);
}

////////////////////////////////////////////////////////////////////////////////
// IMaterialEffects
////////////////////////////////////////////////////////////////////////////////

void MaterialEffects::Reset()
{
}

TMFXEffectId MaterialEffects::GetEffectIdByName(const char* libName, const char* effectName)
{
	return {};
}

TMFXEffectId MaterialEffects::GetEffectId(int surfaceIndex1, int surfaceIndex2)
{
	return {};
}

TMFXEffectId MaterialEffects::GetEffectId(const char* customName, int surfaceIndex2)
{
	return {};
}

TMFXEffectId MaterialEffects::GetEffectId(IEntityClass* pEntityClass, int surfaceIndex2)
{
	return {};
}

SMFXResourceListPtr MaterialEffects::GetResources(TMFXEffectId effectId)
{
	return {};
}

void MaterialEffects::PreLoadAssets()
{
}

bool MaterialEffects::ExecuteEffect(TMFXEffectId effectId, SMFXRunTimeEffectParams& runtimeParams)
{
	return {};
}

void MaterialEffects::StopEffect(TMFXEffectId effectId)
{
}

int MaterialEffects::GetDefaultSurfaceIndex()
{
	return {};
}

int MaterialEffects::GetDefaultCanopyIndex()
{
	return {};
}

bool MaterialEffects::PlayBreakageEffect(ISurfaceType* pSurfaceType, const char* breakageType, const SMFXBreakageParams& mfxBreakageParams)
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
