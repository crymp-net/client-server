#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISystem;
struct IVehicleSystem;

class ScriptBind_VehicleSystem : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x68 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x54 - sizeof(CScriptableBase)] = {};
#endif

public:
	explicit ScriptBind_VehicleSystem(ISystem* pSystem, IVehicleSystem* pVehicleSystem);
};
