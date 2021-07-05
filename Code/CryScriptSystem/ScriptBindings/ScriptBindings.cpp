#include "ScriptBindings.h"

#include "ScriptBind_System.h"
#include "ScriptBind_Particle.h"
#include "ScriptBind_Sound.h"
#include "ScriptBind_Movie.h"
#include "ScriptBind_Script.h"
#include "ScriptBind_Physics.h"

void ScriptBindings::Init(ISystem *pSystem, IScriptSystem *pSS)
{
	m_pBindSystem   = std::make_unique<ScriptBind_System>(pSystem, pSS);
	m_pBindParticle = std::make_unique<ScriptBind_Particle>(pSystem, pSS);
	m_pBindSound    = std::make_unique<ScriptBind_Sound>(pSystem, pSS);
	m_pBindMovie    = std::make_unique<ScriptBind_Movie>(pSystem, pSS);
	m_pBindScript   = std::make_unique<ScriptBind_Script>(pSystem, pSS);
	m_pBindPhysics  = std::make_unique<ScriptBind_Physics>(pSystem, pSS);
}
