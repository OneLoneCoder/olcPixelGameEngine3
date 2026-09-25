#include "host_lin_wayland.h"


//! START IMPLEMENTATION
namespace olc::host
{
    namespace wayland {
        static const wl_registry_listener registry_listener {
            .global = Host_Linux_Wayland::registry_handle_global_callback,
            .global_remove = Host_Linux_Wayland::registry_handle_global_remove_callback,
        };

        static const wl_seat_listener seat_listener {
            .capabilities = Host_Linux_Wayland::seat_capabilities_callback,
            .name = Host_Linux_Wayland::seat_name_callback
        };

        static const wl_pointer_listener pointer_listener {
            .enter = Host_Linux_Wayland::pointer_enter_callback,
            .leave = Host_Linux_Wayland::pointer_leave_callback,
            .motion = Host_Linux_Wayland::pointer_motion_callback,
            .button = Host_Linux_Wayland::pointer_button_callback,
            .axis = Host_Linux_Wayland::pointer_axis_callback,
            .frame = Host_Linux_Wayland::pointer_frame_callback,
            .axis_source = Host_Linux_Wayland::pointer_axis_source_callback,
            .axis_stop = Host_Linux_Wayland::pointer_axis_stop_callback,
            .axis_discrete = Host_Linux_Wayland::pointer_axis_discrete_callback,
            .axis_value120 = Host_Linux_Wayland::pointer_axis_value120_callback,
            .axis_relative_direction = Host_Linux_Wayland::pointer_axis_relative_direction_callback
        };

        static const wl_keyboard_listener keyboard_listener {
            .keymap = Host_Linux_Wayland::keyboard_keymap_callback,
            .enter = Host_Linux_Wayland::keyboard_enter_callback,
            .leave = Host_Linux_Wayland::keyboard_leave_callback,
            .key = Host_Linux_Wayland::keyboard_key_callback,
            .modifiers = Host_Linux_Wayland::keyboard_modifiers_callback,
            .repeat_info = Host_Linux_Wayland::keyboard_repeat_info_callback
        };

        static const wl_touch_listener touch_listener {
            .down = Host_Linux_Wayland::touch_down_callback,
            .up = Host_Linux_Wayland::touch_up_callback,
            .motion = Host_Linux_Wayland::touch_motion_callback,
            .frame = Host_Linux_Wayland::touch_frame_callback,
            .cancel = Host_Linux_Wayland::touch_cancel_callback,
            .shape = Host_Linux_Wayland::touch_shape_callback,
            .orientation = Host_Linux_Wayland::touch_orientation_callback
        };
    }

    namespace xdg {
        static const xdg_wm_base_listener xdg_base_listener {
            .ping = Host_Linux_Wayland::xdg_wm_ping_callback
        };

        static const xdg_surface_listener surface_listener {
            .configure = Host_Linux_Wayland::xdg_surface_configure_callback
        };
    }

    namespace decor {
        static libdecor_interface libdecor_error_listener = {
            .error = Host_Linux_Wayland::libdecor_error_callback,
        };

        static libdecor_frame_interface libdecor_frame_listener = {
            .configure = Host_Linux_Wayland::libdecor_frame_configure_callback,
            .close = Host_Linux_Wayland::libdecor_close_callback,
            .commit = Host_Linux_Wayland::libdecor_commit_callback,
            .dismiss_popup = Host_Linux_Wayland::libdecor_dismiss_popup_callback
        };
    }

