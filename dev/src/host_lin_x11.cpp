#include "host_lin_x11.h"

//! START IMPLEMENTATION
namespace olc::host
{
    Host_Linux_X11::Host_Linux_X11()
    {
        using namespace X11;
        XInitThreads();
        olc_Display = XOpenDisplay(NULL);
        olc_WindowRoot = DefaultRootWindow(olc_Display);

        // If supported, enable receiving touch events
        enableTouch(olc_Display, olc_WindowRoot);

        if(XkbQueryExtension(olc_Display, nullptr, &xkbEventBase, &xkbErrorBase, nullptr, nullptr))
        {
            XkbSelectEventDetails(olc_Display, XkbUseCoreKbd, XkbStateNotify, XkbGroupStateMask, XkbGroupStateMask);
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
        mapKeys[XK_Alt_L] = Key::ALT; mapKeys[XK_Alt_R] = Key::ALT;
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

        // Create a pollfd that we will use later to wait for events to appear on the queue
        // preventing the event loop from becoming a busy loop
        pollfd x11_connection_fd {.fd = ConnectionNumber(olc_Display), .events = POLLIN};

        X11::XEvent xev;
        while(systemActive){
            while (XPending(olc_Display))
            {
                XNextEvent(olc_Display, &xev);
                
                // If there's an update to the keyboard, update it's layout.
                if (xev.type == xkbEventBase + XkbEventCode && kbExtensionsFound)
                {
                    UpdateKeyboardLayout();
                }

                if (xev.type == Expose)
                {
                    X11::XExposeEvent& e = xev.xexpose;
                    X11::Atom wm_state;
                    wm_state = X11::XInternAtom(olc_Display, "_NET_WM_STATE", True);
                    bool did_fullscreen = false;

                    X11::Atom actual_type;
                    int actual_format;
                    unsigned long int num_items;
                    unsigned long int bytes;
                    unsigned char* property{nullptr};
                    int res = X11::XGetWindowProperty(e.display, e.window, wm_state, 
                        0, 
                        ~0, 
                        False,
                        AnyPropertyType,
                        &actual_type,
                        &actual_format,
                        &num_items,
                        &bytes,
                        &property
                    );

                    if(res == Success) {
                        char* name;
                        if(X11::XGetAtomNames(e.display, (Atom*)property, num_items, &name))
                        {
                            for(int i = 0; i < num_items; i++)
                            {
                                if(std::strcmp(name + i, "_NET_WM_STATE_FULLSCREEN") == 0) {
                                    did_fullscreen = true;
                                }
                            }
                        }                        
                    }

                    if(auto* pge_window = get_pge_window(e.window); pge_window) {
                        if(did_fullscreen && (!pge_window->config.bFullScreenable || !pge_window->config.bResizeable) && !pge_window->bWindowIsFullscreen) {
                            SetFullScreen(pge_window, false);
                        } else {
                            X11::XWindowAttributes gwa;
                            X11::XGetWindowAttributes(e.display, e.window, &gwa);
                            pge_window->olc_OnWindowSize(olc::vi2d{gwa.width, gwa.height});
                            pge_window->bWindowIsFullscreen = did_fullscreen;
                        }
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

                    XLookupString(&xev.xkey, NULL, 0, &ks, NULL);
                    
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

                    XLookupString(&xev.xkey, NULL, 0, &ks, NULL);

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
                else if (xev.type == GenericEvent)
                {
                    X11::XGenericEventCookie& xgec = xev.xcookie;

                    if(xgec.extension == xinput_extension_code && X11::XGetEventData(olc_Display, &xgec)) {
                        // Should be one of our touch events
                        handleTouchEvent(static_cast<X11::XIDeviceEvent*>(xgec.data));
                        X11::XFreeEventData(olc_Display, &xgec);
                    }
                }
            }

            // Wait until an event appears on the x11 event queue file descriptor
            poll(&x11_connection_fd, 1, -1);
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
        olc_VisualInfo = glXChooseVisual(olc_Display, 0, olc_GLAttribs);
        olc_ColourMap = XCreateColormap(olc_Display, olc_WindowRoot, olc_VisualInfo->visual, AllocNone);
        olc_SetWindowAttribs.colormap = olc_ColourMap;

        // Register which events we are interested in receiving
        olc_SetWindowAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;

        // Create the window
        X11::Window olc_Window = XCreateWindow(olc_Display, olc_WindowRoot, vWindowPos.x, vWindowPos.y,
            vWindowSize.x, vWindowSize.y,
            0, olc_VisualInfo->depth, InputOutput, olc_VisualInfo->visual,
            CWColormap | CWEventMask, &olc_SetWindowAttribs);

            
        X11::Atom wmDelete = XInternAtom(olc_Display, "WM_DELETE_WINDOW", true);
        X11::XSetWMProtocols(olc_Display, olc_Window, &wmDelete, 1);
        if(!pWindow->config.bResizeable)
        {
            X11::XSizeHints size_hints;
            size_hints.min_width = vWindowSize.x;
            size_hints.max_width = vWindowSize.x;
            size_hints.min_height = vWindowSize.y;
            size_hints.max_height = vWindowSize.y;
            size_hints.flags = PMinSize | PMaxSize;
            X11::XSetNormalHints(olc_Display, olc_Window, &size_hints);
        }
        
        XMapWindow(olc_Display, olc_Window);
        XStoreName(olc_Display, olc_Window, "OneLoneCoder.com - Pixel Game Engine");
        pWindow->SetWindowSize(vWindowSize);
            
        mapUID2X11Window.insert_or_assign(pWindow->GetUID(), olc_Window);
		mapX11Window2PTR.insert_or_assign(olc_Window, pWindow);

        // Create invisible cursor
        char data[1] = {0};
        X11::Pixmap blank = XCreateBitmapFromData(olc_Display, olc_Window, data, 1, 1);
        X11::XColor dummy = {0};
        X11::Cursor cursor = XCreatePixmapCursor(olc_Display, blank, blank, &dummy, &dummy, 0, 0);
        XFreePixmap(olc_Display, blank);
        
        // Add invisible cursor for this window
        mapUID2X11Cursor.insert_or_assign(pWindow->GetUID(), cursor);
        
        return true;
    }

    bool Host_Linux_X11::CloseWindowFrame(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        const auto invisible_cursor = mapUID2X11Cursor.find(pWindow->GetUID());

        if (window_handle != mapUID2X11Window.end() && invisible_cursor != mapUID2X11Cursor.end()) {
            X11::XFreeCursor(olc_Display, invisible_cursor->second);
            X11::XDestroyWindow(olc_Display, window_handle->second);
            mapUID2X11Window.erase(window_handle);
        }
        return true;
    }
    
    bool Host_Linux_X11::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        if (window_handle != mapUID2X11Window.end()) {
            X11::XStoreName(olc_Display, window_handle->second, pWindow->GetWindowTitle().c_str());
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
        if (XkbGetState(olc_Display, XkbUseCoreKbd, &state) != Success)
        {
            return;
        }

        // state.group contains the currently active layout group
        unsigned int currentGroup = state.group;

        XkbDescPtr xkb = XkbGetMap(olc_Display, 0, XkbUseCoreKbd);
        if (!xkb)
        {
            return;
        }

        XkbGetNames(olc_Display, XkbGroupNamesMask, xkb);
    
        if (!xkb->names)
        {
            XkbFreeKeyboard(xkb, 0, True);
            return;
        }

        Atom layoutAtom = xkb->names->groups[currentGroup];
        char* layoutName = layoutAtom ? XGetAtomName(olc_Display, layoutAtom) : nullptr;
    
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
            
            XFree(layoutName);
        }
    
        XkbFreeKeyboard(xkb, 0, True);
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

        X11::XGrabPointer(
            olc_Display, win, True,
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
            GrabModeAsync, GrabModeAsync,
            win, X11::None, CurrentTime
        );

        X11::XWarpPointer(olc_Display, X11::None, win, 0, 0, 0, 0, vPos.x, vPos.y);
        X11::XFlush(olc_Display);

        X11::XUngrabPointer(olc_Display, CurrentTime);
        X11::XFlush(olc_Display);

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
            X11::XUndefineCursor(olc_Display, win);
            return true;
        }
        
        X11::XDefineCursor(olc_Display, win, cursor);
        return true;
    }

    bool Host_Linux_X11::SetFullScreen(olc::Window* pWindow, const bool bFullScreen)
    {
        using namespace X11;
        auto win = mapUID2X11Window.at(pWindow->GetUID());

        // If the bFullScreen flag is set, make the window fullscreen, otherwise restore it
        Atom wm_state;
        Atom fullscreen;
        wm_state = XInternAtom(olc_Display, "_NET_WM_STATE", False);
        fullscreen = XInternAtom(olc_Display, "_NET_WM_STATE_FULLSCREEN", False);
        XEvent xev{ 0 };
        xev.type = ClientMessage;
        xev.xclient.window = win;
        xev.xclient.message_type = wm_state;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = (bFullScreen ? 1 : 0);   // the action (0: off, 1: on, 2: toggle)
        xev.xclient.data.l[1] = fullscreen;             // first property to alter
        xev.xclient.data.l[2] = 0;                      // second property to alter
        xev.xclient.data.l[3] = 0;                      // source indication
        XMapWindow(olc_Display, win);
        XSendEvent(olc_Display, DefaultRootWindow(olc_Display), False,
            SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        XFlush(olc_Display);
        XWindowAttributes gwa;
        XGetWindowAttributes(olc_Display, win, &gwa);
        pWindow->olc_OnWindowSize({gwa.width, gwa.height});
        pWindow->bWindowIsFullscreen = bFullScreen;

        return true;
    }

    void Host_Linux_X11::enableTouch(X11::Display* display, X11::Window window)
    {
        // Check if we have the XInput extension
        int extension_code;
        int ev;
        int err;

        // If we have the extension, check for version 2.3 or higher
        if (X11::XQueryExtension(display, "XInputExtension", &extension_code, &ev, &err)) {
            int major{2};
            int minor{3};

            if(X11::XIQueryVersion(display, &major, &minor) != Success) {
                // Version is incorrect, do not select touch events
                return;
            }
        } else {
            // Do not have the extension, do not select touch events
            return;
        }

        // If we get down here, we have the extension and version is correct
        xinput_extension_code = extension_code;

        X11::XIEventMask mask{};
        mask.deviceid = XIAllDevices;
        mask.mask_len = XIMaskLen(XI_TouchEnd);
        mask.mask = static_cast<unsigned char*>(std::calloc(mask.mask_len, sizeof(char)));

        XISetMask(mask.mask, XI_TouchBegin);
        XISetMask(mask.mask, XI_TouchUpdate);
        XISetMask(mask.mask, XI_TouchEnd);

        X11::XISelectEvents(display, window, &mask, 1);

        X11::XSync(display, False);
        std::free(mask.mask);
    }

    void Host_Linux_X11::handleTouchEvent(X11::XIDeviceEvent* event)
    {
        auto itr_child = mapX11Window2PTR.find(event->child);
        if(itr_child == mapX11Window2PTR.end()) {

            return;
        }
        
        olc::Window* pge_window = itr_child->second;

        switch(event->evtype) {
            case XI_TouchBegin:
                pge_window->olc_OnTouch(
                    event->detail,
                    olc::vf2d(event->event_x, event->event_y),
                    true,
                    false,
                    olc::vf2d{1.0f, .0f}
                );
                break;
            case XI_TouchUpdate:
            {
                pge_window->olc_OnTouch(
                    event->detail,
                    olc::vf2d(event->event_x, event->event_y),
                    false,
                    false,
                    olc::vf2d{1.0f, 1.0f}
                );
                break;
            }
            case XI_TouchEnd:
                pge_window->olc_OnTouch(
                    event->detail,
                    olc::vf2d(event->event_x, event->event_y),
                    false,
                    true,
                    olc::vf2d{1.0f, 1.0f}
                );
                break;
        }
    }

}
//! END IMPLEMENTATION
