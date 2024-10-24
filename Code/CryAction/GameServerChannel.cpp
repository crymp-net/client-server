#include <cstddef>

#include "GameServerChannel.h"

GameServerChannel::GameServerChannel()
{
#ifdef BUILD_64BIT
	static_assert(offsetof(GameServerChannel, m_isInHold) == 0x3b);
#else
	static_assert(offsetof(GameServerChannel, m_isInHold) == 0x1f);
#endif

	// TODO
}
