#pragma once

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/DLL.h"

class CGame;

class Launcher : public ISystemUserCallback
{
	DLL m_CryAction;
	DLL m_CryNetwork;
	DLL m_CrySystem;
	DLL m_CryRenderD3D10;
	SSystemInitParams m_params;
	CGame *m_pGame = nullptr;

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

	CGame *GetGame()
	{
		return m_pGame;
	}

	void Run();
};

///////////////////////////
inline Launcher *gLauncher;
///////////////////////////
