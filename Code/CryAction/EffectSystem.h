#pragma once

#include "CryCommon/CryAction/IEffectSystem.h"

class EffectSystem : public IEffectSystem
{
	void* m_reserved[16 - 1] = {};

public:
	EffectSystem();
	~EffectSystem() override;

	////////////////////////////////////////////////////////////////////////////////
	// IEffectSystem
	////////////////////////////////////////////////////////////////////////////////

	bool Init() override;
	void Update(float delta) override;
	void Shutdown() override;
	void GetMemoryStatistics(ICrySizer*) override;

	EffectId GetEffectId(const char* name) override;

	void Activate(const EffectId& eid) override;
	bool BindEffect(const char* name, IEffect* pEffect) override;
	IGroundEffect* CreateGroundEffect(IEntity* pEntity) override;

	void RegisterFactory(const char *name, IEffect* (*)(), bool isAI) override;

	////////////////////////////////////////////////////////////////////////////////
};
