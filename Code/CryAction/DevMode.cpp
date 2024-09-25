#include <cstdint>

#include "CryCommon/CrySystem/gEnv.h"

#include "DevMode.h"

extern std::uintptr_t CRYACTION_BASE;

DevMode::DevMode()
{
	static std::uintptr_t vtable[2] = {
#ifdef BUILD_64BIT
		CRYACTION_BASE + 0x32bfd0, CRYACTION_BASE + 0x1a79c0,
#else
		CRYACTION_BASE + 0x249e80, CRYACTION_BASE + 0xf9740,
#endif
	};

	*reinterpret_cast<std::uintptr_t**>(this) = vtable;

	IInput* pInput = gEnv->pInput;
	if (pInput)
	{
		pInput->AddEventListener(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
// IInputEventListener
////////////////////////////////////////////////////////////////////////////////

bool DevMode::OnInputEvent(const SInputEvent& event)
{
	return false;
}

bool DevMode::OnInputEventUI(const SInputEvent& event)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
