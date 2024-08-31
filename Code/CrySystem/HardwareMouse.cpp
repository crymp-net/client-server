#include <algorithm>

#include "CryCommon/CryRenderer/IRenderer.h"
#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "Library/WinAPI.h"

#include "HardwareMouse.h"

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
	if (m_counter <= 0)
	{
		CryLogErrorAlways("%s: Too many decrements!", __FUNCTION__);
		return;
	}

	m_counter--;

	if (m_counter == 0)
	{
		this->ShowCursor(false);
	}
}

void HardwareMouse::GetHardwareMousePosition(float* x, float* y)
{
	long lx;
	long ly;
	WinAPI::Cursor::GetPos(lx, ly);

	*x = static_cast<float>(lx);
	*y = static_cast<float>(ly);
}

void HardwareMouse::SetHardwareMousePosition(float x, float y)
{
	WinAPI::Cursor::SetPos(static_cast<long>(x), static_cast<long>(y));
}

void HardwareMouse::GetHardwareMouseClientPosition(float* x, float* y)
{
	void* window = gEnv->pRenderer->GetHWND();
	if (!window)
	{
		*x = 0;
		*y = 0;
		return;
	}

	long lx;
	long ly;
	WinAPI::Cursor::GetPos(lx, ly);
	WinAPI::Window::ConvertPosToWindow(window, lx, ly);

	*x = static_cast<float>(lx);
	*y = static_cast<float>(ly);
}

void HardwareMouse::SetHardwareMouseClientPosition(float x, float y)
{
	void* window = gEnv->pRenderer->GetHWND();
	if (!window)
	{
		return;
	}

	long lx = static_cast<long>(x);
	long ly = static_cast<long>(y);
	WinAPI::Window::ConvertPosToScreen(window, lx, ly);
	WinAPI::Cursor::SetPos(lx, ly);
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
	m_cursorConfineDisabled = false;

	if (!visibleByDefault)
	{
		this->ConfineCursor(true);
	}
}

void HardwareMouse::ConfineCursor(bool confine)
{
	m_cursorConfined = confine;

	if (confine && m_cursorConfineDisabled)
	{
		return;
	}

	const bool isEditor = gEnv->pSystem->IsEditor();

	void* window = isEditor ? gEnv->pRenderer->GetCurrentContextHWND() : gEnv->pRenderer->GetHWND();
	if (!window)
	{
		return;
	}

	const bool hasFocus = WinAPI::Window::IsFocused(window);
	const bool isForeground = WinAPI::Window::IsForeground(window) || isEditor;

	if (confine && hasFocus && isForeground && !gEnv->pSystem->IsEditorMode())
	{
		WinAPI::Cursor::Clip(window);
	}
	else
	{
		WinAPI::Cursor::Clip(nullptr);
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

void HardwareMouse::DisableConfineCursor(bool disable)
{
	if (m_cursorConfineDisabled == disable)
	{
		return;
	}

	m_cursorConfineDisabled = disable;

	if (m_cursorConfined)
	{
		this->ConfineCursor(!disable);
	}
}

void HardwareMouse::ShowCursor(bool show)
{
	if (show)
	{
		this->SetHardwareMousePosition(m_positionX, m_positionY);
	}
	else
	{
		this->GetHardwareMousePosition(&m_positionX, &m_positionY);
	}

	WinAPI::Cursor::Show(show);

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
