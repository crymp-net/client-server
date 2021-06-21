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

	void SetCmdLine();
	void InitWorkingDirectory();
	void LoadEngine();
	void PatchEngine();
	void StartEngine();
	void UpdateLoop();

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

	const DLL & GetCryAction() const
	{
		return m_CryAction;
	}

	const DLL & GetCryNetwork() const
	{
		return m_CryNetwork;
	}

	const DLL & GetCrySystem() const
	{
		return m_CrySystem;
	}

	const SSystemInitParams & GetParams() const
	{
		return m_params;
	}

	const GameWindow & GetGameWindow() const
	{
		return m_gameWindow;
	}

	CGame *GetGame()
	{
		return m_pGame;
	}

	CLog & GetLog()
	{
		return m_log;
	}

	void Run();
};

///////////////////////////
inline Launcher *gLauncher;
///////////////////////////
