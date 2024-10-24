#include "GameServerNub.h"

extern std::uintptr_t CRYACTION_BASE;

void GameServerNub::Update()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2BFBD0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1DE990;
#endif

	(this->*reinterpret_cast<void(GameServerNub::*&)()>(func))();
}

void GameServerNub::ResetOnHoldChannels()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2BFD40;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1DEAB0;
#endif

	(this->*reinterpret_cast<void(GameServerNub::*&)()>(func))();
}

GameServerChannel* GameServerNub::GetChannel(std::uint16_t channelId)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2BF080;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1DE280;
#endif

	return (this->*reinterpret_cast<GameServerChannel*(GameServerNub::*&)(std::uint16_t)>(func))(channelId);
}

std::uint16_t GameServerNub::GetChannelId(INetChannel* pNetChannel)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2BF0D0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1DE310;
#endif

	return (this->*reinterpret_cast<std::uint16_t(GameServerNub::*&)(INetChannel*)>(func))(pNetChannel);
}
