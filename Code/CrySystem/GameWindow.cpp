/**
 * @file
 * @brief Implementation of game window.
 * In Crysis the following code is in CryGame, but Crytek removed it from the Mod SDK. The code is mostly reverse engineered
 * and slightly optimized version of the original code. The actual window is created in renderer DLL using the window class
 * registered here.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "CryCommon/I3DEngine.h"  // required by IRenderer.h
#include "CryCommon/IRenderer.h"
#include "CryCommon/IHardwareMouse.h"
#include "CryCommon/IInput.h"
#include "CryGame/Game.h"
#include "CryGame/Menus/OptionsManager.h"
#include "Launcher/Resources.h"
#include "Launcher/Util.h"  // Util::ErrorBox

#include "GameWindow.h"

static LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// NOTE: original code contains many additional useless NULL checks (gEnv, gEnv->pSystem, ...)

	switch (msg)
	{
		case WM_MOVE:  // 0x3
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_MOVE, x, y);

			break;
		}
		case WM_SIZE:  // 0x5
		{
			int w = GET_X_LPARAM(lParam);
			int h = GET_Y_LPARAM(lParam);

			gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_RESIZE, w, h);

			break;
		}
		case WM_CLOSE:  // 0x10
		{
			gEnv->pSystem->Quit();

			return 0;
		}
		case WM_INPUTLANGCHANGE:  // 0x51
		{
			gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_LANGUAGE_CHANGE, wParam, lParam);

			break;
		}
		case WM_STYLECHANGED:  // 0x7D
		{
			gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, 1, 0);

			break;
		}
		case WM_CHAR:  // 0x102
		{
			if (gEnv->pInput)
			{
				SInputEvent event;
				event.modifiers = gEnv->pInput->GetModifiers();
				event.deviceId = eDI_Keyboard;
				event.state = eIS_UI;
				event.value = 1.0;

				char keyCode = static_cast<char>(wParam);
				wchar_t keyCodeWide[2] = {};

				MultiByteToWideChar(CP_ACP, 0, &keyCode, 1, keyCodeWide, 2);

				// this looks like a mistake
				event.timestamp = keyCodeWide[0];

				// MB_CUR_MAX should be here, but it's slow
				char keyName[4] = {};

				if (wctomb(keyName, static_cast<wchar_t>(wParam)) >= 0)
				{
					if (keyName[1] == 0 && static_cast<unsigned char>(keyName[0]) >= 32)
					{
						event.keyName = keyName;

						gEnv->pInput->PostInputEvent(event);
					}
				}
			}

			break;
		}
		case WM_SYSKEYDOWN:  // 0x104
		{
			switch (wParam)
			{
				case VK_RETURN:  // Alt + Enter
				{
					if (gEnv->pRenderer && gEnv->pRenderer->GetRenderType() != eRT_DX10)
					{
						if (ICVar *pFullscreenCVar = gEnv->pConsole->GetCVar("r_Fullscreen"))
						{
							int fullscreen = pFullscreenCVar->GetIVal();
							pFullscreenCVar->Set(static_cast<int>(fullscreen == 0));
						}
					}

					break;
				}
				case VK_F4:  // Alt + F4
				{
					break;
				}
				default:
				{
					return 0;
				}
			}

			break;
		}
		case WM_SYSCHAR:  // 0x106
		case WM_HOTKEY:  // 0x312
		{
			return 0;
		}
		case WM_MOUSEMOVE:  // 0x200
		{
			if (gEnv->pHardwareMouse)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				gEnv->pHardwareMouse->Event(x, y, HARDWAREMOUSEEVENT_MOVE);
			}

			return 0;
		}
		case WM_LBUTTONDOWN:  // 0x201
		{
			if (gEnv->pHardwareMouse)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				gEnv->pHardwareMouse->Event(x, y, HARDWAREMOUSEEVENT_LBUTTONDOWN);
			}

			return 0;
		}
		case WM_LBUTTONUP:  // 0x202
		{
			if (gEnv->pHardwareMouse)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				gEnv->pHardwareMouse->Event(x, y, HARDWAREMOUSEEVENT_LBUTTONUP);
			}

			return 0;
		}
		case WM_LBUTTONDBLCLK:  // 0x203
		{
			if (gEnv->pHardwareMouse)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				gEnv->pHardwareMouse->Event(x, y, HARDWAREMOUSEEVENT_LBUTTONDOUBLECLICK);
			}

			return 0;
		}
		case WM_ENTERMENULOOP:  // 0x211
		case WM_ENTERSIZEMOVE:  // 0x231
		{
			if (g_pGame)
			{
				g_pGame->ShowMousePointer(true);
			}

			return 0;
		}
		case WM_EXITMENULOOP:  // 0x212
		case WM_EXITSIZEMOVE:  // 0x232
		{
			if (g_pGame)
			{
				g_pGame->ShowMousePointer(false);
			}

			return 0;
		}
		case WM_MOUSEACTIVATE:
		{
			return MA_ACTIVATEANDEAT;
		}
	}

	return DefWindowProcA(window, msg, wParam, lParam);
}

GameWindow::GameWindow()
: m_classID(0)
{
}

GameWindow::~GameWindow()
{
	if (m_classID)
	{
		UnregisterClassA("CryENGINE", GetModuleHandleA(NULL));
	}
}

bool GameWindow::init()
{
	HMODULE exe = GetModuleHandleA(NULL);

	WNDCLASSA windowClass;
	windowClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = exe;
	windowClass.hIcon = LoadIconA(exe, MAKEINTRESOURCEA(LAUNCHER_ICON));
	windowClass.hCursor = LoadCursorA(exe, MAKEINTRESOURCEA(LAUNCHER_CURSOR_GREEN));
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "CryENGINE";

	m_classID = RegisterClassA(&windowClass);
	if (!m_classID)
	{
		Util::ErrorBox("Failed to register CryENGINE window class!");
		return false;
	}

	return true;
}

bool GameWindow::update()
{
	MSG msg;

	if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return true;
}

void GameWindow::onQuit()
{
	MSG msg;

	while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

void GameWindow::UpdateCursorColor()
{
	HMODULE exe = GetModuleHandleA(NULL);

	HCURSOR cursor = NULL;

	switch (g_pGame->GetOptions()->GetCrysisProfileColor())
	{
		case CrysisProfileColor_Amber:
		{
			cursor = LoadCursorA(exe, MAKEINTRESOURCEA(LAUNCHER_CURSOR_AMBER));
			break;
		}
		case CrysisProfileColor_Blue:
		{
			cursor = LoadCursorA(exe, MAKEINTRESOURCEA(LAUNCHER_CURSOR_BLUE));
			break;
		}
		case CrysisProfileColor_Green:
		{
			cursor = LoadCursorA(exe, MAKEINTRESOURCEA(LAUNCHER_CURSOR_GREEN));
			break;
		}
		case CrysisProfileColor_Red:
		{
			cursor = LoadCursorA(exe, MAKEINTRESOURCEA(LAUNCHER_CURSOR_RED));
			break;
		}
		case CrysisProfileColor_White:
		{
			cursor = LoadCursorA(exe, MAKEINTRESOURCEA(LAUNCHER_CURSOR_WHITE));
			break;
		}
		default:
		{
			// default green cursor
			cursor = LoadCursorA(exe, MAKEINTRESOURCEA(LAUNCHER_CURSOR_GREEN));
			break;
		}
	}

	if (cursor)
	{
		HWND window = static_cast<HWND>(gEnv->pRenderer->GetHWND());

		SetClassLongPtrA(window, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
		SetCursor(cursor);
	}
}
