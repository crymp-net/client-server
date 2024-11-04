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
	CryLogComment("%s(%d)", __FUNCTION__, static_cast<int>(gameMode));

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
	CryLogComment("%s: m_counter=%d", __FUNCTION__, m_counter);

	m_counter++;

	if (m_counter == 1)
	{
		this->ShowCursor(true);
	}
}

void HardwareMouse::DecrementCounter()
{
	CryLogComment("%s: m_counter=%d", __FUNCTION__, m_counter);

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
	CryLogComment("%s(%d)", __FUNCTION__, static_cast<int>(visibleByDefault));

	m_counter = visibleByDefault ? 1 : 0;
	this->GetHardwareMousePosition(&m_positionX, &m_positionY);
	m_incrementX = 0;
	m_incrementY = 0;
	m_acceleration = 1;

	if (!visibleByDefault)
	{
		this->ConfineCursor(true);
	}
}

void HardwareMouse::ConfineCursor(bool confine)
{
	CryLogComment("%s(%d)", __FUNCTION__, static_cast<int>(confine));

	const bool isEditor = gEnv->pSystem->IsEditor();

	void* window = isEditor ? gEnv->pRenderer->GetCurrentContextHWND() : gEnv->pRenderer->GetHWND();
	if (!window)
	{
		return;
	}

	if (confine && !gEnv->pSystem->IsEditorMode())
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
		const bool hasFocus = wParam != 0;

		CryLogComment("%s: ESYSTEM_EVENT_CHANGE_FOCUS %d", __FUNCTION__, static_cast<int>(hasFocus));

		if (hasFocus)
		{
			if (!gEnv->pSystem->IsEditorMode() && m_lostFocus)
			{
				m_lostFocus = false;
				this->DecrementCounter();
			}

			if (this->IsFullscreen())
			{
				this->ConfineCursor(true);
			}
		}
		else
		{
			if (!gEnv->pSystem->IsEditorMode() && !m_lostFocus)
			{
				m_lostFocus = true;
				this->IncrementCounter();
			}
		}
	}
	else if (event == ESYSTEM_EVENT_MOVE)
	{
		CryLogComment("%s: ESYSTEM_EVENT_MOVE", __FUNCTION__);

		if (this->IsFullscreen() || m_counter == 0)
		{
			this->ConfineCursor(true);
		}
	}
	else if (event == ESYSTEM_EVENT_RESIZE)
	{
		CryLogComment("%s: ESYSTEM_EVENT_RESIZE", __FUNCTION__);

		if (this->IsFullscreen() || m_counter == 0)
		{
			this->ConfineCursor(true);
		}
	}
	else if (event == ESYSTEM_EVENT_TOGGLE_FULLSCREEN)
	{
		const bool isFullscreen = wParam != 0;

		CryLogComment("%s: ESYSTEM_EVENT_TOGGLE_FULLSCREEN %d", __FUNCTION__, static_cast<int>(isFullscreen));

		if (isFullscreen || m_counter == 0)
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
	CryLogComment("%s(%d)", __FUNCTION__, static_cast<int>(show));

	if (show)
	{
		this->SetHardwareMousePosition(m_positionX, m_positionY);
	}
	else
	{
		this->GetHardwareMousePosition(&m_positionX, &m_positionY);
	}

	WinAPI::Cursor::Show(show);

	this->ConfineCursor(!show || this->IsFullscreen());

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
