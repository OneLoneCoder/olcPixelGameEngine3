#include "host_lin_x11.h"

//! START IMPLEMENTATION
namespace olc::host
{
    Host_Linux_X11::Host_Linux_X11()
    {
        using namespace X11;

        libx11_handle = dlopen("libX11.so", RTLD_LAZY);
        if(libx11_handle)
        {
            ZERO_LOAD_SYM(libx11_handle, XCreateBitmapFromData)
            ZERO_LOAD_SYM(libx11_handle, XCreateColormap)
            ZERO_LOAD_SYM(libx11_handle, XCreatePixmapCursor)
            ZERO_LOAD_SYM(libx11_handle, XCreateWindow)
            ZERO_LOAD_SYM(libx11_handle, XDefineCursor)
            ZERO_LOAD_SYM(libx11_handle, XDestroyWindow)
            ZERO_LOAD_SYM(libx11_handle, XFlush)
            ZERO_LOAD_SYM(libx11_handle, XFree)
            ZERO_LOAD_SYM(libx11_handle, XFreeCursor)
            ZERO_LOAD_SYM(libx11_handle, XFreePixmap)
            ZERO_LOAD_SYM(libx11_handle, XGetAtomName)
            ZERO_LOAD_SYM(libx11_handle, XGetWindowAttributes)
            ZERO_LOAD_SYM(libx11_handle, XGrabPointer)
            ZERO_LOAD_SYM(libx11_handle, XInitThreads)
            ZERO_LOAD_SYM(libx11_handle, XInternAtom)
            ZERO_LOAD_SYM(libx11_handle, XLookupString)
            ZERO_LOAD_SYM(libx11_handle, XMapWindow)
            ZERO_LOAD_SYM(libx11_handle, XNextEvent)
            ZERO_LOAD_SYM(libx11_handle, XOpenDisplay)
            ZERO_LOAD_SYM(libx11_handle, XPending)
            ZERO_LOAD_SYM(libx11_handle, XSetWMProtocols)
            ZERO_LOAD_SYM(libx11_handle, XStoreName)
            ZERO_LOAD_SYM(libx11_handle, XUndefineCursor)
            ZERO_LOAD_SYM(libx11_handle, XUngrabPointer)
            ZERO_LOAD_SYM(libx11_handle, XWarpPointer)
            ZERO_LOAD_SYM(libx11_handle, XkbFreeKeyboard)
            ZERO_LOAD_SYM(libx11_handle, XkbGetMap)
            ZERO_LOAD_SYM(libx11_handle, XkbGetNames)
            ZERO_LOAD_SYM(libx11_handle, XkbGetState)
            ZERO_LOAD_SYM(libx11_handle, XkbQueryExtension)
            ZERO_LOAD_SYM(libx11_handle, XkbSelectEventDetails)
        }
        else throw std::runtime_error("Failed to load libX11.so");
        
        libglx_handle = dlopen("libGLX.so", RTLD_LAZY);
        if(libglx_handle)
        {
            ZERO_LOAD_SYM(libglx_handle, glXChooseVisual)
            ZERO_LOAD_SYM(libglx_handle, glXCreateContext)
            ZERO_LOAD_SYM(libglx_handle, glXDestroyContext)
            ZERO_LOAD_SYM(libglx_handle, glXGetProcAddress)
            ZERO_LOAD_SYM(libglx_handle, glXMakeCurrent)
            ZERO_LOAD_SYM(libglx_handle, glXSwapBuffers)
        }
        else throw std::runtime_error("Failed to load libGLX.so");
        
        libgl_handle = dlopen("libGL.so", RTLD_LAZY);
        if(libgl_handle)
        {
            ZERO_LOAD_SYM(libgl_handle, glGenTextures)
            ZERO_LOAD_SYM(libgl_handle, glBindTexture)
            ZERO_LOAD_SYM(libgl_handle, glTexParameteri)
            ZERO_LOAD_SYM(libgl_handle, glTexEnvf)
            ZERO_LOAD_SYM(libgl_handle, glDeleteTextures)
            ZERO_LOAD_SYM(libgl_handle, glTexImage2D)
            ZERO_LOAD_SYM(libgl_handle, glClear)
            ZERO_LOAD_SYM(libgl_handle, glViewport)
            ZERO_LOAD_SYM(libgl_handle, glClearColor)
            ZERO_LOAD_SYM(libgl_handle, glReadPixels)
            ZERO_LOAD_SYM(libgl_handle, glCullFace)
            ZERO_LOAD_SYM(libgl_handle, glEnable)
            ZERO_LOAD_SYM(libgl_handle, glDisable)
            ZERO_LOAD_SYM(libgl_handle, glDrawArrays)
            ZERO_LOAD_SYM(libgl_handle, glBlendFunc)
            ZERO_LOAD_SYM(libgl_handle, glDepthFunc)
            ZERO_LOAD_SYM(libgl_handle, glGetTexImage)
            ZERO_LOAD_SYM(libgl_handle, glHint)
            ZERO_LOAD_SYM(libgl_handle, glPolygonMode)
            ZERO_LOAD_SYM(libgl_handle, glFrontFace)
            ZERO_LOAD_SYM(libgl_handle, glFinish)
        }
        else throw std::runtime_error("Failed to load libGLX.so");

        zero_XInitThreads();
        olc_Display = zero_XOpenDisplay(NULL);
        olc_WindowRoot = DefaultRootWindow(olc_Display);

        if(zero_XkbQueryExtension(olc_Display, nullptr, &xkbEventBase, &xkbErrorBase, nullptr, nullptr))
        {
            zero_XkbSelectEventDetails(olc_Display, XkbUseCoreKbd, XkbStateNotify, XkbGroupStateMask, XkbGroupStateMask);
            kbExtensionsFound = true;
            UpdateKeyboardLayout();
        }

        mapKeys[NoSymbol] = Key::NONE;

        int keyTracker = static_cast<int>(Key::A);
        uint32_t uppercase = static_cast<uint32_t>(XK_A);
        uint32_t lowercase = static_cast<uint32_t>(XK_a);

        for (; uppercase <= static_cast<uint32_t>(XK_Z); ++uppercase, ++lowercase)
        {
            mapKeys[uppercase] = (Key)keyTracker;
            mapKeys[lowercase] = (Key)keyTracker;
            ++keyTracker;
        }

        mapKeys[XK_F1] = Key::F1; mapKeys[XK_F2] = Key::F2; mapKeys[XK_F3] = Key::F3; mapKeys[XK_F4] = Key::F4;
        mapKeys[XK_F5] = Key::F5; mapKeys[XK_F6] = Key::F6; mapKeys[XK_F7] = Key::F7; mapKeys[XK_F8] = Key::F8;
        mapKeys[XK_F9] = Key::F9; mapKeys[XK_F10] = Key::F10; mapKeys[XK_F11] = Key::F11; mapKeys[XK_F12] = Key::F12;

        mapKeys[XK_Down] = Key::DOWN; mapKeys[XK_Left] = Key::LEFT; mapKeys[XK_Right] = Key::RIGHT; mapKeys[XK_Up] = Key::UP;
        mapKeys[XK_KP_Enter] = Key::ENTER; mapKeys[XK_Return] = Key::ENTER;

        mapKeys[XK_BackSpace] = Key::BACK; mapKeys[XK_Escape] = Key::ESCAPE; mapKeys[XK_Linefeed] = Key::ENTER;	mapKeys[XK_Pause] = Key::PAUSE;
        mapKeys[XK_Scroll_Lock] = Key::SCROLL; mapKeys[XK_Tab] = Key::TAB; mapKeys[XK_Delete] = Key::DEL; mapKeys[XK_Home] = Key::HOME;
        mapKeys[XK_End] = Key::END; mapKeys[XK_Page_Up] = Key::PGUP; mapKeys[XK_Page_Down] = Key::PGDN;	mapKeys[XK_Insert] = Key::INS;
        mapKeys[XK_Shift_L] = Key::SHIFT; mapKeys[XK_Shift_R] = Key::SHIFT; mapKeys[XK_Control_L] = Key::CTRL; mapKeys[XK_Control_R] = Key::CTRL;
        mapKeys[XK_space] = Key::SPACE; mapKeys[XK_period] = Key::PERIOD;

        mapKeys[XK_0] = Key::K0; mapKeys[XK_1] = Key::K1; mapKeys[XK_2] = Key::K2; mapKeys[XK_3] = Key::K3; mapKeys[XK_4] = Key::K4;
        mapKeys[XK_5] = Key::K5; mapKeys[XK_6] = Key::K6; mapKeys[XK_7] = Key::K7; mapKeys[XK_8] = Key::K8; mapKeys[XK_9] = Key::K9;

        mapKeys[XK_KP_0] = Key::NP0; mapKeys[XK_KP_1] = Key::NP1; mapKeys[XK_KP_2] = Key::NP2; mapKeys[XK_KP_3] = Key::NP3; mapKeys[XK_KP_4] = Key::NP4;
        mapKeys[XK_KP_5] = Key::NP5; mapKeys[XK_KP_6] = Key::NP6; mapKeys[XK_KP_7] = Key::NP7; mapKeys[XK_KP_8] = Key::NP8; mapKeys[XK_KP_9] = Key::NP9;
        mapKeys[XK_KP_Multiply] = Key::NP_MUL; mapKeys[XK_KP_Add] = Key::NP_ADD; mapKeys[XK_KP_Divide] = Key::NP_DIV; mapKeys[XK_KP_Subtract] = Key::NP_SUB; mapKeys[XK_KP_Decimal] = Key::NP_DECIMAL;

        // These map the keypad when NUMLOCK is off
        mapKeys[XK_KP_Home] = Key::HOME; mapKeys[XK_KP_End] = Key::END; mapKeys[XK_KP_Up] = Key::UP;
        mapKeys[XK_KP_Down] = Key::DOWN; mapKeys[XK_KP_Left] = Key::LEFT; mapKeys[XK_KP_Right] = Key::RIGHT;
        mapKeys[XK_KP_Page_Up] = Key::PGUP; mapKeys[XK_KP_Page_Down] = Key::PGDN; mapKeys[XK_KP_Insert] = Key::INS;
        mapKeys[XK_KP_Delete] = Key::DEL;

        // These keys vary depending on the keyboard. I've included comments for US and UK keyboard layouts
        mapKeys[XK_semicolon] = Key::OEM_1;		// On US and UK keyboards this is the ';:' key
        mapKeys[XK_slash] = Key::OEM_2;			// On US and UK keyboards this is the '/?' key
        mapKeys[XK_asciitilde] = Key::OEM_3;	// On US keyboard this is the '~' key
        mapKeys[XK_grave] = Key::OEM_3;	// On US keyboard this is the '`' key
        mapKeys[XK_bracketleft] = Key::OEM_4;	// On US and UK keyboards this is the '[{' key
        mapKeys[XK_backslash] = Key::OEM_5;		// On US keyboard this is '\|' key.
        mapKeys[XK_bracketright] = Key::OEM_6;	// On US and UK keyboards this is the ']}' key
        mapKeys[XK_apostrophe] = Key::OEM_7;	// On US keyboard this is the single/double quote key. On UK, this is the single quote/@ symbol key
        mapKeys[XK_numbersign] = Key::OEM_8;	// miscellaneous characters. Varies by keyboard. I believe this to be the '#~' key on UK keyboards
        mapKeys[XK_equal] = Key::EQUALS;		// the '+' key on any keyboard
        mapKeys[XK_comma] = Key::COMMA;			// the comma key on any keyboard
        mapKeys[XK_minus] = Key::MINUS;			// the minus key on any keyboard			

        mapKeys[XK_Caps_Lock] = Key::CAPS_LOCK;

        mapMouseButtons[1] = 0; // left click
        mapMouseButtons[2] = 2; // middle click
        mapMouseButtons[3] = 1; // right click
        mapMouseButtons[8] = 3;
        mapMouseButtons[9] = 4;
    }
    
