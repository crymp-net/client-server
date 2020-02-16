/**
 * @file
 * @brief CryMP Client launcher.
 */

#pragma once

#include "CryCommon/ISystem.h"
#include "Library/DLL.h"

#include "Executor.h"

struct IGameFramework;

class Launcher : public ISystemUserCallback
{
	DLL m_libCrySystem;
	DLL m_libCryAction;
	DLL m_libCryNetwork;
	Executor m_executor;
	ISystem *m_pSystem;
	IGameFramework *m_pGameFramework;
	SSystemInitParams *m_pInitParams;

	static Launcher *s_pInstance;

public:
	Launcher()
	: m_libCrySystem(),
	  m_libCryAction(),
	  m_libCryNetwork(),
	  m_executor(),
	  m_pSystem(),
	  m_pGameFramework(),
	  m_pInitParams()
	{
		s_pInstance = this;
	}

	bool run(SSystemInitParams & params);

	// ISystemUserCallback
	bool OnError(const char *msg) override;
	void OnSaveDocument() override;
	void OnProcessSwitch() override;
	void OnInitProgress(const char *msg) override;
	void OnInit(ISystem *pSystem) override;
	void OnShutdown() override;
	void OnUpdate() override;
	void GetMemoryUsage(ICrySizer *pSizer) override;

	static const DLL & GetCrySystemDLL()
	{
		return s_pInstance->m_libCrySystem;
	}

	static const DLL & GetCryActionDLL()
	{
		return s_pInstance->m_libCryAction;
	}

	static const DLL & GetCryNetworkDLL()
	{
		return s_pInstance->m_libCryNetwork;
	}

	static Executor & GetExecutor()
	{
		return s_pInstance->m_executor;
	}

	static ISystem *GetISystem()
	{
		return s_pInstance->m_pSystem;
	}

	static IGameFramework *GetIGameFramework()
	{
		return s_pInstance->m_pGameFramework;
	}

	static SSystemInitParams *GetInitParams()
	{
		return s_pInstance->m_pInitParams;
	}
};
