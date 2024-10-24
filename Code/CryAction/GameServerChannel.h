#pragma once

#include <cstdint>

#include "GameChannel.h"

class GameServerChannel : public GameChannel
{
	// m_reserved_<32-bit-offset>_<64-bit-offset>
	void* m_reserved[5] = {};
	std::uint16_t m_reserved_0x1c_0x38 = 0;
	bool m_reserved_0x1e_0x3a = false;
	bool m_isInHold = false;  // m_reserved_0x1f_0x3b

	GameServerChannel();
public:
	bool IsOnHold() const { return m_isInHold; }
};
