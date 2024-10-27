#pragma once

#include "CryCommon/CryNetwork/NetHelpers.h"

#include "GameChannel.h"

class GameClientChannel : public CNetMessageSinkHelper<GameClientChannel, GameChannel>
{
};
