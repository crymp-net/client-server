#pragma once

#include "CryCommon/CryNetwork/INetwork.h"

class GameQueryListener : public IGameQueryListener
{
	// m_reserved_<32-bit-offset>_<64-bit-offset>
	INetQueryListener* m_pNetListener;  // m_reserved_0x4_0x8
	void* m_reserved_0x8_0x10[123] = {};
	unsigned int m_reserved[2] = {};

public:
	GameQueryListener();

	void SetNetListener(INetQueryListener* pListener);
	void Complete();

	////////////////////////////////////////////////////////////////////////////////
	// IGameQueryListener
	////////////////////////////////////////////////////////////////////////////////

	void AddServer(const char* description, const char* target, const char* additionalText, uint32 ping) override;
	void RemoveServer(string address) override;
	void AddPong(string address, uint32 ping) override;
	void GetCurrentServers(char*** pastrServers, int& o_amount) override;
	void GetServer(int number, char** server, char** data, int& ping) override;
	const char* GetServerData(const char* server, int& o_ping) override;
	void RefreshPings() override;
	void OnReceiveGameState(const char*, XmlNodeRef) override;
	void Update() override;
	void Release() override;
	void ConnectToServer(const char* server) override;
	void GetValuesFromData(char *strData, SServerData *pServerData) override;
	void GetMemoryStatistics(ICrySizer* pSizer) override;

	////////////////////////////////////////////////////////////////////////////////
};
