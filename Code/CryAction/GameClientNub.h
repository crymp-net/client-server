#pragma once

#include "CryCommon/CryNetwork/INetwork.h"

class GameClientChannel;

class GameClientNub : public IGameNub
{
	void* m_reserved = nullptr;
	GameClientChannel* m_pGameClientChannel = nullptr;

public:
	////////////////////////////////////////////////////////////////////////////////
	// IGameNub
	////////////////////////////////////////////////////////////////////////////////

	void Release() override;
	SCreateChannelResult CreateChannel(INetChannel* pChannel, const char* connectionString) override;
	void FailedActiveConnect(EDisconnectionCause cause, const char* description) override;

	////////////////////////////////////////////////////////////////////////////////

	GameClientChannel* GetGameClientChannel() const { return m_pGameClientChannel; }
};
