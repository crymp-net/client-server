#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISystem;
struct IGameFramework;
struct IVehicle;

class ScriptBind_Vehicle : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x70 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x58 - sizeof(CScriptableBase)] = {};
#endif

public:
	explicit ScriptBind_Vehicle(ISystem* pSystem, IGameFramework* pGameFramework);

	void AttachTo(IVehicle* pVehicle);
};
