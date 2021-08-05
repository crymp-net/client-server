#pragma once

#include <memory>

struct IScriptSystem;

class CScriptableBase;

class ScriptBindings
{
	std::unique_ptr<CScriptableBase> m_pBindSystem;
	std::unique_ptr<CScriptableBase> m_pBindParticle;
	std::unique_ptr<CScriptableBase> m_pBindSound;
	std::unique_ptr<CScriptableBase> m_pBindMovie;
	std::unique_ptr<CScriptableBase> m_pBindScript;
	std::unique_ptr<CScriptableBase> m_pBindPhysics;

public:
	void Init(IScriptSystem *pSS);
};
