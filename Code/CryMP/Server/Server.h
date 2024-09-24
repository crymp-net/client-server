#pragma once

#include <memory>

#include "CryCommon/CryAction/IGameFramework.h"

class Executor;
class HTTPClient;

class Server : public IGameFrameworkListener
{
public:
	IGame *pGame = nullptr;
	IGameFramework* pGameFramework = nullptr;

	std::unique_ptr<Executor> pExecutor;
	std::unique_ptr<HTTPClient> pHttpClient;

	Server();
	~Server();

	void Init(IGameFramework* pGameFramework);

	void UpdateLoop();

private:
	// IGameFrameworkListener
	void OnPostUpdate(float deltaTime) override;
	void OnSaveGame(ISaveGame* saveGame) override;
	void OnLoadGame(ILoadGame* loadGame) override;
	void OnLevelEnd(const char* nextLevel) override;
	void OnActionEvent(const SActionEvent& event) override;
};

///////////////////////
inline Server* gServer;
///////////////////////