    Host_Linux_Wayland::Host_Linux_Wayland()
    {
        
        display = wl_display_connect(NULL);
        registry = wl_display_get_registry(display);

        wl_registry_add_listener(registry, &wayland::registry_listener, this);
        wl_display_roundtrip(display);
        
        if(compositor == nullptr || xdg_wm == nullptr || seat == nullptr) {
            throw;
        }

        decor_context = libdecor_new(display, &decor::libdecor_error_listener);

        // Load the default cursor
        cursor_theme = wl_cursor_theme_load(NULL, 24, shm);
        wl_cursor *cursor = wl_cursor_theme_get_cursor(cursor_theme, "left_ptr");

        cursor_image = cursor->images[0];
        wl_buffer *cursor_buffer = wl_cursor_image_get_buffer(cursor_image);

        cursor_surface = wl_compositor_create_surface(compositor);
        wl_surface_attach(cursor_surface, cursor_buffer, 0, 0);
        wl_surface_commit(cursor_surface);


        kb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

        wl_display_roundtrip(display);

        // Setup the keymap with XKB codes, which are basically the same as the X11 codes
        mapKeys[XKB_KEY_NoSymbol] = Key::NONE;

        int keyTracker = static_cast<int>(Key::A);
        uint32_t uppercase = static_cast<uint32_t>(XKB_KEY_A);
        uint32_t lowercase = static_cast<uint32_t>(XKB_KEY_a);

        for (; uppercase <= static_cast<uint32_t>(XKB_KEY_Z); ++uppercase, ++lowercase)
        {
            mapKeys[uppercase] = (Key)keyTracker;
            mapKeys[lowercase] = (Key)keyTracker;
            ++keyTracker;
        }

        mapKeys[XKB_KEY_F1] = Key::F1; mapKeys[XKB_KEY_F2] = Key::F2; mapKeys[XKB_KEY_F3] = Key::F3; mapKeys[XKB_KEY_F4] = Key::F4;
        mapKeys[XKB_KEY_F5] = Key::F5; mapKeys[XKB_KEY_F6] = Key::F6; mapKeys[XKB_KEY_F7] = Key::F7; mapKeys[XKB_KEY_F8] = Key::F8;
        mapKeys[XKB_KEY_F9] = Key::F9; mapKeys[XKB_KEY_F10] = Key::F10; mapKeys[XKB_KEY_F11] = Key::F11; mapKeys[XKB_KEY_F12] = Key::F12;

        mapKeys[XKB_KEY_Down] = Key::DOWN; mapKeys[XKB_KEY_Left] = Key::LEFT; mapKeys[XKB_KEY_Right] = Key::RIGHT; mapKeys[XKB_KEY_Up] = Key::UP;
        mapKeys[XKB_KEY_KP_Enter] = Key::ENTER; mapKeys[XKB_KEY_Return] = Key::ENTER;

        mapKeys[XKB_KEY_BackSpace] = Key::BACK; mapKeys[XKB_KEY_Escape] = Key::ESCAPE; mapKeys[XKB_KEY_Linefeed] = Key::ENTER;	mapKeys[XKB_KEY_Pause] = Key::PAUSE;
        mapKeys[XKB_KEY_Scroll_Lock] = Key::SCROLL; mapKeys[XKB_KEY_Tab] = Key::TAB; mapKeys[XKB_KEY_Delete] = Key::DEL; mapKeys[XKB_KEY_Home] = Key::HOME;
        mapKeys[XKB_KEY_End] = Key::END; mapKeys[XKB_KEY_Page_Up] = Key::PGUP; mapKeys[XKB_KEY_Page_Down] = Key::PGDN;	mapKeys[XKB_KEY_Insert] = Key::INS;
        mapKeys[XKB_KEY_Shift_L] = Key::SHIFT; mapKeys[XKB_KEY_Shift_R] = Key::SHIFT; mapKeys[XKB_KEY_Control_L] = Key::CTRL; mapKeys[XKB_KEY_Control_R] = Key::CTRL;
        mapKeys[XKB_KEY_Alt_L] = Key::ALT; mapKeys[XKB_KEY_Alt_R] = Key::ALT;
        mapKeys[XKB_KEY_space] = Key::SPACE; mapKeys[XKB_KEY_period] = Key::PERIOD;

        mapKeys[XKB_KEY_0] = Key::K0; mapKeys[XKB_KEY_1] = Key::K1; mapKeys[XKB_KEY_2] = Key::K2; mapKeys[XKB_KEY_3] = Key::K3; mapKeys[XKB_KEY_4] = Key::K4;
        mapKeys[XKB_KEY_5] = Key::K5; mapKeys[XKB_KEY_6] = Key::K6; mapKeys[XKB_KEY_7] = Key::K7; mapKeys[XKB_KEY_8] = Key::K8; mapKeys[XKB_KEY_9] = Key::K9;

        mapKeys[XKB_KEY_KP_0] = Key::NP0; mapKeys[XKB_KEY_KP_1] = Key::NP1; mapKeys[XKB_KEY_KP_2] = Key::NP2; mapKeys[XKB_KEY_KP_3] = Key::NP3; mapKeys[XKB_KEY_KP_4] = Key::NP4;
        mapKeys[XKB_KEY_KP_5] = Key::NP5; mapKeys[XKB_KEY_KP_6] = Key::NP6; mapKeys[XKB_KEY_KP_7] = Key::NP7; mapKeys[XKB_KEY_KP_8] = Key::NP8; mapKeys[XKB_KEY_KP_9] = Key::NP9;
        mapKeys[XKB_KEY_KP_Multiply] = Key::NP_MUL; mapKeys[XKB_KEY_KP_Add] = Key::NP_ADD; mapKeys[XKB_KEY_KP_Divide] = Key::NP_DIV; mapKeys[XKB_KEY_KP_Subtract] = Key::NP_SUB; mapKeys[XKB_KEY_KP_Decimal] = Key::NP_DECIMAL;

        // These map the keypad when NUMLOCK is off
        mapKeys[XKB_KEY_KP_Home] = Key::HOME; mapKeys[XKB_KEY_KP_End] = Key::END; mapKeys[XKB_KEY_KP_Up] = Key::UP;
        mapKeys[XKB_KEY_KP_Down] = Key::DOWN; mapKeys[XKB_KEY_KP_Left] = Key::LEFT; mapKeys[XKB_KEY_KP_Right] = Key::RIGHT;
        mapKeys[XKB_KEY_KP_Page_Up] = Key::PGUP; mapKeys[XKB_KEY_KP_Page_Down] = Key::PGDN; mapKeys[XKB_KEY_KP_Insert] = Key::INS;
        mapKeys[XKB_KEY_KP_Delete] = Key::DEL;

        // These keys vary depending on the keyboard. I've included comments for US and UK keyboard layouts
        mapKeys[XKB_KEY_semicolon] = Key::OEM_1;		// On US and UK keyboards this is the ';:' key
        mapKeys[XKB_KEY_slash] = Key::OEM_2;			// On US and UK keyboards this is the '/?' key
        mapKeys[XKB_KEY_asciitilde] = Key::OEM_3;	// On US keyboard this is the '~' key
        mapKeys[XKB_KEY_grave] = Key::OEM_3;	// On US keyboard this is the '`' key
        mapKeys[XKB_KEY_bracketleft] = Key::OEM_4;	// On US and UK keyboards this is the '[{' key
        mapKeys[XKB_KEY_backslash] = Key::OEM_5;		// On US keyboard this is '\|' key.
        mapKeys[XKB_KEY_bracketright] = Key::OEM_6;	// On US and UK keyboards this is the ']}' key
        mapKeys[XKB_KEY_apostrophe] = Key::OEM_7;	// On US keyboard this is the single/double quote key. On UK, this is the single quote/@ symbol key
        mapKeys[XKB_KEY_numbersign] = Key::OEM_8;	// miscellaneous characters. Varies by keyboard. I believe this to be the '#~' key on UK keyboards
        mapKeys[XKB_KEY_equal] = Key::EQUALS;		// the '+' key on any keyboard
        mapKeys[XKB_KEY_comma] = Key::COMMA;			// the comma key on any keyboard
        mapKeys[XKB_KEY_minus] = Key::MINUS;			// the minus key on any keyboard			

        mapKeys[XKB_KEY_Caps_Lock] = Key::CAPS_LOCK;

        UpdateKeyboardLayout();
    }

