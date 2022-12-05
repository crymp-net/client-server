#pragma once

#include "CryCommon/CrySystem/ISystem.h"

class CGame;

class Launcher : public ISystemUserCallback
{
public:
	struct DLLs
	{
		// do not try to unload these DLLs as it might crash
		void* pCryAction = nullptr;
		void* pCryNetwork = nullptr;
		void* pCrySystem = nullptr;
		void* pCryRenderD3D9 = nullptr;
		void* pCryRenderD3D10 = nullptr;
	};

private:
	DLLs m_dlls;
	SSystemInitParams m_params;

	void SetCmdLine();
	void InitWorkingDirectory();
	void LoadEngine();
	void PatchEngine();
	void StartEngine();

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

	DLLs& GetDLLs()
	{
		return m_dlls;
	}

	const SSystemInitParams & GetParams() const
	{
		return m_params;
	}

	void Run();
};

///////////////////////////
inline Launcher *gLauncher;
///////////////////////////
