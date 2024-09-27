#pragma once

#include "CryCommon/CryAction/IGameTokens.h"

class GameTokenSystem : public IGameTokenSystem
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x78 - 0x8] = {};
#else
	unsigned char m_data[0x3c - 0x4] = {};
#endif

public:
	GameTokenSystem();

	////////////////////////////////////////////////////////////////////////////////
	// IGameFramework
	////////////////////////////////////////////////////////////////////////////////

	void GetMemoryStatistics(ICrySizer*) override;

	IGameToken* SetOrCreateToken(const char* tokenName, const TFlowInputData& defaultValue) override;
	void DeleteToken(IGameToken* pToken) override;
	IGameToken* FindToken(const char* tokenName) override;
	void RenameToken(IGameToken* pToken, const char* newTokenName) override;

	void RegisterListener(const char* gameToken, IGameTokenEventListener* pListener, bool forceCreate, bool immediateCallback) override;
	void UnregisterListener(const char* gameToken, IGameTokenEventListener* pListener) override;

	void RegisterListener(IGameTokenEventListener* pListener) override;
	void UnregisterListener(IGameTokenEventListener* pListener) override;

	void LoadLibs(const char* fileSpec) override;
	void RemoveLibrary(const char* prefix) override;

	void Reset() override;

	void Serialize(TSerialize ser) override;

	void DebugDraw() override;

	////////////////////////////////////////////////////////////////////////////////
};