    WaylandWindow::~WaylandWindow() {
        if(window) {
            wl_egl_window_destroy(window);
        }
        if(surface_xdg) {
            xdg_surface_destroy(surface_xdg);
        }
        if(decor_frame) {
            libdecor_frame_unref(decor_frame);
        }
        wl_surface_destroy(surface);
    }

    Host_Linux_Wayland::~Host_Linux_Wayland()
    {
        mapUID2OlcWindow.clear();
        mapUID2Window.clear();

        if(decor_context) {
            libdecor_unref(decor_context);
            decor_context = nullptr;
        }

        xkb_state_unref(kb_state);
        xkb_keymap_unref(kb_keymap);
        xkb_context_unref(kb_context);
        wl_cursor_theme_destroy(cursor_theme);
        wl_surface_destroy(cursor_surface);

        xdg_wm_base_destroy(xdg_wm);
        if(pointer_warp)
        {
            wp_pointer_warp_v1_destroy(pointer_warp);
        }
        if(touch) {
            wl_touch_destroy(touch);
        }
        wl_keyboard_destroy(keyboard);
        wl_pointer_destroy(pointer);
        wl_seat_destroy(seat);
        wl_compositor_destroy(compositor);
        wl_shm_destroy(shm);
        wl_registry_destroy(registry);
        wl_display_disconnect(display);
    }

    bool Host_Linux_Wayland::OnApplicationStart(olc::PixelGameEngine* pPrimary)
    {
        pPrimaryPGE = pPrimary;
        return true;
    }

    bool Host_Linux_Wayland::StartSystem()
    {
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
        
        pollfd decor_wl_fd {.fd = libdecor_get_fd(decor_context), .events = POLLIN};

        bool keep_running = true;
        while(systemActive && keep_running) {
            if(decor_context) {
                poll(&decor_wl_fd, 1, -1);
                {
                    std::lock_guard<std::mutex> l{decor_mutex};
                    keep_running = libdecor_dispatch(decor_context, 0) >= 0;
                }
            }
        }
        
        systemActive = false;
        if(threadSystem.joinable())
            threadSystem.join();
    
        return pPrimaryPGE->OnPostContextEnd();
    }

    bool Host_Linux_Wayland::StopSystem()
    {
        systemActive = false;
        return true;
    }

    bool Host_Linux_Wayland::OnSystemThreadStart()
    {
        return pPrimaryPGE->OnContextStart();
    }

    bool Host_Linux_Wayland::OnSystemTick()
    {
        return pPrimaryPGE->OnContextTick();;
    }

    bool Host_Linux_Wayland::OnSystemThreadEnd()
    {
        return pPrimaryPGE->OnContextEnd();
    }

    bool Host_Linux_Wayland::OnApplicationEnd()
    {
        return true;
    }

    bool Host_Linux_Wayland::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        // Create a window
        WaylandWindow& w = mapUID2Window[pWindow->GetUID()];
        wl_region* region = wl_compositor_create_region(compositor);
        wl_region_add(region, vWindowPos.x, vWindowPos.y, vWindowSize.x, vWindowSize.y);
        
        w.surface = wl_compositor_create_surface(compositor);

        std::lock_guard<std::mutex> l{decor_mutex};
        w.decor_frame = libdecor_decorate(decor_context, w.surface, &decor::libdecor_frame_listener, this);
        w.floating_width = vWindowSize.x;
        w.floating_height = vWindowSize.y;
        libdecor_frame_set_app_id(w.decor_frame, "olcPixelGameEngine");
        libdecor_frame_set_title(w.decor_frame, "OneLoneCoder.com - Pixel Game Engine");

        if(!pWindow->config.bResizeable) {
            libdecor_frame_unset_capabilities(w.decor_frame, LIBDECOR_ACTION_RESIZE);
        }

        libdecor_frame_map(w.decor_frame);

        wl_surface_set_opaque_region(w.surface, region);
        w.window = wl_egl_window_create(w.surface, vWindowSize.x, vWindowSize.y);
        w.olc_window_uid = pWindow->GetUID();
        wl_surface_commit(w.surface);
        wl_region_destroy(region);

