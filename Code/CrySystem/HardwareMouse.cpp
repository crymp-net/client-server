#include <algorithm>

#include "CryCommon/CryRenderer/IRenderer.h"
#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ITimer.h"

#include "HardwareMouse.h"

// TODO: remove this evil include and use Library/WinAPI.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HardwareMouse HardwareMouse::s_globalInstance;

void HardwareMouse::Init()
{
	this->GetHardwareMousePosition(&m_positionX, &m_positionY);

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);
}


HardwareMouse::HardwareMouse()
{
}

HardwareMouse::~HardwareMouse()
{
	gEnv->pInput->RemoveEventListener(this);
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
}

////////////////////////////////////////////////////////////////////////////////
// IHardwareMouse
////////////////////////////////////////////////////////////////////////////////

void HardwareMouse::Release()
{
}

void HardwareMouse::OnPreInitRenderer()
{
}

void HardwareMouse::OnPostInitInput()
{
	gEnv->pInput->AddEventListener(this);
}

void HardwareMouse::Event(int x, int y, EHARDWAREMOUSEEVENT event)
{
	if (gEnv->pConsole->GetStatus())
	{
		return;
	}

	for (IHardwareMouseEventListener* listener : m_listeners)
	{
		listener->OnHardwareMouseEvent(x, y, event);
	}
}

void HardwareMouse::AddListener(IHardwareMouseEventListener* listener)
{
	if (listener && std::ranges::count(m_listeners, listener) == 0)
	{
		m_listeners.emplace_back(listener);
	}
}

void HardwareMouse::RemoveListener(IHardwareMouseEventListener* listener)
{
	std::erase(m_listeners, listener);
}

void HardwareMouse::SetGameMode(bool gameMode)
{
	if (gameMode)
	{
		this->DecrementCounter();
	}
	else
	{
		this->IncrementCounter();
	}
}

void HardwareMouse::IncrementCounter()
{
	m_counter++;

	if (m_counter == 1)
	{
		this->ShowCursor(true);
	}
}

void HardwareMouse::DecrementCounter()
{
	m_counter--;

	if (m_counter == 0)
	{
		this->ShowCursor(false);
	}
}

void HardwareMouse::GetHardwareMousePosition(float* x, float* y)
{
	POINT point;
	::GetCursorPos(&point);

	*x = static_cast<float>(point.x);
	*y = static_cast<float>(point.y);
}

void HardwareMouse::SetHardwareMousePosition(float x, float y)
{
	::SetCursorPos(static_cast<int>(x), static_cast<int>(y));
}

void HardwareMouse::GetHardwareMouseClientPosition(float* x, float* y)
{
	HWND window = static_cast<HWND>(gEnv->pRenderer->GetHWND());
	if (!window)
	{
		*x = 0;
		*y = 0;
		return;
	}

	POINT point;
	::GetCursorPos(&point);
	::ScreenToClient(window, &point);

	*x = static_cast<float>(point.x);
	*y = static_cast<float>(point.y);
}

void HardwareMouse::SetHardwareMouseClientPosition(float x, float y)
{
	HWND window = static_cast<HWND>(gEnv->pRenderer->GetHWND());
	if (!window)
	{
		return;
	}

	POINT point;
	point.x = static_cast<long>(x);
	point.y = static_cast<long>(y);
	::ClientToScreen(window, &point);
	::SetCursorPos(point.x, point.y);
}

void HardwareMouse::Reset(bool visibleByDefault)
{
	m_counter = visibleByDefault ? 1 : 0;
	this->GetHardwareMousePosition(&m_positionX, &m_positionY);
	m_incrementX = 0;
	m_incrementY = 0;
	m_acceleration = 1;
	m_hasFocus = true;
	m_cursorReleased = false;

	if (!visibleByDefault)
	{
		this->ConfineCursor(true);
	}
}

