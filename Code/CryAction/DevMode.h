#pragma once

#include "CryCommon/CryInput/IInput.h"

class DevMode : public IInputEventListener
{
	void* m_reserved = nullptr;

public:
	DevMode();

	////////////////////////////////////////////////////////////////////////////////
	// IInputEventListener
	////////////////////////////////////////////////////////////////////////////////

	bool OnInputEvent(const SInputEvent& event) override;
	bool OnInputEventUI(const SInputEvent& event) override;

	////////////////////////////////////////////////////////////////////////////////
};
