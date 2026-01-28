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
    }

    namespace xdg {
        static const xdg_wm_base_listener xdg_base_listener {
            .ping = Host_Linux_Wayland::xdg_wm_ping_callback
        };

        static const xdg_surface_listener surface_listener {
            .configure = Host_Linux_Wayland::xdg_surface_configure_callback
        };

        static const xdg_toplevel_listener xdg_top_listener {
            .configure = Host_Linux_Wayland::xdg_toplevel_configure_callback,
            .close = Host_Linux_Wayland::xdg_toplevel_close_callback,
            .configure_bounds = Host_Linux_Wayland::xdg_toplevel_configure_bounds_callback,
            .wm_capabilities = Host_Linux_Wayland::xdg_toplevel_capabilities_callback
        };

        static const zxdg_toplevel_decoration_v1_listener toplevel_decoration_listener {
            .configure = Host_Linux_Wayland::xdg_toplevel_decoration_configure_callback
        };
    }

    Host_Linux_Wayland::Host_Linux_Wayland()
    {
        
        display = wl_display_connect(NULL);
        registry = wl_display_get_registry(display);

        wl_registry_add_listener(registry, &wayland::registry_listener, this);
        wl_display_roundtrip(display);

        if(compositor == nullptr || xdg_wm == nullptr || seat == nullptr || decoration_manager == nullptr) {
            throw;
        }
        
        xdg_wm_base_add_listener(xdg_wm, &xdg::xdg_base_listener, this);
        wl_seat_add_listener(seat, &wayland::seat_listener, this);

        kb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

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
    }

    Host_Linux_Wayland::~Host_Linux_Wayland()
    {
        for (auto& itr : mapUID2Window) {
            auto& wayland_window = itr.second;
            wl_egl_window_destroy(wayland_window.window);
            xdg_toplevel_destroy(wayland_window.toplevel);
            xdg_surface_destroy(wayland_window.surface_xdg);
            wl_surface_destroy(wayland_window.surface);
        }

        xkb_state_unref(kb_state);
        xkb_context_unref(kb_context);

        wl_display_disconnect(display);
    }

    bool Host_Linux_Wayland::StartSystemEventLoop(bool bBlockIfPossible)
    {
        if(bBlockIfPossible) {
            while(!terminate && wl_display_dispatch_pending(display) != -1) {

            }
        }

        return true;
    }

    bool Host_Linux_Wayland::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        // Create a window
        WaylandWindow w;
        wl_region* region = wl_compositor_create_region(compositor);
        wl_region_add(region, vWindowPos.x, vWindowPos.y, vWindowSize.x, vWindowSize.y);
        
        w.surface = wl_compositor_create_surface(compositor);
        w.surface_xdg = xdg_wm_base_get_xdg_surface(xdg_wm, w.surface);

        xdg_surface_add_listener(w.surface_xdg, &xdg::surface_listener, this);
        w.toplevel = xdg_surface_get_toplevel(w.surface_xdg);
        xdg_toplevel_set_title(w.toplevel, "OneLoneCoder.com - Pixel Game Engine");
        xdg_toplevel_add_listener(w.toplevel, &xdg::xdg_top_listener, this);
        wl_surface_set_opaque_region(w.surface, region);
        w.window = wl_egl_window_create(w.surface, vWindowSize.x, vWindowSize.y);
        w.olc_window_uid = pWindow->GetUID();
        wl_surface_commit(w.surface);
        wl_region_destroy(region);

        w.decorations = zxdg_decoration_manager_v1_get_toplevel_decoration(decoration_manager, w.toplevel);
        zxdg_toplevel_decoration_v1_add_listener(w.decorations, &xdg::toplevel_decoration_listener, this);
        zxdg_toplevel_decoration_v1_set_mode(w.decorations, 2);

        pWindow->SetWindowPosition(vWindowPos);
        pWindow->SetWindowSize(vWindowSize);

        mapUID2Window.insert_or_assign(pWindow->GetUID(), w);
        mapUID2OlcWindow.insert_or_assign(pWindow->GetUID(), pWindow);
        return true;
    }

    bool Host_Linux_Wayland::CloseWindowFrame(olc::Window* pWindow)
    {
        auto itr = mapUID2Window.find(pWindow->GetUID());
        if(itr != mapUID2Window.end()) {
            auto& wayland_window = itr->second;
            wl_egl_window_destroy(wayland_window.window);
            zxdg_toplevel_decoration_v1_destroy(wayland_window.decorations);
            xdg_toplevel_destroy(wayland_window.toplevel);
            xdg_surface_destroy(wayland_window.surface_xdg);
            wl_surface_destroy(wayland_window.surface);
            auto uid = wayland_window.olc_window_uid;
            mapUID2Window.erase(uid);
            mapUID2OlcWindow.erase(uid);
        }

        return true;
    }
    bool Host_Linux_Wayland::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        auto itr = mapUID2Window.find(pWindow->GetUID());
        if(itr != mapUID2Window.end()) {
            xdg_toplevel_set_title(itr->second.toplevel, pWindow->GetWindowTitle().c_str());
        }
        return true;
    }

    olc::KeyboardLayout Host_Linux_Wayland::GetKeyboardLayout() const
    {
        return olc::KeyboardLayout::QWERTY_US;
    }

    void Host_Linux_Wayland::registry_handle_global(wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
        if(std::strcmp(interface, wl_compositor_interface.name) == 0) {
            compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, version));
        }
        if(std::strcmp(interface, xdg_wm_base_interface.name) == 0) {
            xdg_wm = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, version));
        }
        if(std::strcmp(interface, wl_seat_interface.name) == 0) {
            seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, version));
        }
        if(std::strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0) {
            decoration_manager = static_cast<zxdg_decoration_manager_v1*>(wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, version));
        }
        if(std::strcmp(interface, wl_keyboard_interface.name) == 0) {
            keyboard = static_cast<wl_keyboard*>(wl_registry_bind(registry, name, &wl_keyboard_interface, version));
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
            wl_keyboard_add_listener(keyboard, &wayland::keyboard_listener, this);
        }
    }

    void Host_Linux_Wayland::xdg_toplevel_configure(xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
    {
        for(auto& i : mapUID2Window) {
            auto& w = i.second;
            if(w.toplevel == toplevel) {
                // Attempt to constrain the window size to what the compositor may have told us earlier
                // in a bounds_configure message
                if(w.bounds_x != 0) {
                    width = std::min<int32_t>(width, w.bounds_x);
                }

                if(w.bounds_y != 0) {
                    height = std::min<int32_t>(height, w.bounds_y);
                }
                
                mapUID2OlcWindow[i.first]->olc_OnWindowSize({width, height});
                wl_egl_window_resize(i.second.window, width, height, 0, 0);
                wl_surface_commit(i.second.surface);
            }
        }
    }

    void Host_Linux_Wayland::xdg_toplevel_close(xdg_toplevel* toplevel)
    {
        for(auto& i : mapUID2Window) {
            if(i.second.toplevel == toplevel) {
                auto itr = mapUID2OlcWindow.find(i.second.olc_window_uid);
                if(itr != mapUID2OlcWindow.end()) {
                    auto* ptr = itr->second;
                    ptr->olc_OnWindowClose();
                    terminate = true;
                }
            }
        }
    }

    void Host_Linux_Wayland::xdg_toplevel_configure_bounds(xdg_toplevel* toplevel, int32_t width, int32_t height)
    {
        for(auto& i : mapUID2Window) {
            if(i.second.toplevel == toplevel) {
                i.second.bounds_x = width;
                i.second.bounds_y = height;
            }
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

        if (pointer_state.event_mask & wayland::PointerEventMask::PointerEventEnter) {
            for(auto& itr : mapUID2Window) {
                if (itr.second.surface == event->surface) {
                    active_window_id = itr.first;
                }
            }
        }

        auto* pge_window = mapUID2OlcWindow[active_window_id];

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

        xkb_keymap* keymap = xkb_keymap_new_from_string(kb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        munmap(keymap_string, size);
        close(fd);

        if (!keymap) {
            return;
        }

        // Unreference the previous state if it exists and we got a new keymap from the server
        xkb_state_unref(kb_state);
        kb_state = xkb_state_new(keymap);
        xkb_keymap_unref(keymap);
    }

    void Host_Linux_Wayland::keyboard_enter_callback(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_enter(keyboard, serial, surface, keys);
    }

    void Host_Linux_Wayland::keyboard_enter(wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys)
    {
        // Currently do nothing
    }

    void Host_Linux_Wayland::keyboard_leave_callback(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_leave(keyboard, serial, surface);
    }

    void Host_Linux_Wayland::keyboard_leave(wl_keyboard* keyboard, uint32_t serial, wl_surface* surface)
    {
        // Currently do nothing
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
            pge_window->olc_OnKeyPress(olc_key, state == WL_KEYBOARD_KEY_STATE_PRESSED);
        }
    }

    void Host_Linux_Wayland::keyboard_modifiers_callback(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->keyboard_modifiers(keyboard, serial, mods_depressed, mods_latched, mods_locked, group);
    }

    void Host_Linux_Wayland::keyboard_modifiers(wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
    {
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

    // XDG Callbacks
    void Host_Linux_Wayland::xdg_wm_ping_callback(void* data, xdg_wm_base* wm, uint32_t serial) {
        xdg_wm_base_pong(wm, serial);
    }

    void Host_Linux_Wayland::xdg_surface_configure_callback(void* data, xdg_surface* surface, uint32_t serial)
    {
        xdg_surface_ack_configure(surface, serial);
    }

    void Host_Linux_Wayland::xdg_toplevel_configure_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->xdg_toplevel_configure(toplevel, width, height, states);
    }

    void Host_Linux_Wayland::xdg_toplevel_close_callback(void* data, xdg_toplevel* toplevel)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->xdg_toplevel_close(toplevel);
    }

    void Host_Linux_Wayland::xdg_toplevel_configure_bounds_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height) {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->xdg_toplevel_configure_bounds(toplevel, width, height);
        return;
    }

    void Host_Linux_Wayland::xdg_toplevel_capabilities_callback(void* data, xdg_toplevel* toplevel, wl_array* capabilities)
    {
        return;
    }

    void Host_Linux_Wayland::xdg_toplevel_decoration_configure_callback(void* data, zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1, uint32_t mode)
    {
        // auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        // fprintf(stderr, "zxdg_decoration_manager_v1 mode %d\n", mode);
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

    bool Host_Linux_Wayland::ConnectHostResourceToRenderer()
    {
        return true;
    }

    bool Host_Linux_Wayland::SyncWithDesktopComposite()
    {
        return true;
    }

}