        pWindow->SetWindowPosition(vWindowPos);
        pWindow->SetWindowSize(vWindowSize);

        mapUID2OlcWindow.insert_or_assign(pWindow->GetUID(), pWindow);

        return true;
    }

    bool Host_Linux_Wayland::CloseWindowFrame(olc::Window* pWindow)
    {
        const auto uid = pWindow->GetUID();
        mapUID2Window.erase(uid);
        mapUID2OlcWindow.erase(uid);

        return true;
    }
    bool Host_Linux_Wayland::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        auto itr = mapUID2Window.find(pWindow->GetUID());
        if(itr != mapUID2Window.end()) {
            std::lock_guard<std::mutex> l{decor_mutex};
            libdecor_frame_set_title(itr->second.decor_frame, pWindow->GetWindowTitle().c_str());
        }
        return true;
    }

    void Host_Linux_Wayland::UpdateKeyboardLayout()
    {
        if(!kb_keymap)
        {
            keyboardLayout = OLC_DEFAULT_KEYBOARD_LAYOUT;
            return;
        }
        
        const char* layoutName = xkb_keymap_layout_get_name(kb_keymap, kb_group);
    
        if (!layoutName)
        {
            keyboardLayout = OLC_DEFAULT_KEYBOARD_LAYOUT;
            return;
        }
        
        keyboardLayout = OLC_DEFAULT_KEYBOARD_LAYOUT;
        
        std::string layout(layoutName);
        std::transform(layout.begin(), layout.end(), layout.begin(), ::tolower);
    
        if (layout.find("us") != std::string::npos)
            keyboardLayout = olc::KeyboardLayout::QWERTY_US;
        else if (layout.find("gb") != std::string::npos || layout.find("uk") != std::string::npos)
            keyboardLayout = olc::KeyboardLayout::QWERTY_UK;
        else if (layout.find("de") != std::string::npos || layout.find("german") != std::string::npos)
            keyboardLayout = olc::KeyboardLayout::QWERTZ;
        else if (layout.find("fr") != std::string::npos || layout.find("french") != std::string::npos)
            keyboardLayout = olc::KeyboardLayout::AZERTY;
    }

    olc::KeyboardLayout Host_Linux_Wayland::GetKeyboardLayout() const
    {
        return keyboardLayout;
    }

    bool Host_Linux_Wayland::SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos)
    {
        if(pointer_warp) {
            auto itr = mapUID2Window.find(pWindow->GetUID());
            if(itr != mapUID2Window.end()) {
                wp_pointer_warp_v1_warp_pointer(pointer_warp, itr->second.surface, pointer, wl_fixed_from_int(vPos.x), wl_fixed_from_int(vPos.y), enter_serial);
                pWindow->olc_OnMouseMove(vPos);
                return true;
            }
        }

        return false;
    }

    bool Host_Linux_Wayland::SetMouseVisible(olc::Window* pWindow, const bool bVisible)
    {
        auto itr = mapUID2Window.find(pWindow->GetUID());
        if(itr != mapUID2Window.end()) {
            itr->second.cursor_visible = bVisible;

            if(bVisible) {
                wl_pointer_set_cursor(pointer, enter_serial, cursor_surface, cursor_image->hotspot_x, cursor_image->hotspot_y);
            } else {
                wl_pointer_set_cursor(pointer, enter_serial, nullptr, 0, 0);
            }
        }

        return true;
    }

    bool Host_Linux_Wayland::SetFullScreen(olc::Window* pWindow, const bool bFullScreen)
    {
        auto itr = mapUID2Window.find(pWindow->GetUID());
        if(itr != mapUID2Window.end()) {
            pWindow->bWindowIsFullscreen = bFullScreen;
            if(bFullScreen) {
                libdecor_frame_set_fullscreen(itr->second.decor_frame, nullptr);

            } else {
                libdecor_frame_unset_fullscreen(itr->second.decor_frame);
            }
        }
        return true;
    }

    void Host_Linux_Wayland::registry_handle_global(wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
        if(std::strcmp(interface, wl_compositor_interface.name) == 0) {
            compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, version));
        }
        if(std::strcmp(interface, wl_shm_interface.name) == 0) {
            shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, version));
        }
        if(std::strcmp(interface, xdg_wm_base_interface.name) == 0) {
            xdg_wm = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, version));
        }
        if(std::strcmp(interface, wl_seat_interface.name) == 0) {
            seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, version));
            wl_seat_add_listener(seat, &wayland::seat_listener, this);
        }
        if(std::strcmp(interface, wl_keyboard_interface.name) == 0) {
            keyboard = static_cast<wl_keyboard*>(wl_registry_bind(registry, name, &wl_keyboard_interface, version));
        }
        if(std::strcmp(interface, wp_pointer_warp_v1_interface.name) == 0) {
            pointer_warp = static_cast<wp_pointer_warp_v1*>(wl_registry_bind(registry, name, &wp_pointer_warp_v1_interface, version));
        }
    }
    
    void Host_Linux_Wayland::registry_handle_global_remove(wl_registry* registry, uint32_t name)
    {

    }

    void Host_Linux_Wayland::seat_capabilities(wl_seat* seat, uint32_t capabilities)
    {
        if (capabilities & WL_SEAT_CAPABILITY_POINTER && pointer == nullptr) {
            pointer = wl_seat_get_pointer(seat);
            wl_pointer_add_listener(pointer, &wayland::pointer_listener, this);
        }

        if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD && keyboard == nullptr) {
            keyboard = wl_seat_get_keyboard(seat);
            keyboard_version = wl_keyboard_get_version(keyboard);
            wl_keyboard_add_listener(keyboard, &wayland::keyboard_listener, this);
        }

        if (capabilities & WL_SEAT_CAPABILITY_TOUCH && touch == nullptr) {
            touch = wl_seat_get_touch(seat);
            wl_touch_add_listener(touch, &wayland::touch_listener, this);
        }
    }

    void Host_Linux_Wayland::registry_handle_global_callback(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->registry_handle_global(registry, name, interface, version);
    }
    void Host_Linux_Wayland::registry_handle_global_remove_callback(void* data, wl_registry* registry, uint32_t name)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->registry_handle_global_remove(registry, name);
    }

    void Host_Linux_Wayland::seat_capabilities_callback(void* data, wl_seat* seat, uint32_t capabilities)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->seat_capabilities(seat, capabilities);
    }

    void Host_Linux_Wayland::seat_name_callback(void* data, wl_seat* wl_seat, const char* name)
    {

    }

    // Pointer Callbacks
    void Host_Linux_Wayland::pointer_enter_callback(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_enter(pointer, serial, surface, surface_x, surface_y);
    }

    void Host_Linux_Wayland::pointer_enter(wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventEnter;
        pointer_state.serial = serial;
        // Save so we can reuse the serial for pointer warping
        enter_serial = serial;
        pointer_state.surface = surface;
        pointer_state.surface_x = surface_x;
        pointer_state.surface_y = surface_y;
    }

    void Host_Linux_Wayland::pointer_leave_callback(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_leave(pointer, serial, surface);
    }

    void Host_Linux_Wayland::pointer_leave(wl_pointer* pointer, uint32_t serial, wl_surface* surface)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventLeave;
        pointer_state.serial = serial;
    }

    void Host_Linux_Wayland::pointer_motion_callback(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_motion(pointer, time, surface_x, surface_y);
    }

    void Host_Linux_Wayland::pointer_motion(wl_pointer* pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventMotion;
        pointer_state.time = time;
        pointer_state.surface_x = surface_x;
        pointer_state.surface_y = surface_y;   
    }

    void Host_Linux_Wayland::pointer_button_callback(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_button(pointer, serial, time, button, state);
    }

    void Host_Linux_Wayland::pointer_button(wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventButton;
        pointer_state.time = time;
        pointer_state.serial = serial;
        pointer_state.button = button;
        pointer_state.state = state;
    }

    void Host_Linux_Wayland::pointer_axis_callback(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_axis(pointer, time, axis, value);
    }

    void Host_Linux_Wayland::pointer_axis(wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventAxis;
        pointer_state.time = time;
        pointer_state.axes[axis].valid = true;
        pointer_state.axes[axis].value = value;
    }

    void Host_Linux_Wayland::pointer_frame_callback(void* data, wl_pointer* pointer)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_frame(pointer);
    }

    void Host_Linux_Wayland::pointer_frame(wl_pointer* pointer)
    {
        wayland::PointerState *event = &pointer_state;
        auto pointer_window = active_window_id;
        // Since PGE does not distinguish between "mouse hover" and "window focus" we won't actually trigger a Window Focus
        // for the mouse hovering over the window.  We'll just send this mouse event to that window without actually marking it as focused.
        if (pointer_state.event_mask & wayland::PointerEventMask::PointerEventEnter) {
            for(auto& itr : mapUID2Window) {
                if (itr.second.surface == event->surface) {
                    pointer_window = itr.first;
                    
                    // Need to set the mouse back to the correct hidden / not hidden state when it enters the window
                    if(itr.second.cursor_visible) {
                        wl_pointer_set_cursor(pointer, enter_serial, cursor_surface, cursor_image->hotspot_x, cursor_image->hotspot_y);
                    } else {
                        wl_pointer_set_cursor(pointer, enter_serial, nullptr, 0, 0);
                    }
                }
            }
        }
        
        auto* pge_window = mapUID2OlcWindow[pointer_window];

        if (pointer_state.event_mask & wayland::PointerEventMask::PointerEventMotion) {
                pge_window->olc_OnMouseMove(olc::vi2d{
                    wl_fixed_to_int(pointer_state.surface_x), 
                    wl_fixed_to_int(pointer_state.surface_y)
                });
        }

        if (pointer_state.event_mask & wayland::PointerEventMask::PointerEventButton) {
            switch (pointer_state.button) {
                case BTN_LEFT: pge_window->olc_OnMouseButton(0, pointer_state.state == WL_POINTER_BUTTON_STATE_PRESSED); break;
                case BTN_MIDDLE: pge_window->olc_OnMouseButton(2, pointer_state.state == WL_POINTER_BUTTON_STATE_PRESSED); break;
                case BTN_RIGHT: pge_window->olc_OnMouseButton(1, pointer_state.state == WL_POINTER_BUTTON_STATE_PRESSED); break;
                case BTN_SIDE: pge_window->olc_OnMouseButton(3, pointer_state.state == WL_POINTER_BUTTON_STATE_PRESSED); break;
                case BTN_EXTRA: pge_window->olc_OnMouseButton(4, pointer_state.state == WL_POINTER_BUTTON_STATE_PRESSED); break;
                default: break;
            }
        }

        if(pointer_state.event_mask & wayland::PointerEventMask::PointerEventAxis
            && pointer_state.axes[WL_POINTER_AXIS_VERTICAL_SCROLL].valid)
        {
            pge_window->olc_OnMouseWheel(-wl_fixed_to_int(pointer_state.axes[WL_POINTER_AXIS_VERTICAL_SCROLL].value));
        }

        memset(event, 0, sizeof(*event));     
    }

    void Host_Linux_Wayland::pointer_axis_source_callback(void* data, wl_pointer* pointer, uint32_t axis_source)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_axis_source(pointer, axis_source);
    }

    void Host_Linux_Wayland::pointer_axis_source(wl_pointer* pointer, uint32_t axis_source)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventAxisSource;
        pointer_state.axis_source = axis_source;
    }

    void Host_Linux_Wayland::pointer_axis_stop_callback(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_axis_stop(pointer, time, axis);
    }

    void Host_Linux_Wayland::pointer_axis_stop(wl_pointer* pointer, uint32_t time, uint32_t axis)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventAxisStop;
        pointer_state.time = time;
        pointer_state.axes[axis].valid = true;
    }

    void Host_Linux_Wayland::pointer_axis_discrete_callback(void* data, wl_pointer* pointer, uint32_t axis, int32_t discrete)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->pointer_axis_discrete(pointer, axis, discrete);
    }

    void Host_Linux_Wayland::pointer_axis_discrete(wl_pointer* pointer, uint32_t axis, int32_t discrete)
    {
        pointer_state.event_mask |= wayland::PointerEventMask::PointerEventDiscrete;
        pointer_state.axes[axis].valid = true;
        pointer_state.axes[axis].discrete = discrete;
    }

    void Host_Linux_Wayland::pointer_axis_value120_callback(void* data, wl_pointer* pointer, uint32_t axis, int32_t value120)
    {
        //auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        //host->pointer_axis_value120(pointer, axis, value120);
    }

    void Host_Linux_Wayland::pointer_axis_relative_direction_callback(void* data, wl_pointer* pointer, uint32_t axis, uint32_t direction)
    {
        //auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        //host->pointer_axis_relative_direction(pointer, axis, direction);
    }
 
    // Keyboard Callbacks
    void Host_Linux_Wayland::keyboard_keymap_callback(void* data, wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_keymap(keyboard, format, fd, size);
    }

    void Host_Linux_Wayland::keyboard_keymap(wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size)
    {
        if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
            close(fd);
            return;
        }

        char* keymap_string = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
        if (keymap_string == MAP_FAILED) {
            close(fd);
            return;
        }

        if(kb_keymap)
            xkb_keymap_unref(kb_keymap);

        kb_keymap = xkb_keymap_new_from_string(kb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        munmap(keymap_string, size);
        close(fd);

        if (!kb_keymap) {
            return;
        }

        // Unreference the previous state if it exists and we got a new keymap from the server
        xkb_state_unref(kb_state);
        kb_state = xkb_state_new(kb_keymap);
        UpdateKeyboardLayout();
    }

    void Host_Linux_Wayland::keyboard_enter_callback(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_enter(keyboard, serial, surface, keys);
    }

    void Host_Linux_Wayland::keyboard_enter(wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
    {
        // Find the window that the keyboard is active on and mark it active
        for(auto& i : mapUID2Window) {
            if(i.second.surface == surface) {
                active_window_id = i.first;
            }
        }
        
        auto* pge_window = mapUID2OlcWindow[active_window_id];
        pge_window->olc_OnFocus(true);
    }

    void Host_Linux_Wayland::keyboard_leave_callback(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_leave(keyboard, serial, surface);
    }

    void Host_Linux_Wayland::keyboard_leave(wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
    {
        auto* pge_window = mapUID2OlcWindow[active_window_id];
        pge_window->olc_OnFocus(false);
    }

    void Host_Linux_Wayland::keyboard_key_callback(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_key(keyboard, serial, time, key, state);
    }

    void Host_Linux_Wayland::keyboard_key(wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
    {
        const auto sym = xkb_state_key_get_one_sym(kb_state, key + 8); // XKB keys are offset by 8
        auto itr = mapKeys.find(sym);
        if(itr != mapKeys.end()) {
            auto olc_key = itr->second;
            auto* pge_window = mapUID2OlcWindow[active_window_id];
            
            // Wayland keyboard version 10 and above support key repeat and release states
            if(keyboard_version >= 10)
            {
                switch (state) {
                    case WL_KEYBOARD_KEY_STATE_RELEASED:
                        pge_window->olc_OnKeyPress(olc_key, false);
                        break;
                    case WL_KEYBOARD_KEY_STATE_REPEATED:
                        pge_window->olc_OnKeyPress(olc_key, false);
                        // Intentional fallthrough
                    case WL_KEYBOARD_KEY_STATE_PRESSED:
                        pge_window->olc_OnKeyPress(olc_key, true);
                        break;
                }
            }
            else
            {
                // Ubuntu still parties like its 1999 apparently
                pge_window->olc_OnKeyPress(olc_key, state == WL_KEYBOARD_KEY_STATE_PRESSED);
            }
        }
    }

    void Host_Linux_Wayland::keyboard_modifiers_callback(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_modifiers(keyboard, serial, mods_depressed, mods_latched, mods_locked, group);
    }

    void Host_Linux_Wayland::keyboard_modifiers(wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
    {
        // The 'group' parameter is the currently active layout index!
        if(kb_group != group)
        {
            kb_group = group;
            UpdateKeyboardLayout();
        }

        xkb_state_update_mask(kb_state,
            mods_depressed & ~(1), // Just like X11, ignore the shift key
            mods_latched,
            mods_locked,
            0,
            0,
            group);
    }

    void Host_Linux_Wayland::keyboard_repeat_info_callback(void* data, wl_keyboard* keyboard, int32_t rate, int32_t delay)
    {
        // auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        // host->keyboard_repeat_info(keyboard, rate, delay);
    }

    // Touch Callbacks
    void Host_Linux_Wayland::touch_down_callback(void* data, wl_touch* touch, uint32_t serial, uint32_t time, wl_surface* surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->touch_down(touch, serial, time, surface, id, x, y);
    }

    void Host_Linux_Wayland::touch_down(wl_touch* touch, uint32_t serial, uint32_t time, wl_surface* surface, int32_t id, wl_fixed_t x, wl_fixed_t y) {
        auto& touch_state = touches[id];

        touch_state.event_mask |= wayland::TouchEventMask::TouchEventDown;
        touch_state.surface = surface;
        touch_state.id = id;
        touch_state.surface_x = x;
        touch_state.surface_y = y;
        touch_state.serial = serial;
        touch_state.time = time;
    }

    void Host_Linux_Wayland::touch_up_callback(void* data, wl_touch* touch, uint32_t serial, uint32_t time, int32_t id) {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->touch_up(touch, serial, time, id);
    }

    void Host_Linux_Wayland::touch_up(wl_touch* touch, uint32_t serial, uint32_t time, int32_t id) {
        auto& touch_state = touches[id];

        touch_state.event_mask |= wayland::TouchEventMask::TouchEventUp;
        touch_state.serial = serial;
        touch_state.time = time;
    }

    void Host_Linux_Wayland::touch_motion_callback(void* data, wl_touch* touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y) {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->touch_motion(touch, time, id, x, y);
    }

    void Host_Linux_Wayland::touch_motion(wl_touch* touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y) {
        auto& touch_state = touches[id];

        touch_state.event_mask |= wayland::TouchEventMask::TouchEventMotion;
        touch_state.time = time;
        touch_state.surface_x = x;
        touch_state.surface_y = y;
    }

    void Host_Linux_Wayland::touch_frame_callback(void* data, wl_touch* touch) {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->touch_frame(touch);
    }

    void Host_Linux_Wayland::touch_frame(wl_touch* touch) {
        // Commit all of the touches
        for(auto itr = touches.begin(); itr != touches.end();) {
            const auto& id = itr->first;
            const auto& touch = itr->second;

            olc::Window* pge_window {nullptr};
            size_t uid {0};
            for(auto& uid_itr : mapUID2Window) {
                if(uid_itr.second.surface == touch.surface) {
                    uid = uid_itr.second.olc_window_uid;
                }
            }

            if(const auto& pge_itr = mapUID2OlcWindow.find(uid); pge_itr != mapUID2OlcWindow.end()) {
                pge_window = pge_itr->second;
            } else {
                continue;
            }

            auto p_x = wl_fixed_to_double(touch.surface_x);
            auto p_y = wl_fixed_to_double(touch.surface_y);
            auto s_x = wl_fixed_to_double(touch.major);
            auto s_y = wl_fixed_to_double(touch.minor);
            olc::vf2d size (std::max(s_x, 1.0), std::max(s_y, 1.0));
            pge_window->olc_OnTouch(
                static_cast<uint32_t>(touch.id),
                olc::vf2d(p_x, p_y),
                touch.event_mask & wayland::TouchEventMask::TouchEventDown,
                touch.event_mask & wayland::TouchEventMask::TouchEventUp,
                size,
                false,
                0.0f,
                wl_fixed_to_double(touch.orientation)
            );

            if(touch.event_mask & wayland::TouchEventMask::TouchEventUp) {
                itr = touches.erase(itr);
            } else {
                ++itr;
            }
        }
    }

    void Host_Linux_Wayland::touch_cancel_callback(void* data, wl_touch* touch) {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->touch_cancel(touch);
    }

    void Host_Linux_Wayland::touch_cancel(wl_touch* touch) {
        // according to the protocol, this ends all touch events, so send an Up and clear the whole thing
        for(const auto& [id, touch] : touches) {
            olc::Window* pge_window {nullptr};
            size_t uid {0};
            for(auto& uid_itr : mapUID2Window) {
                if(uid_itr.second.surface == touch.surface) {
                    uid = uid_itr.second.olc_window_uid;
                }
            }

            if(const auto& pge_itr = mapUID2OlcWindow.find(uid); pge_itr != mapUID2OlcWindow.end()) {
                pge_window = pge_itr->second;
            } else {
                continue;
            }

            auto p_x = wl_fixed_to_double(touch.surface_x);
            auto p_y = wl_fixed_to_double(touch.surface_y);
            auto s_x = wl_fixed_to_double(touch.major);
            auto s_y = wl_fixed_to_double(touch.minor);
            olc::vf2d size (std::max(s_x, 1.0), std::max(s_y, 1.0));
            pge_window->olc_OnTouch(
                static_cast<uint32_t>(touch.id),
                olc::vf2d(p_x, p_y),
                touch.event_mask & wayland::TouchEventMask::TouchEventDown,
                true, // Forced Up event to clear all these IDs on the PGE side
                size,
                false,
                0.0f,
                wl_fixed_to_double(touch.orientation)
            );
        }

        touches.clear();
    }

    void Host_Linux_Wayland::touch_shape_callback(void* data, wl_touch* touch, int32_t id, wl_fixed_t major, wl_fixed_t minor) {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->touch_shape(touch, id, major, minor);
    }

    void Host_Linux_Wayland::touch_shape(wl_touch* touch, int32_t id, wl_fixed_t major, wl_fixed_t minor) {
        auto& touch_state = touches[id];

        touch_state.event_mask |= wayland::TouchEventMask::TouchEventShape;
        touch_state.major = major;
        touch_state.minor = minor;
    }

    void Host_Linux_Wayland::touch_orientation_callback(void* data, wl_touch* touch, int32_t id, wl_fixed_t orientation) {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->touch_orientation(touch, id, orientation);
    }

    void Host_Linux_Wayland::touch_orientation(wl_touch* touch, int32_t id, wl_fixed_t orientation) {
        auto& touch_state = touches[id];

        touch_state.event_mask |= wayland::TouchEventMask::TouchEventOrientation;
        touch_state.orientation = orientation;
    }


    // XDG Callbacks
    void Host_Linux_Wayland::xdg_wm_ping_callback(void* data, xdg_wm_base* wm, uint32_t serial) {
        xdg_wm_base_pong(wm, serial);
    }

    void Host_Linux_Wayland::xdg_surface_configure_callback(void* data, xdg_surface* surface, uint32_t serial)
    {
        xdg_surface_ack_configure(surface, serial);
    }


    void Host_Linux_Wayland::libdecor_error_callback(libdecor* context, libdecor_error error, const char* message)
    {
        std::cerr << "libdecor: " << error << ": " << message << "\n";
    }

    void Host_Linux_Wayland::libdecor_frame_configure_callback(libdecor_frame* frame, libdecor_configuration* config, void* data)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->libdecor_frame_configure(frame, config);
    }

    void Host_Linux_Wayland::libdecor_frame_configure(libdecor_frame* frame, libdecor_configuration* config)
    {
        for(auto& i : mapUID2Window) {
            if(i.second.decor_frame == frame) {
                auto* window = &i.second;
                auto& olc_window = mapUID2OlcWindow.at(i.first);

                int width{};
                int height{};

                if(!libdecor_configuration_get_window_state(config, &window->decor_window_state)) {
                    window->decor_window_state = LIBDECOR_WINDOW_STATE_NONE;
                }

                libdecor_configuration_get_content_size(config, frame, &width, &height);

                window->configured_width = width == 0 ? window->floating_width : width;
                window->configured_height = height == 0 ? window->floating_height : height;

                libdecor_state* state = libdecor_state_new(window->configured_width, window->configured_height);
                libdecor_frame_commit(frame, state, config);
                libdecor_state_free(state);

                // If we're not returning from fullscreen, goahead and resize
                if(libdecor_frame_is_floating(frame) && !olc_window->bWindowIsFullscreen) {
                    window->floating_width = width;
                    window->floating_height = height;
                }

                olc_window->bWindowIsFullscreen = (window->decor_window_state & LIBDECOR_WINDOW_STATE_FULLSCREEN) != 0;
                mapUID2OlcWindow[i.first]->olc_OnWindowSize({window->configured_width, window->configured_height});
                wl_egl_window_resize(window->window, window->configured_width, window->configured_height, 0, 0);
            }
        }
    }

    void Host_Linux_Wayland::libdecor_close_callback(libdecor_frame* frame, void* data)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->libdecor_close(frame);
    }
    
    void Host_Linux_Wayland::libdecor_close(libdecor_frame* frame)
    {
        for(auto& i : mapUID2Window) {
            if(i.second.decor_frame == frame) {
                auto itr = mapUID2OlcWindow.find(i.second.olc_window_uid);
                if (itr != mapUID2OlcWindow.end()) {
                    auto* ptr = itr->second;
                    ptr->olc_OnWindowClose();
                }
            }
        }        
    }

    void Host_Linux_Wayland::libdecor_commit_callback(libdecor_frame* frame, void* data)
    {
        // Don't want to actually do anything here because it messes with the EGL surface swapping for refresh
        // and causes the application to close (not crash) due to a wayland protocol violation
        //auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        //host->libdecor_commit(frame);
    }

    void Host_Linux_Wayland::libdecor_commit(libdecor_frame* frame)
    {
    }

    void Host_Linux_Wayland::libdecor_dismiss_popup_callback(libdecor_frame* frame, const char* seat_name, void* data)
    {

    }

    std::vector<void*> Host_Linux_Wayland::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2Window.find(pWindow->GetUID());
        if (window_handle != mapUID2Window.end()) {
            return {reinterpret_cast<void*>(&window_handle->second),
                reinterpret_cast<void*>(display)
            };
        }
        return {};
    }

    bool Host_Linux_Wayland::SyncWithDesktopComposite()
    {
        return true;
    }

}