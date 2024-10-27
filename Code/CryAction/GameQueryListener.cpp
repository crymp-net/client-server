#include <cstddef>
#include <cstdint>

#include "GameQueryListener.h"

extern std::uintptr_t CRYACTION_BASE;

GameQueryListener::GameQueryListener()
{
#ifdef BUILD_64BIT
	static_assert(sizeof(GameQueryListener) == 0x3f0);
	static_assert(offsetof(GameQueryListener, m_pNetListener) == 0x8);
#else
	static_assert(sizeof(GameQueryListener) == 0x1fc);
	static_assert(offsetof(GameQueryListener, m_pNetListener) == 0x4);
#endif

#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2BD820;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1DCB10;
#endif

	(this->*reinterpret_cast<void(GameQueryListener::*&)()>(ctor))();
}

void GameQueryListener::SetNetListener(INetQueryListener* pListener)
{
	m_pNetListener = pListener;
}

void GameQueryListener::Complete()
{
	m_pNetListener->DeleteNetQueryListener();
}

////////////////////////////////////////////////////////////////////////////////
// IGameQueryListener
////////////////////////////////////////////////////////////////////////////////

void GameQueryListener::AddServer(const char* description, const char* target, const char* additionalText, uint32 ping)
{
}

void GameQueryListener::RemoveServer(string address)
{
}

void GameQueryListener::AddPong(string address, uint32 ping)
{
}

void GameQueryListener::GetCurrentServers(char*** pastrServers, int& o_amount)
{
}

void GameQueryListener::GetServer(int number, char** server, char** data, int& ping)
{
}

const char* GameQueryListener::GetServerData(const char* server, int& o_ping)
{
	return nullptr;
}

void GameQueryListener::RefreshPings()
{
}

void GameQueryListener::OnReceiveGameState(const char*, XmlNodeRef)
{
}

void GameQueryListener::Update()
{
}

void GameQueryListener::Release()
{
}

void GameQueryListener::ConnectToServer(const char* server)
{
}

void GameQueryListener::GetValuesFromData(char *strData, SServerData *pServerData)
{
}

void GameQueryListener::GetMemoryStatistics(ICrySizer* pSizer)
{
}

////////////////////////////////////////////////////////////////////////////////
