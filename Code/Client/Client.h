#pragma once

#include <memory>
#include <random>
#include <string_view>
#include <type_traits>

#include "CryCommon/IGameFramework.h"
#include "CryAction/ILevelSystem.h"
#include "CryCommon/IEntitySystem.h"

struct IConsoleCmdArgs;

class Executor;
class HTTPClient;
class GSMasterHook;
class ScriptCommands;
class ScriptCallbacks;
class ScriptBind_CPPAPI;

class Client : public IGameFrameworkListener, public ILevelSystemListener, public IEntitySystemSink
{
	IGameFramework *m_pGameFramework = nullptr;

	std::string_view m_scriptMain;
	std::string_view m_scriptGameRules;
	std::string_view m_scriptJSON;
	std::string_view m_scriptRPC;

	std::unique_ptr<Executor> m_pExecutor;
	std::unique_ptr<HTTPClient> m_pHTTPClient;
	std::unique_ptr<GSMasterHook> m_pGSMasterHook;
	std::unique_ptr<ScriptCommands> m_pScriptCommands;
	std::unique_ptr<ScriptCallbacks> m_pScriptCallbacks;
	std::unique_ptr<ScriptBind_CPPAPI> m_pScriptBind_CPPAPI;

	std::minstd_rand m_randomEngine;

	unsigned int m_contract = 0;
	std::string m_activePak = "";

	static void OnConnectCmd(IConsoleCmdArgs *pArgs);
	static void OnDisconnectCmd(IConsoleCmdArgs *pArgs);

public:
	Client();
	~Client();

	void init(IGameFramework *pGameFramework);

	// IGameFrameworkListener
	void OnPostUpdate(float deltaTime) override;
	void OnSaveGame(ISaveGame *pSaveGame) override;
	void OnLoadGame(ILoadGame *pLoadGame) override;
	void OnLevelEnd(const char *nextLevel) override;
	void OnActionEvent(const SActionEvent & event) override;

	// ILevelSystemListener
	void OnLevelNotFound(const char *levelName) override;
	void OnLoadingStart(ILevelInfo *pLevel) override;
	void OnLoadingComplete(ILevel *pLevel) override;
	void OnLoadingError(ILevelInfo *pLevel, const char *error) override;
	void OnLoadingProgress(ILevelInfo *pLevel, int progressAmount) override;

	// IEntitySystemSink
	bool OnBeforeSpawn(SEntitySpawnParams & params) override;
	void OnSpawn(IEntity *pEntity, SEntitySpawnParams & params) override;
	bool OnRemove(IEntity *pEntity) override;
	void OnEvent(IEntity *pEntity, SEntityEvent & event) override;

	IGameFramework *getGameFramework()
	{
		return m_pGameFramework;
	}

	Executor *getExecutor()
	{
		return m_pExecutor.get();
	}

	HTTPClient *getHTTPClient()
	{
		return m_pHTTPClient.get();
	}

	GSMasterHook *getGSMasterHook()
	{
		return m_pGSMasterHook.get();
	}

	ScriptCommands *getScriptCommands()
	{
		return m_pScriptCommands.get();
	}

	ScriptCallbacks *getScriptCallbacks()
	{
		return m_pScriptCallbacks.get();
	}

	ScriptBind_CPPAPI *getScriptBind_CPPAPI()
	{
		return m_pScriptBind_CPPAPI.get();
	}

	// ints
	template<class T>
	typename std::enable_if<std::is_integral<T>::value, T>::type getRandomNumber(T min, T max)
	{
		// uniform distribution is stateless
		return std::uniform_int_distribution<T>(min, max)(m_randomEngine);  // [min, max]
	}

	// floats
	template<class T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type getRandomNumber(T min, T max)
	{
		// uniform distribution is stateless
		return std::uniform_real_distribution<T>(min, max)(m_randomEngine);  // [min, max)
	}

	bool loadServerPak(const std::string& path);
	bool unloadServerPak();

	unsigned int getContract() const
	{
		return m_contract;
	}

	unsigned int nextContract()
	{
		return ++m_contract;
	}
};

///////////////////////
inline Client *gClient;
///////////////////////
