// Copyright (C) 2001-2004 Crytek GmbH

#pragma once

// When double-click, the sequence is the following:
// - HARDWAREMOUSEEVENT_LBUTTONDOWN
// - HARDWAREMOUSEEVENT_LBUTTONUP
// - HARDWAREMOUSEEVENT_LBUTTONDOUBLECLICK
// - HARDWAREMOUSEEVENT_LBUTTONUP

enum EHARDWAREMOUSEEVENT
{
	HARDWAREMOUSEEVENT_MOVE,
	HARDWAREMOUSEEVENT_LBUTTONDOWN,
	HARDWAREMOUSEEVENT_LBUTTONUP,
	HARDWAREMOUSEEVENT_LBUTTONDOUBLECLICK,
};

struct IHardwareMouseEventListener
{
	virtual void OnHardwareMouseEvent(int x, int y, EHARDWAREMOUSEEVENT event) = 0;
};

struct IHardwareMouse
{
	virtual void Release() = 0;

	// We need to register after the creation of the device but before its init
	virtual void OnPreInitRenderer() = 0;

	// We need to register after the creation of input to emulate mouse
	virtual void OnPostInitInput() = 0;

	// Call that in your WndProc on WM_MOUSEMOVE, WM_LBUTTONDOWN and WM_LBUTTONUP
	virtual void Event(int x, int y, EHARDWAREMOUSEEVENT event) = 0;

	// Add/Remove whoever is interested by the mouse cursor
	virtual void AddListener(IHardwareMouseEventListener* pListener) = 0;
	virtual void RemoveListener(IHardwareMouseEventListener* pListener) = 0;

	// Called only in Editor when switching from editing to game mode
	virtual void SetGameMode(bool gameMode) = 0;

	// Increment when you want to show the cursor, decrement otherwise
	virtual void IncrementCounter() = 0;
	virtual void DecrementCounter() = 0;

	// Standard get/set functions, mainly for Gamepad emulation purpose
	virtual void GetHardwareMousePosition(float* x, float* y) = 0;
	virtual void SetHardwareMousePosition(float x, float y) = 0;

	// Same as above, but relative to upper-left corner to the client area of our app
	virtual void GetHardwareMouseClientPosition(float* x, float* y) = 0;
	virtual void SetHardwareMouseClientPosition(float x, float y) = 0;

	// I consider a call to that function as a workaround...
	virtual void Reset(bool visibleByDefault) = 0;
	virtual void ConfineCursor(bool confine) = 0;
};
