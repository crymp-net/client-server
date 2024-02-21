#pragma once

#include <vector>

#include "CryCommon/CryInput/IInput.h"
#include "CryCommon/CrySystem/IHardwareMouse.h"
#include "CryCommon/CrySystem/ISystem.h"

class HardwareMouse : public IHardwareMouse, public ISystemEventListener, public IInputEventListener
{
	int m_counter = 0;              // +0x30
	float m_cursorX = 0;            // +0x34
	float m_cursorY = 0;            // +0x38
	float m_somethingX = 0;         // +0x3c
	float m_somethingY = 0;         // +0x40
	bool m_hasFocus = true;         // +0x44
	bool m_cursorReleased = false;  // +0x45

	std::vector<IHardwareMouseEventListener*> m_listeners;

	static HardwareMouse s_globalInstance;

public:
	// to be removed once we have our own CrySystem
	static HardwareMouse& GetInstance()
	{
		return s_globalInstance;
	}

	// and this too
	void Init();

	HardwareMouse();
	~HardwareMouse();

	////////////////////////////////////////////////////////////////////////////////
	// IHardwareMouse
	////////////////////////////////////////////////////////////////////////////////

	void Release() override;

	void OnPreInitRenderer() override;
	void OnPostInitInput() override;

	void Event(int x, int y, EHARDWAREMOUSEEVENT event) override;

	void AddListener(IHardwareMouseEventListener* listener) override;
	void RemoveListener(IHardwareMouseEventListener* listener) override;

	void SetGameMode(bool gameMode) override;

	void IncrementCounter() override;
	void DecrementCounter() override;

	void GetHardwareMousePosition(float* x, float* y) override;
	void SetHardwareMousePosition(float x, float y) override;

	void GetHardwareMouseClientPosition(float* x, float* y) override;
	void SetHardwareMouseClientPosition(float x, float y) override;

	void Reset(bool visibleByDefault) override;
	void ConfineCursor(bool confine) override;

	////////////////////////////////////////////////////////////////////////////////
	// ISystemEventListener
	////////////////////////////////////////////////////////////////////////////////

	void OnSystemEvent(ESystemEvent event, UINT_PTR wParam, UINT_PTR lParam) override;

	////////////////////////////////////////////////////////////////////////////////
	// IInputEventListener
	////////////////////////////////////////////////////////////////////////////////

	bool OnInputEvent(const SInputEvent& event) override;
	bool OnInputEventUI(const SInputEvent& event) override;

	////////////////////////////////////////////////////////////////////////////////

private:
	void ShowCursor(bool show);

	static bool IsFullscreen();
};
