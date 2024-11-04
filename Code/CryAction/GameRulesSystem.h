#pragma once

#include "CryCommon/CryAction/IGameRulesSystem.h"

struct ISystem;
struct IGameFramework;

class GameRulesSystem : public IGameRulesSystem
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x50 - 0x8] = {};
#else
	unsigned char m_data[0x28 - 0x4] = {};
#endif

public:
	explicit GameRulesSystem(ISystem* pSystem, IGameFramework* pGameFramework);

	////////////////////////////////////////////////////////////////////////////////
	// IGameRulesSystem
	////////////////////////////////////////////////////////////////////////////////

	bool RegisterGameRules(const char* rulesName, const char* extensionName) override;

	bool CreateGameRules(const char* rulesName) override;
	bool DestroyGameRules() override;

	void AddGameRulesAlias(const char* rulesName, const char* alias) override;
	void AddGameRulesLevelLocation(const char* rulesName, const char* mapLocation) override;

	const char* GetGameRulesLevelLocation(const char* rulesName, int index) override;
	const char* GetGameRulesName(const char* alias) const override;
	bool HaveGameRules(const char* rulesName) override;

	void SetCurrentGameRules(IGameRules* pGameRules) override;
	IGameRules* GetCurrentGameRules() const override;

	////////////////////////////////////////////////////////////////////////////////
};
