#pragma once

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/DLL.h"

#include "GameWindow.h"
#include "Log.h"

class CGame;

class Launcher : public ISystemUserCallback
{
	DLL m_CryAction;
	DLL m_CryNetwork;
	DLL m_CrySystem;
	SSystemInitParams m_params;
	GameWindow m_gameWindow;
	CGame *m_pGame = nullptr;
	CLog m_log;

	void initWorkingDirectory();
	void loadEngine();
	void patchEngine();
	void startEngine();
	void updateLoop();

public:
	Launcher();
	~Launcher();

	// ISystemUserCallback
	bool OnError(const char *error) override;
	void OnSaveDocument() override;
	void OnProcessSwitch() override;
	void OnInitProgress(const char *message) override;
	void OnInit(ISystem *pSystem) override;
	void OnShutdown() override;
	void OnUpdate() override;
	void GetMemoryUsage(ICrySizer *pSizer) override;

	const DLL & getCryAction() const
	{
		return m_CryAction;
	}

	const DLL & getCryNetwork() const
	{
		return m_CryNetwork;
	}

	const DLL & getCrySystem() const
	{
		return m_CrySystem;
	}

	const SSystemInitParams & getParams() const
	{
		return m_params;
	}

	const GameWindow & getGameWindow() const
	{
		return m_gameWindow;
	}

	CGame *getGame()
	{
		return m_pGame;
	}

	CLog & getLog()
	{
		return m_log;
	}

	void setAppInstance(void *hInstance)
	{
		m_params.hInstance = hInstance;
	}

	void setLogFileName(const char *logFileName)
	{
		m_params.logFileName = logFileName;
	}

	void setCmdLine(const char *cmdLine);

	void run();
};

///////////////////////////
inline Launcher *gLauncher;
///////////////////////////
