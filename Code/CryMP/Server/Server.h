#pragma once

#include <memory>

#include "CryCommon/CryAction/IGameFramework.h"

class Executor;
class HTTPClient;

class Server : public IGameFrameworkListener
{
public:
	IGameFramework* gameFramework = nullptr;

	std::unique_ptr<Executor> executor;
	std::unique_ptr<HTTPClient> httpClient;

	Server();
	~Server();

	void Init(IGameFramework* gameFramework);

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