    Host_Linux_X11::~Host_Linux_X11()
    {
        if(libglx_handle) dlclose(libglx_handle);
        if(libx11_handle) dlclose(libx11_handle);
        if(libgl_handle) dlclose(libgl_handle);

        libglx_handle = nullptr;
        libx11_handle = nullptr;
        libgl_handle = nullptr;
    }

    bool Host_Linux_X11::OnApplicationStart(olc::PixelGameEngine* pPrimary)
    {
        pPrimaryPGE = pPrimary;
        return true;
    }

    bool Host_Linux_X11::StartSystem()
    {
        using namespace X11;

        pPrimaryPGE->OnPreContextStart();

		// Create system thread - handles gpu context
		std::thread threadSystem([this]()
			{
				// Notify start of system thread
				if (!this->OnSystemThreadStart())
				{
					// PGE->OnContextStart() failed, or user aborted OnUserCreate()
					return;
				}

				// Main system loop
				while (systemActive)
				{
					// Perform primary window update
					if (!this->OnSystemTick())
					{
						StopSystem();
					}
				}

				// Notify end of system thread
				if (!this->OnSystemThreadEnd())
				{
					// PGE->OnContextEnd() failed
					return;
				}
			});

        auto get_pge_window = [&](auto x11_window) -> olc::Window* {
            auto itr = mapX11Window2PTR.find(x11_window);
            if(itr != mapX11Window2PTR.end()) {
                return itr->second;
            }
            return nullptr;
        };

        X11::XEvent xev;
        while(systemActive){
            while (zero_XPending(olc_Display))
            {
                zero_XNextEvent(olc_Display, &xev);
                
                // If there's an update to the keyboard, update it's layout.
                if (xev.type == xkbEventBase + XkbEventCode && kbExtensionsFound)
                {
                    UpdateKeyboardLayout();
                }

                if (xev.type == Expose)
                {
                    //auto* expose_event = reinterpret_cast<XExposeEvent*>(&xev);
                    X11::XExposeEvent& e = xev.xexpose;
                    if(auto* pge_window = get_pge_window(e.window); pge_window) {
                        X11::XWindowAttributes gwa;
                        zero_XGetWindowAttributes(e.display, e.window, &gwa);
                        pge_window->olc_OnWindowSize(olc::vi2d{gwa.width, gwa.height});
                    }
                }
                else if (xev.type == ConfigureNotify)
                {
                    X11::XConfigureEvent& xce = xev.xconfigure;
                    if(auto* pge_window = get_pge_window(xce.window); pge_window) {
                        pge_window->olc_OnWindowSize(olc::vi2d{xce.width, xce.height});
                    }
                }
                else if (xev.type == KeyPress)
                {
                    KeySym ks;

                    // Unset the "shift" bit so that Key and Shift-Key will be mapped to the same olc::key
                    // since the system kind of assumes this
                    xev.xkey.state &= ~(1); 

                    zero_XLookupString(&xev.xkey, NULL, 0, &ks, NULL);
                    
                    if(auto* pge_window = get_pge_window(xev.xkey.window); pge_window) {
                        auto it = mapKeys.find(static_cast<uint32_t>(ks));
                        if(it != mapKeys.end()) {
                            pge_window->olc_OnKeyPress(it->second, true);
                        }
                    }
                }
                else if (xev.type == KeyRelease)
                {
                    KeySym ks;

                    zero_XLookupString(&xev.xkey, NULL, 0, &ks, NULL);

                    if(auto* pge_window = get_pge_window(xev.xkey.window); pge_window) {
                        auto it = mapKeys.find(static_cast<uint32_t>(ks));
                        if(it != mapKeys.end()) {
                            pge_window->olc_OnKeyPress(it->second, false);
                        }
                    }
                }
                else if (xev.type == ButtonPress)
                {
                    if(auto* pge_window = get_pge_window(xev.xbutton.window); pge_window) {
                        auto it = mapMouseButtons.find(xev.xbutton.button);
                        if(it != mapMouseButtons.end())
                        {
                            pge_window->olc_OnMouseButton(mapMouseButtons[xev.xbutton.button], true);
                            continue; // Thank you. NEXT!!!
                        }
                        
                        // If we make it here, we may be dealing with scrolling buttons
                        switch (xev.xbutton.button)
                        {
                            case 4:	pge_window->olc_OnMouseWheel(120); break;
                            case 5:	pge_window->olc_OnMouseWheel(-120); break;
                            default: break;
                        }
                    }
                }
                else if (xev.type == ButtonRelease)
                {
                    if(auto* pge_window = get_pge_window(xev.xbutton.window); pge_window) {
                        auto it = mapMouseButtons.find(xev.xbutton.button);
                        if(it != mapMouseButtons.end())
                        {
                            pge_window->olc_OnMouseButton(mapMouseButtons[xev.xbutton.button], false);
                            continue; // Thank you. NEXT!!!
                        }
                    }
                }
                else if (xev.type == MotionNotify)
                {
                    X11::XMotionEvent& xme = xev.xmotion;
                    if(auto* pge_window = get_pge_window(xev.xbutton.window); pge_window) {
                        pge_window->olc_OnMouseMove(olc::vi2d{xme.x, xme.y});
                    
                    }
                }
                else if (xev.type == FocusIn)
                {
                	
                    if(auto* pge_window = get_pge_window(xev.xfocus.window); pge_window) {
                        pge_window->olc_OnFocus(true);
                    }
                }
                else if (xev.type == FocusOut)
                {
                	if(auto* pge_window = get_pge_window(xev.xfocus.window); pge_window) {
                        pge_window->olc_OnFocus(false);
                    }
                }
                else if (xev.type == ClientMessage)
                {
                    X11::XClientMessageEvent& xcme = xev.xclient;
                    if(auto* pge_window = get_pge_window(xcme.window); pge_window) {
                        pge_window->olc_OnWindowClose();
                    }
                }
            }
        }

        systemActive = false;
        if(threadSystem.joinable())
            threadSystem.join();
    
        return pPrimaryPGE->OnPostContextEnd();
    }

