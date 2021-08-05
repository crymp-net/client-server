#include "ScriptBindings.h"
#include "ScriptBind_System.h"
#include "ScriptBind_Particle.h"
#include "ScriptBind_Sound.h"
#include "ScriptBind_Movie.h"
#include "ScriptBind_Script.h"
#include "ScriptBind_Physics.h"

void ScriptBindings::Init(IScriptSystem *pSS)
{
	m_pBindSystem   = std::make_unique<ScriptBind_System>(pSS);
	m_pBindParticle = std::make_unique<ScriptBind_Particle>(pSS);
	m_pBindSound    = std::make_unique<ScriptBind_Sound>(pSS);
	m_pBindMovie    = std::make_unique<ScriptBind_Movie>(pSS);
	m_pBindScript   = std::make_unique<ScriptBind_Script>(pSS);
	m_pBindPhysics  = std::make_unique<ScriptBind_Physics>(pSS);
}