void HardwareMouse::ConfineCursor(bool confine)
{
	const bool isEditor = gEnv->pSystem->IsEditor();

	HWND window = static_cast<HWND>(isEditor ? gEnv->pRenderer->GetCurrentContextHWND() : gEnv->pRenderer->GetHWND());
	if (!window)
	{
		return;
	}

	const bool hasFocus = ::GetFocus() == window;
	const bool isForeground = ::GetForegroundWindow() == window || isEditor;

	if (confine && hasFocus && isForeground && !gEnv->pSystem->IsEditorMode())
	{
		RECT rect;
		::GetClientRect(window, &rect);
		::ClientToScreen(window, reinterpret_cast<POINT*>(&rect.left));
		::ClientToScreen(window, reinterpret_cast<POINT*>(&rect.right));
		::ClipCursor(&rect);
	}
	else
	{
		::ClipCursor(nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////
// ISystemEventListener
////////////////////////////////////////////////////////////////////////////////

void HardwareMouse::OnSystemEvent(ESystemEvent event, UINT_PTR wParam, UINT_PTR lParam)
{
	if (event == ESYSTEM_EVENT_CHANGE_FOCUS)
	{
		m_hasFocus = wParam != 0;

		if (m_hasFocus)
		{
			if (!gEnv->pSystem->IsEditorMode() && m_cursorReleased)
			{
				m_cursorReleased = false;
				this->DecrementCounter();
			}

			if (this->IsFullscreen() || !gEnv->pSystem->IsDevMode())
			{
				this->ConfineCursor(true);
			}
		}
		else if (!gEnv->pSystem->IsEditorMode())
		{
			if (m_counter == 0)
			{
				m_cursorReleased = true;
				this->IncrementCounter();
			}

			this->ConfineCursor(false);
		}
	}
	else if (event == ESYSTEM_EVENT_MOVE || event == ESYSTEM_EVENT_RESIZE)
	{
		if (this->IsFullscreen() || m_counter == 0)
		{
			this->ConfineCursor(true);
		}
	}
	else if (event == ESYSTEM_EVENT_TOGGLE_FULLSCREEN)
	{
		const bool fullscreen = wParam != 0;

		if (fullscreen || m_counter == 0)
		{
			this->ConfineCursor(true);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// IInputEventListener
////////////////////////////////////////////////////////////////////////////////

bool HardwareMouse::OnInputEvent(const SInputEvent& event)
{
	if (m_counter == 0)
	{
		return false;
	}

	if (event.deviceId == eDI_XI)
	{
		if (_stricmp(event.keyName, "xi_thumbrx") == 0)
		{
			m_incrementX = event.value;
		}
		else if (_stricmp(event.keyName, "xi_thumbry") == 0)
		{
			m_incrementY = -event.value;
		}
	}
	else if (event.keyId == eKI_SYS_Commit)
	{
		if (-0.3 <= m_incrementX && m_incrementX <= 0.3 && -0.3 <= m_incrementY && m_incrementY <= 0.3)
		{
			this->GetHardwareMousePosition(&m_positionX, &m_positionY);
			m_acceleration = 1;
		}
		else
		{
			const float frameTime = gEnv->pTimer->GetFrameTime(ITimer::ETIMER_UI);

			if (m_acceleration < 10)
			{
				m_acceleration += frameTime * 5;
			}

			m_positionX += m_incrementX * m_acceleration * frameTime * 100.0;
			m_positionY += m_incrementY * m_acceleration * frameTime * 100.0;

			this->SetHardwareMousePosition(m_positionX, m_positionY);
		}
	}

	return false;
}

bool HardwareMouse::OnInputEventUI(const SInputEvent& event)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void HardwareMouse::ShowCursor(bool show)
{
	if (show)
	{
		this->SetHardwareMousePosition(m_positionX, m_positionY);

		::ShowCursor(TRUE);
	}
	else
	{
		this->GetHardwareMousePosition(&m_positionX, &m_positionY);

		::ShowCursor(FALSE);
	}

	this->ConfineCursor(!show || this->IsFullscreen() || !gEnv->pSystem->IsDevMode());

	gEnv->pInput->SetExclusiveMode(eDI_Mouse, false);
}

bool HardwareMouse::IsFullscreen()
{
	ICVar* fullscreen = gEnv->pConsole->GetCVar("r_Fullscreen");
	if (!fullscreen)
	{
		return false;
	}

	return fullscreen->GetIVal() != 0;
}
