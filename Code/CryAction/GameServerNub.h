#pragma once

#include <cstdint>

struct INetChannel;

class GameServerChannel;

class GameServerNub
{
public:
	void Update();
	void ResetOnHoldChannels();

	GameServerChannel* GetChannel(std::uint16_t channelId);
	std::uint16_t GetChannelId(INetChannel* pNetChannel);
};
