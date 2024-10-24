#pragma once

#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CryNetwork/INetwork.h"

class GameContext
{
	// m_reserved_<32-bit-offset>_<64-bit-offset>
	void* m_reserved_0x0_0x0[14] = {};
	INetContext* m_pNetContext = nullptr;  // m_reserved_0x38_0x70
	void* m_reserved_0x20_0x78[23] = {};
	unsigned int m_reserved_0x98_0x130[4] = {};
	EGameStartFlags m_flags = {};  // m_reserved_0xa8_0x140
	unsigned int m_reserved_0xac_0x144 = 0;
	void* m_reserved_0xb0_0x148 = nullptr;
	bool m_isInLevelLoad = false;  // m_reserved_0xb4_0x150
	bool m_isGameStarted = false;  // m_reserved_0xb5_0x151
	bool m_isLoadingSaveGame = false;  // m_reserved_0xb6_0x152
	void* m_reserved_1[8] = {};
	unsigned int m_reserved_2[4] = {};

public:
	GameContext();

	INetContext* GetNetContext() { return m_pNetContext; }

	EGameStartFlags GetFlags() const { return m_flags; }
	bool IsInLevelLoad() const { return m_isInLevelLoad; }
	bool IsGameStarted() const { return m_isGameStarted; }
	bool IsLoadingSaveGame() const { return m_isLoadingSaveGame; }

	void ResetMap(bool isServer);

	bool ClassIdFromName(std::uint16_t& id, const CryStringT<char>& name) const;
	bool ClassNameFromId(CryStringT<char>& name, std::uint16_t id) const;
};
