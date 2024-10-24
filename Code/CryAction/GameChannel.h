#pragma once

#include "CryCommon/CryNetwork/INetwork.h"

class GameChannel : public IGameChannel
{
	INetChannel* m_pNetChannel = nullptr;

protected:
	GameChannel();

public:
	////////////////////////////////////////////////////////////////////////////////
	// IGameChannel
	////////////////////////////////////////////////////////////////////////////////

	void DefineProtocol(IProtocolBuilder* pBuilder) override;
	bool HasDef(const SNetMessageDef* pDef) override;

	void Release() override;
	void OnDisconnect(EDisconnectionCause cause, const char* description) override;

	////////////////////////////////////////////////////////////////////////////////

	INetChannel* GetNetChannel() { return m_pNetChannel; }
};
