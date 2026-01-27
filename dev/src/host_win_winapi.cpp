#include "host_win_winapi.h"

//! START IMPLEMENTATION
namespace olc::host
{
	// Forward Declaration
	static LRESULT CALLBACK WINAPI_EventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);



	

	// Static linkage to lpfnWndProc - the hWnd is tagged with meta-info to get
	// access to the actual host instance, which can more conveninetly process
	// the event across multiple window instances
	static LRESULT CALLBACK WINAPI_EventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// CreateWindowEx will fire a WM_CREATE event at the window, which we're
		// not interested in, and this will occur before we've populated our 
		// linkage maps. We can detect for this condition here which means
		// subsequent look-ups dont fail. NOTE: Do not assume WM_CREATE is the
		// first message that is sent, its just one that we know is reliably sent.
		if (uMsg == WM_CREATE)
		{
			// Associate the window's little blob of user memory with host iface
			auto cfg = ((CREATESTRUCT*)lParam)->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)cfg);
		}
		else
		{
			// If this returns a value, then the host iface should be responsible
			// for handling the message.
			auto host = (Host_Windows_WinAPI*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (host)
				return host->OnWindowEvent(hWnd, uMsg, wParam, lParam);
		}
		
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	std::wstring Host_Windows_WinAPI::ConvertS2W(std::string s)
	{
#ifdef __MINGW32__
		wchar_t* buffer = new wchar_t[s.length() + 1];
		mbstowcs(buffer, s.c_str(), s.length());
		buffer[s.length()] = L'\0';
#else
		int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
		wchar_t* buffer = new wchar_t[count];
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
#endif
		std::wstring w(buffer);
		delete[] buffer;
		return w;
	}

	Host_Windows_WinAPI::Host_Windows_WinAPI()
	{
		// Map Windows Defined VK_ Codes to olc::KeyCodes
		mapKeys[0x00] = Key::NONE;

		// Windows doesn't define A-Z
		mapKeys[0x41] = Key::A;
		mapKeys[0x42] = Key::B;
		mapKeys[0x43] = Key::C;
		mapKeys[0x44] = Key::D;
		mapKeys[0x45] = Key::E;
		mapKeys[0x46] = Key::F;
		mapKeys[0x47] = Key::G;
		mapKeys[0x48] = Key::H;
		mapKeys[0x49] = Key::I;
		mapKeys[0x4A] = Key::J;
		mapKeys[0x4B] = Key::K;
		mapKeys[0x4C] = Key::L;
		mapKeys[0x4D] = Key::M;
		mapKeys[0x4E] = Key::N;
		mapKeys[0x4F] = Key::O;
		mapKeys[0x50] = Key::P;
		mapKeys[0x51] = Key::Q;
		mapKeys[0x52] = Key::R;
		mapKeys[0x53] = Key::S;
		mapKeys[0x54] = Key::T;
		mapKeys[0x55] = Key::U;
		mapKeys[0x56] = Key::V;
		mapKeys[0x57] = Key::W;
		mapKeys[0x58] = Key::X;
		mapKeys[0x59] = Key::Y;
		mapKeys[0x5A] = Key::Z;

		// Windows doesnt define numeric keys
		mapKeys[0x30] = Key::K0;
		mapKeys[0x31] = Key::K1;
		mapKeys[0x32] = Key::K2;
		mapKeys[0x33] = Key::K3;
		mapKeys[0x34] = Key::K4;
		mapKeys[0x35] = Key::K5;
		mapKeys[0x36] = Key::K6;
		mapKeys[0x37] = Key::K7;
		mapKeys[0x38] = Key::K8;
		mapKeys[0x39] = Key::K9;

		// Function Keys
		mapKeys[VK_F1] = Key::F1;
		mapKeys[VK_F2] = Key::F2;
		mapKeys[VK_F3] = Key::F3;
		mapKeys[VK_F4] = Key::F4;
		mapKeys[VK_F5] = Key::F5;
		mapKeys[VK_F6] = Key::F6;
		mapKeys[VK_F7] = Key::F7;
		mapKeys[VK_F8] = Key::F8;
		mapKeys[VK_F9] = Key::F9;
		mapKeys[VK_F10] = Key::F10;
		mapKeys[VK_F11] = Key::F11;
		mapKeys[VK_F12] = Key::F12;

		// Arrow Keys
		mapKeys[VK_DOWN] = Key::DOWN;
		mapKeys[VK_LEFT] = Key::LEFT;
		mapKeys[VK_RIGHT] = Key::RIGHT;
		mapKeys[VK_UP] = Key::UP;

		// Other Keys
		mapKeys[VK_BACK] = Key::BACK;
		mapKeys[VK_ESCAPE] = Key::ESCAPE;
		mapKeys[VK_RETURN] = Key::ENTER;
		mapKeys[VK_PAUSE] = Key::PAUSE;
		mapKeys[VK_SCROLL] = Key::SCROLL;
		mapKeys[VK_TAB] = Key::TAB;
		mapKeys[VK_DELETE] = Key::DEL;
		mapKeys[VK_HOME] = Key::HOME;
		mapKeys[VK_END] = Key::END;
		mapKeys[VK_PRIOR] = Key::PGUP;
		mapKeys[VK_NEXT] = Key::PGDN;
		mapKeys[VK_INSERT] = Key::INS;
		mapKeys[VK_SHIFT] = Key::SHIFT;
		mapKeys[VK_CONTROL] = Key::CTRL;
		mapKeys[VK_SPACE] = Key::SPACE;
		mapKeys[VK_CAPITAL] = Key::CAPS_LOCK;

		// Numpad
		mapKeys[VK_NUMPAD0] = Key::NP0;
		mapKeys[VK_NUMPAD1] = Key::NP1;
		mapKeys[VK_NUMPAD2] = Key::NP2;
		mapKeys[VK_NUMPAD3] = Key::NP3;
		mapKeys[VK_NUMPAD4] = Key::NP4;
		mapKeys[VK_NUMPAD5] = Key::NP5;
		mapKeys[VK_NUMPAD6] = Key::NP6;
		mapKeys[VK_NUMPAD7] = Key::NP7;
		mapKeys[VK_NUMPAD8] = Key::NP8;
		mapKeys[VK_NUMPAD9] = Key::NP9;
		mapKeys[VK_MULTIPLY] = Key::NP_MUL;
		mapKeys[VK_ADD] = Key::NP_ADD;
		mapKeys[VK_DIVIDE] = Key::NP_DIV;
		mapKeys[VK_SUBTRACT] = Key::NP_SUB;
		mapKeys[VK_DECIMAL] = Key::NP_DECIMAL;

		// OEM Keys
		mapKeys[VK_OEM_1] = Key::OEM_1;			// On US and UK keyboards this is the ';:' key
		mapKeys[VK_OEM_2] = Key::OEM_2;			// On US and UK keyboards this is the '/?' key
		mapKeys[VK_OEM_3] = Key::OEM_3;			// On US keyboard this is the '~' key
		mapKeys[VK_OEM_4] = Key::OEM_4;			// On US and UK keyboards this is the '[{' key
		mapKeys[VK_OEM_5] = Key::OEM_5;			// On US keyboard this is '\|' key.
		mapKeys[VK_OEM_6] = Key::OEM_6;			// On US and UK keyboards this is the ']}' key
		mapKeys[VK_OEM_7] = Key::OEM_7;			// On US keyboard this is the single/double quote key. On UK, this is the single quote/@ symbol key
		mapKeys[VK_OEM_8] = Key::OEM_8;			// miscellaneous characters. Varies by keyboard
		mapKeys[VK_OEM_PLUS] = Key::EQUALS;		// the '+' key on any keyboard
		mapKeys[VK_OEM_COMMA] = Key::COMMA;		// the comma key on any keyboard
		mapKeys[VK_OEM_MINUS] = Key::MINUS;		// the minus key on any keyboard
		mapKeys[VK_OEM_PERIOD] = Key::PERIOD;	// the period key on any keyboard
	}

	// Windows app needs an event loop somewhere. This is blocking of course. This loop handles
	// all windows created for this host.
	bool Host_Windows_WinAPI::StartSystemEventLoop(bool bBlockIfPossible)
	{
		if (bBlockIfPossible)
		{
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return true;
	}

	bool Host_Windows_WinAPI::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
	{
		olc_IgnoreUnused(bFullScreen);

		// The user created olc::Window object is the SSoT for what a window
		// should look like, so get that sort of thing from there
		olc::vi2d vWinPos = vWindowPos;
		olc::vi2d vWinSize = vWindowSize;
		

		// Define WindowClass
		WNDCLASSEX wc = { 0 };		
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpfnWndProc = WINAPI_EventHandler;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;// sizeof(this); // For static meta-info
		wc.lpszMenuName = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszClassName = olcT("OLC_PIXEL_GAME_ENGINE3");
		RegisterClassEx(&wc);

		// Define window furniture
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

		olc::vi2d vTopLeft = vWindowPos;

		//// Handle Fullscreen
		//if (bFullScreen)
		//{
		//	dwExStyle = 0;
		//	dwStyle = WS_VISIBLE | WS_POPUP;
		//	HMONITOR hmon = MonitorFromWindow(olc_hWnd, MONITOR_DEFAULTTONEAREST);
		//	MONITORINFO mi = { sizeof(mi) };
		//	if (!GetMonitorInfo(hmon, &mi)) return olc::rcode::FAIL;
		//	vWindowSize = { mi.rcMonitor.right, mi.rcMonitor.bottom };
		//	vTopLeft.x = 0;
		//	vTopLeft.y = 0;
		//}

		// Keep client size as requested
		RECT rWndRect = { 0, 0, vWindowSize.x, vWindowSize.y };
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;

		// Create the actual OS window, return a handle
		HWND hWnd = CreateWindowEx(dwExStyle, olcT("OLC_PIXEL_GAME_ENGINE3"), olcT(""), dwStyle,
			vTopLeft.x, vTopLeft.y, width, height, NULL, NULL, GetModuleHandle(nullptr), this);

		// Update window size to match actual client area given. In situations where the window
		// is clamped to the desktop, the client area may be smaller than requested.
		RECT rClient;
		GetClientRect(hWnd, &rClient);
		pWindow->SetWindowSize({ rClient.right - rClient.left, rClient.bottom - rClient.top });

		LONG_PTR lp = GetWindowLongPtr(hWnd, GWL_STYLE);
		SetWindowLongPtr(hWnd, GWL_STYLE, lp | (WS_CAPTION | WS_SYSMENU | WS_POPUPWINDOW | WS_THICKFRAME));
		lp = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, lp | (WS_EX_WINDOWEDGE));

		//SetWindowPos(hWnd, NULL, vWinPos.x, vWinPos.y, width, height, SWP_SHOWWINDOW);
		//ShowWindow(hWnd, 1);
		//UpdateWindow(hWnd);

		// Now... awkwardly, the above has already fired off some window messages
		// and they arent necessarily in a consistent order. Whereas one might 
		// assume WM_CREATE would be the first, there are some others on more
		// modern systems. This is awkward because we havent yet associated the
		// source window with a long_ptr to this class, and therefore we can't
		// call the appropriate event handler.


		// Store the link bewteen host resource and window
		mapUID2HWND.insert_or_assign(pWindow->GetUID(), hWnd);
		mapHWND2PTR.insert_or_assign(hWnd, pWindow);

	


		//DragAcceptFiles(olc_hWnd, true);

		return true;
	}

	bool Host_Windows_WinAPI::CloseWindowFrame(olc::Window* pWindow)
	{
		DestroyWindow((HWND)GetHostWindowDescriptor(pWindow).front());
		return false;
	}

	bool Host_Windows_WinAPI::UpdateWindowFrameTitle(olc::Window* pWindow)
	{
#ifdef UNICODE
		SetWindowText(mapUID2HWND.at(pWindow->GetUID()), ConvertS2W(pWindow->GetWindowTitle()).c_str());
#else
		SetWindowText(mapUID2HWND.at(pWindow->GetUID()), pWindow->GetWindowTitle().c_str());
#endif
		return true;
	}

	std::vector<void*> Host_Windows_WinAPI::GetHostWindowDescriptor(olc::Window* pWindow)
	{
		return { mapUID2HWND[pWindow->GetUID()] };
	}

	bool Host_Windows_WinAPI::ConnectHostResourceToRenderer()
	{
		return false;
	}

	bool Host_Windows_WinAPI::SyncWithDesktopComposite()
	{
		return DwmFlush() == S_OK;
	}

	LRESULT Host_Windows_WinAPI::OnWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!mapHWND2PTR.contains(hWnd))
			return DefWindowProc(hWnd, uMsg, wParam, lParam);;

		// Get target olc::Window
		const auto& window = mapHWND2PTR.at(hWnd);

		// Many WinAPI events are literally ancient these days, so need some interpretation
		// to get to the useful data.

		switch (uMsg)
		{
		case WM_MOUSEMOVE: // Mouse has moved within a window
			{
				// Extract mouse X & Y
				uint16_t x = uint16_t(lParam & 0xFFFF); 
				uint16_t y = uint16_t((lParam >> 16) & 0xFFFF);
				int16_t ix = *(int16_t*)&x;   
				int16_t iy = *(int16_t*)&y;
				// Tell window new mouse location
				window->olc_OnMouseMove(olc::vi2d{ ix, iy });
				break;
			}
		

			//		case WM_MOVE:       vWinPos = olc::vi2d(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);  ptrPGE->olc_UpdateWindowPos(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);	return 0;
		case WM_SIZE:
			{				
				window->olc_OnWindowSize(olc::vi2d(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF));
				break;
			}

		case WM_MOUSEWHEEL:
			{
				window->olc_OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
				break;
			}
			//		case WM_MOUSELEAVE: ptrPGE->olc_UpdateMouseFocus(false);                                    return 0;
			//		case WM_SETFOCUS:	ptrPGE->olc_UpdateKeyFocus(true);                                       return 0;
			//		case WM_KILLFOCUS:	ptrPGE->olc_UpdateKeyFocus(false);                                      return 0;
			//		case WM_KEYDOWN:	ptrPGE->olc_UpdateKeyState(int32_t(wParam), true);                      return 0;
			//		case WM_KEYUP:		ptrPGE->olc_UpdateKeyState(int32_t(wParam), false);                     return 0;
			//		case WM_SYSKEYDOWN: ptrPGE->olc_UpdateKeyState(int32_t(wParam), true);						return 0;
			//		case WM_SYSKEYUP:	ptrPGE->olc_UpdateKeyState(int32_t(wParam), false);						return 0;

		case WM_KEYDOWN:
			{
				if (mapKeys.contains(int32_t(wParam)))
				{
					window->olc_OnKeyPress(mapKeys[int32_t(wParam)], true);
				}
				break;
			}

		case WM_KEYUP:
			{
				if (mapKeys.contains(int32_t(wParam)))
				{
					window->olc_OnKeyPress(mapKeys[int32_t(wParam)], false);
				}
				break;
			}

		case WM_LBUTTONDOWN:
			{
				window->olc_OnMouseButton(0, true);
				break;
			}
		case WM_LBUTTONUP:
			{
				window->olc_OnMouseButton(0, false);
				break;
			}
		case WM_RBUTTONDOWN:
			{
				window->olc_OnMouseButton(1, true);
				break;
			}
		case WM_RBUTTONUP:
			{
				window->olc_OnMouseButton(1, false);
				break;
			}
		case WM_MBUTTONDOWN:
			{
				window->olc_OnMouseButton(2, true);
				break;
			}
		case WM_MBUTTONUP:
			{
				window->olc_OnMouseButton(2, false);
				break;
			}
			//		case WM_DROPFILES:
			//		{
			//			// This is all eww...
			//			HDROP drop = (HDROP)wParam;
			//			
			//			uint32_t nFiles = DragQueryFile(drop, 0xFFFFFFFF, nullptr, 0);
			//			std::vector<std::string> vFiles;
			//			for (uint32_t i = 0; i < nFiles; i++)
			//			{
			//				TCHAR dfbuffer[256]{};
			//				uint32_t len = DragQueryFile(drop, i, nullptr, 0);
			//				DragQueryFile(drop, i, dfbuffer, 256);
			//#ifdef UNICODE
			//#ifdef __MINGW32__
			//				char* buffer = new char[len + 1];
			//				wcstombs(buffer, dfbuffer, len);
			//				buffer[len] = '\0';
			//#else
			//				int count = WideCharToMultiByte(CP_UTF8, 0, dfbuffer, -1, NULL, 0, NULL, NULL);
			//				char* buffer = new char[count];
			//				WideCharToMultiByte(CP_UTF8, 0, dfbuffer, -1, buffer, count, NULL, NULL);
			//#endif				
			//				vFiles.push_back(std::string(buffer));
			//				delete[] buffer;
			//#else
			//				vFiles.push_back(std::string(dfbuffer));
			//#endif
			//			}
			//			
			//			// Even more eww...
			//			POINT p; DragQueryPoint(drop, &p);
			//			ptrPGE->olc_DropFiles(p.x, p.y, vFiles);
			//			DragFinish(drop);
			//			return 0;
			//		}
			//		break;
			//			
			//			
		case WM_CLOSE:
			{
				window->olc_OnWindowClose();
				break;
				//return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
		case WM_DESTROY:	
			PostQuitMessage(0); 
			DestroyWindow(hWnd);

		}


		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	olc::KeyboardLayout Host_Windows_WinAPI::GetKeyboardLayout() const
	{
		HKL kbl = ::GetKeyboardLayout(0);
		size_t highWord = ((size_t)kbl >> 16) & 0xFFFF;

		if (highWord == 0x00000409) // US
			return olc::KeyboardLayout::QWERTY_US;
		else if(highWord == 0x00000809) // UK
			return olc::KeyboardLayout::QWERTY_UK;
		else if(highWord == 0x00000407) // DE
			return olc::KeyboardLayout::QWERTZ;
		else if(highWord == 0x0000040C) // FR
			return olc::KeyboardLayout::AZERTY;

		return OLC_DEFAULT_KEYBOARD_LAYOUT;
	}

};
//! END IMPLEMENTATION