    bool Host_Linux_X11::StopSystem()
    {
        systemActive = false;
        return true;
    }

    bool Host_Linux_X11::OnSystemThreadStart()
    {
        return pPrimaryPGE->OnContextStart();
    }

    bool Host_Linux_X11::OnSystemTick()
    {
        return pPrimaryPGE->OnContextTick();;
    }

    bool Host_Linux_X11::OnSystemThreadEnd()
    {
        return pPrimaryPGE->OnContextEnd();
    }

    bool Host_Linux_X11::OnApplicationEnd()
    {
        return true;
    }

    bool Host_Linux_X11::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        // Based on the display capabilities, configure the appearance of the window
        // to do this namespacing, both x11 and glx have to be included in the x11 namespace
        GLint olc_GLAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, X11::None };
        olc_VisualInfo = zero_glXChooseVisual(olc_Display, 0, olc_GLAttribs);
        olc_ColourMap = zero_XCreateColormap(olc_Display, olc_WindowRoot, olc_VisualInfo->visual, AllocNone);
        olc_SetWindowAttribs.colormap = olc_ColourMap;

        // Register which events we are interested in receiving
        olc_SetWindowAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;

        // Create the window
        X11::Window olc_Window = zero_XCreateWindow(olc_Display, olc_WindowRoot, vWindowPos.x + 1920, vWindowPos.y,
            vWindowSize.x, vWindowSize.y,
            0, olc_VisualInfo->depth, InputOutput, olc_VisualInfo->visual,
            CWColormap | CWEventMask, &olc_SetWindowAttribs);

            
        X11::Atom wmDelete = zero_XInternAtom(olc_Display, "WM_DELETE_WINDOW", true);
        zero_XSetWMProtocols(olc_Display, olc_Window, &wmDelete, 1);
        
        zero_XMapWindow(olc_Display, olc_Window);
        zero_XStoreName(olc_Display, olc_Window, "OneLoneCoder.com - Pixel Game Engine");
        pWindow->SetWindowSize(vWindowSize);
            
        mapUID2X11Window.insert_or_assign(pWindow->GetUID(), olc_Window);
		mapX11Window2PTR.insert_or_assign(olc_Window, pWindow);

        // Create invisible cursor
        char data[1] = {0};
        X11::Pixmap blank = zero_XCreateBitmapFromData(olc_Display, olc_Window, data, 1, 1);
        X11::XColor dummy = {0};
        X11::Cursor cursor = zero_XCreatePixmapCursor(olc_Display, blank, blank, &dummy, &dummy, 0, 0);
        zero_XFreePixmap(olc_Display, blank);
        
        // Add invisible cursor for this window
        mapUID2X11Cursor.insert_or_assign(pWindow->GetUID(), cursor);
        
        return true;
    }

    bool Host_Linux_X11::CloseWindowFrame(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        const auto invisible_cursor = mapUID2X11Cursor.find(pWindow->GetUID());

        if (window_handle != mapUID2X11Window.end() && invisible_cursor != mapUID2X11Cursor.end()) {
            zero_XFreeCursor(olc_Display, invisible_cursor->second);
            zero_XDestroyWindow(olc_Display, window_handle->second);
            mapUID2X11Window.erase(window_handle);
        }
        return true;
    }
    
    bool Host_Linux_X11::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        if (window_handle != mapUID2X11Window.end()) {
            zero_XStoreName(olc_Display, window_handle->second, pWindow->GetWindowTitle().c_str());
        }
        return true;
    }

    std::vector<void*> Host_Linux_X11::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        if (window_handle != mapUID2X11Window.end()) {
            return {reinterpret_cast<void*>(window_handle->second),
                reinterpret_cast<void*>(olc_Display)
            };
        }
  		return {};
    }

    olc::KeyboardLayout Host_Linux_X11::GetKeyboardLayout() const {
        return keyboardLayout;
    }

    void Host_Linux_X11::UpdateKeyboardLayout()
    {
        using namespace X11;
        keyboardLayout = OLC_DEFAULT_KEYBOARD_LAYOUT;

        XkbStateRec state;
        if (zero_XkbGetState(olc_Display, XkbUseCoreKbd, &state) != Success)
        {
            return;
        }

        // state.group contains the currently active layout group
        unsigned int currentGroup = state.group;

        XkbDescPtr xkb = zero_XkbGetMap(olc_Display, 0, XkbUseCoreKbd);
        if (!xkb)
        {
            return;
        }

        zero_XkbGetNames(olc_Display, XkbGroupNamesMask, xkb);
    
        if (!xkb->names)
        {
            zero_XkbFreeKeyboard(xkb, 0, True);
            return;
        }

        Atom layoutAtom = xkb->names->groups[currentGroup];
        char* layoutName = layoutAtom ? zero_XGetAtomName(olc_Display, layoutAtom) : nullptr;
    
        if (layoutName)
        {
            std::string layout(layoutName);
            
            // Convert to lowercase for easier comparison
            std::transform(layout.begin(), layout.end(), layout.begin(), ::tolower);
        
            if (layout.find("us") != std::string::npos)
                keyboardLayout = olc::KeyboardLayout::QWERTY_US;
            else if (layout.find("gb") != std::string::npos || layout.find("uk") != std::string::npos)
                keyboardLayout = olc::KeyboardLayout::QWERTY_UK;
            else if (layout.find("de") != std::string::npos || layout.find("german") != std::string::npos)
                keyboardLayout = olc::KeyboardLayout::QWERTZ;
            else if (layout.find("fr") != std::string::npos || layout.find("french") != std::string::npos)
                keyboardLayout = olc::KeyboardLayout::AZERTY;
            
            zero_XFree(layoutName);
        }
    
        zero_XkbFreeKeyboard(xkb, 0, True);
    }

    // Wait for entire host desktop refresh (for smooooth vsync)
    bool Host_Linux_X11::SyncWithDesktopComposite()
    {
        return true;
    }

    bool Host_Linux_X11::SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos)
    {
        auto win = mapUID2X11Window.at(pWindow->GetUID());
        
        // NOTE: xwayland will only allow warping when we have an active grab on a
        //       hidden mouse cursor.

        zero_XGrabPointer(
            olc_Display, win, True,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
            GrabModeAsync, GrabModeAsync,
            win, X11::None, CurrentTime
        );

        zero_XWarpPointer(olc_Display, X11::None, win, 0, 0, 0, 0, vPos.x, vPos.y);
        zero_XFlush(olc_Display);

        zero_XUngrabPointer(olc_Display, CurrentTime);
        zero_XFlush(olc_Display);

        return true;
    }
    
    bool Host_Linux_X11::SetMouseVisible(olc::Window* pWindow, const bool bVisible)
    {
        // NOTE: works on X11 and Xwayland, but does not work correctly in WSL2
        
        // nothing to change, do nothing
        if(bMouseIsVisible == bVisible)
            return true;

        bMouseIsVisible = bVisible;
        
        auto win = mapUID2X11Window.at(pWindow->GetUID());
        auto cursor = mapUID2X11Cursor.at(pWindow->GetUID());
        
        if(bMouseIsVisible)
        {
            zero_XUndefineCursor(olc_Display, win);
            return true;
        }
        
        zero_XDefineCursor(olc_Display, win, cursor);
        return true;
    }


}
//! END IMPLEMENTATION
