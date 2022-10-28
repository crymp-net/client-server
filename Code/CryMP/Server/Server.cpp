#include "CryMP/Common/Executor.h"
#include "CryMP/Common/HTTPClient.h"

#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

void Server::Init(IGameFramework* gameFramework)
{
	this->gameFramework = gameFramework;

	this->executor = std::make_unique<Executor>();
	this->httpClient = std::make_unique<HTTPClient>(*this->executor);

	this->gameFramework->RegisterListener(this, "crymp-server", FRAMEWORKLISTENERPRIORITY_DEFAULT);
}

void Server::OnPostUpdate(float deltaTime)
{
	this->executor->OnUpdate();
}

void Server::OnSaveGame(ISaveGame* saveGame)
{
}

void Server::OnLoadGame(ILoadGame* loadGame)
{
}

void Server::OnLevelEnd(const char* nextLevel)
{
}

void Server::OnActionEvent(const SActionEvent& event)
{
	switch (event.m_event)
	{
		case eAE_channelCreated:
		case eAE_channelDestroyed:
		case eAE_connectFailed:
		case eAE_connected:
		case eAE_disconnected:
		case eAE_clientDisconnected:
		case eAE_resetBegin:
		case eAE_resetEnd:
		case eAE_resetProgress:
		case eAE_preSaveGame:
		case eAE_postSaveGame:
		case eAE_inGame:
		case eAE_serverName:
		case eAE_serverIp:
		case eAE_earlyPreUpdate:
		{
			break;
		}
	}
}
