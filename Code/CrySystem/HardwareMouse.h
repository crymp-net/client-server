#pragma once

#include <vector>

#include "CryCommon/CryInput/IInput.h"
#include "CryCommon/CrySystem/IHardwareMouse.h"
#include "CryCommon/CrySystem/ISystem.h"

class HardwareMouse : public IHardwareMouse, public ISystemEventListener, public IInputEventListener
{
	int m_counter = 1;
	float m_positionX = 0;
	float m_positionY = 0;
	float m_incrementX = 0;
	float m_incrementY = 0;
	float m_acceleration = 1;
	bool m_hasFocus = true;
	bool m_cursorReleased = false;

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
