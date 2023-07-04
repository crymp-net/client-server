#include <cstdint>

#include "CryCommon/CrySystem/gEnv.h"

#include "DevMode.h"

DevMode::DevMode()
{
	static constinit std::uintptr_t vtable[2] = {
#ifdef BUILD_64BIT
		0x3082bfd0, 0x306a79c0,
#else
		0x30749e80, 0x305f9740,
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
