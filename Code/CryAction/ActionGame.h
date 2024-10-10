#pragma once

#include "CryCommon/CryCore/smartptr.h"

class ScriptRMI;

class ActionGame : public _reference_target_t
{
public:
	explicit ActionGame(ScriptRMI* pScriptRMI);
	~ActionGame();

	bool Update();
};

#ifdef BUILD_64BIT
//static_assert(sizeof(ActionGame) == 0xb70);
#else
//static_assert(sizeof(ActionGame) == 0x7dc);
#endif
