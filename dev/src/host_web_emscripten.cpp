#include "host_web_emscripten.h"

//! START IMPLEMENTATION
namespace olc::host
{
    std::unordered_map<size_t, std::string> Host_Web_Emscripten::mapUID2CanvasId;
    std::unordered_map<std::string, olc::Window*> Host_Web_Emscripten::mapCanvasId2PTR;
    std::unordered_map<size_t, std::unique_ptr<Host_Web_Emscripten::CallbackData>> Host_Web_Emscripten::mapUID2CallbackData;

    Host_Web_Emscripten::Host_Web_Emscripten()
    {
        std::cout << "Emscripten: host constructed.\n";
        
        // Detect and Store Keyboard Layout
        EM_ASM({
            if (!navigator.keyboard || !navigator.keyboard.getLayoutMap)
                return;

            navigator.keyboard.getLayoutMap().then(function(map)
            {
                const keys = [map.get("KeyQ"), map.get("KeyW"), map.get("KeyE"), map.get("KeyR"), map.get("KeyT"), map.get("KeyY"), map.get("Backslash")];
                
                // QWERTY - UK/US
                if(keys[0] == 'q' && keys[1] == 'w' && keys[2] == 'e' && keys[3] == 'r' && keys[4] == 't' && keys[5] == 'y') {
                    if(keys[6] == '#' || keys[6] == '~')
                    {
                        Module.keyboardLayout = 0;
                        return;
                    }
                    else
                    {
                        Module.keyboardLayout = 1;
                        return;
                    }
                }

                // QWERTZ - DE
                if(keys[0] == 'q' && keys[1] == 'w' && keys[2] == 'e' && keys[3] == 'r' && keys[4] == 't' && keys[5] == 'z') {
                    Module.keyboardLayout = 2; 
                    return;
                }
                
                // AZERTY - FR
                if(keys[0] == 'q' && keys[1] == 'w' && keys[2] == 'e' && keys[3] == 'r' && keys[4] == 't' && keys[5] == 'z') {
                    Module.keyboardLayout = 3;
                    return;
                }
                
            });
        });

        // Map Emscripten Defined DOM_PK_ Codes to olc::KeyCodes
        mapKeys[DOM_PK_UNKNOWN] = Key::NONE;
        
        // A-Z
        mapKeys[DOM_PK_A] = Key::A;
        mapKeys[DOM_PK_B] = Key::B;
        mapKeys[DOM_PK_C] = Key::C;
        mapKeys[DOM_PK_D] = Key::D;
        mapKeys[DOM_PK_E] = Key::E;
        mapKeys[DOM_PK_F] = Key::F;
        mapKeys[DOM_PK_G] = Key::G;
        mapKeys[DOM_PK_H] = Key::H;
        mapKeys[DOM_PK_I] = Key::I;
        mapKeys[DOM_PK_J] = Key::J;
        mapKeys[DOM_PK_K] = Key::K;
        mapKeys[DOM_PK_L] = Key::L;
        mapKeys[DOM_PK_M] = Key::M;
        mapKeys[DOM_PK_N] = Key::N;
        mapKeys[DOM_PK_O] = Key::O;
        mapKeys[DOM_PK_P] = Key::P;
        mapKeys[DOM_PK_Q] = Key::Q;
        mapKeys[DOM_PK_R] = Key::R;
        mapKeys[DOM_PK_S] = Key::S;
        mapKeys[DOM_PK_T] = Key::T;
        mapKeys[DOM_PK_U] = Key::U;
        mapKeys[DOM_PK_V] = Key::V;
        mapKeys[DOM_PK_W] = Key::W;
        mapKeys[DOM_PK_X] = Key::X;
        mapKeys[DOM_PK_Y] = Key::Y;
        mapKeys[DOM_PK_Z] = Key::Z;
        
        // Numeric Keys
        mapKeys[DOM_PK_0] = Key::K0;
        mapKeys[DOM_PK_1] = Key::K1;
        mapKeys[DOM_PK_2] = Key::K2;
        mapKeys[DOM_PK_3] = Key::K3;
        mapKeys[DOM_PK_4] = Key::K4;
        mapKeys[DOM_PK_5] = Key::K5;
        mapKeys[DOM_PK_6] = Key::K6;
        mapKeys[DOM_PK_7] = Key::K7;
        mapKeys[DOM_PK_8] = Key::K8;
        mapKeys[DOM_PK_9] = Key::K9;
        
        // Function Keys
        mapKeys[DOM_PK_F1] = Key::F1;
        mapKeys[DOM_PK_F2] = Key::F2;
        mapKeys[DOM_PK_F3] = Key::F3;
        mapKeys[DOM_PK_F4] = Key::F4;
        mapKeys[DOM_PK_F5] = Key::F5;
        mapKeys[DOM_PK_F6] = Key::F6;
        mapKeys[DOM_PK_F7] = Key::F7;
        mapKeys[DOM_PK_F8] = Key::F8;
        mapKeys[DOM_PK_F9] = Key::F9;
        mapKeys[DOM_PK_F10] = Key::F10;
        mapKeys[DOM_PK_F11] = Key::F11;
        mapKeys[DOM_PK_F12] = Key::F12;
        
        // Arrow Keys
        mapKeys[DOM_PK_ARROW_DOWN] = Key::DOWN;
        mapKeys[DOM_PK_ARROW_LEFT] = Key::LEFT;
        mapKeys[DOM_PK_ARROW_RIGHT] = Key::RIGHT;
        mapKeys[DOM_PK_ARROW_UP] = Key::UP;

        // Other Keys
        mapKeys[DOM_PK_BACKSPACE] = Key::BACK;
        mapKeys[DOM_PK_ESCAPE] = Key::ESCAPE;
        mapKeys[DOM_PK_ENTER] = Key::ENTER;
        mapKeys[DOM_PK_PAUSE] = Key::PAUSE;
        mapKeys[DOM_PK_SCROLL_LOCK] = Key::SCROLL;
        mapKeys[DOM_PK_TAB] = Key::TAB;
        mapKeys[DOM_PK_DELETE] = Key::DEL;
        mapKeys[DOM_PK_HOME] = Key::HOME;
        mapKeys[DOM_PK_END] = Key::END;
        mapKeys[DOM_PK_PAGE_UP] = Key::PGUP;
        mapKeys[DOM_PK_PAGE_DOWN] = Key::PGDN;
        mapKeys[DOM_PK_INSERT] = Key::INS;
        mapKeys[DOM_PK_SHIFT_LEFT] = Key::SHIFT;
        mapKeys[DOM_PK_SHIFT_RIGHT] = Key::SHIFT;
        mapKeys[DOM_PK_CONTROL_LEFT] = Key::CTRL;
        mapKeys[DOM_PK_CONTROL_RIGHT] = Key::CTRL;
        mapKeys[DOM_PK_SPACE] = Key::SPACE;
        mapKeys[DOM_PK_CAPS_LOCK] = Key::CAPS_LOCK;

        // Numpad
        mapKeys[DOM_PK_NUMPAD_0] = Key::NP0;
        mapKeys[DOM_PK_NUMPAD_1] = Key::NP1;
        mapKeys[DOM_PK_NUMPAD_2] = Key::NP2;
        mapKeys[DOM_PK_NUMPAD_3] = Key::NP3;
        mapKeys[DOM_PK_NUMPAD_4] = Key::NP4;
        mapKeys[DOM_PK_NUMPAD_5] = Key::NP5;
        mapKeys[DOM_PK_NUMPAD_6] = Key::NP6;
        mapKeys[DOM_PK_NUMPAD_7] = Key::NP7;
        mapKeys[DOM_PK_NUMPAD_8] = Key::NP8;
        mapKeys[DOM_PK_NUMPAD_9] = Key::NP9;
        mapKeys[DOM_PK_NUMPAD_MULTIPLY] = Key::NP_MUL;
        mapKeys[DOM_PK_NUMPAD_DIVIDE] = Key::NP_DIV;
        mapKeys[DOM_PK_NUMPAD_ADD] = Key::NP_ADD;
        mapKeys[DOM_PK_NUMPAD_SUBTRACT] = Key::NP_SUB;
        mapKeys[DOM_PK_NUMPAD_DECIMAL] = Key::NP_DECIMAL;
        mapKeys[DOM_PK_NUMPAD_EQUAL] = Key::EQUALS;
        mapKeys[DOM_PK_NUMPAD_ENTER] = Key::ENTER;
        
        mapKeys[DOM_PK_SEMICOLON] = Key::OEM_1;
        mapKeys[DOM_PK_SLASH] = Key::OEM_2;
        mapKeys[DOM_PK_BACKQUOTE] = Key::OEM_3;
        mapKeys[DOM_PK_BRACKET_LEFT] = Key::OEM_4;
        mapKeys[DOM_PK_BACKSLASH] = Key::OEM_5;
        mapKeys[DOM_PK_BRACKET_RIGHT] = Key::OEM_6;
        mapKeys[DOM_PK_QUOTE] = Key::OEM_7;
        
        mapKeys[DOM_PK_EQUAL] = Key::EQUALS;
        mapKeys[DOM_PK_COMMA] = Key::COMMA;
        mapKeys[DOM_PK_MINUS] = Key::MINUS;
        mapKeys[DOM_PK_PERIOD] = Key::PERIOD;
    }

    bool Host_Web_Emscripten::StartSystemEventLoop(bool bBlockIfPossible)
    {
        std::cout << "Emscripten: StartSystemEventLoop called, but not used.\n";
        return true;
    }

    bool Host_Web_Emscripten::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        std::cout << "Emscripten: AddWindowFrame called.\n";

        // The user created olc::Window object is the SSoT for what a window
		// should look like, so get that sort of thing from there
		olc::vi2d vWinPos = vWindowPos;
		olc::vi2d vWinSize = vWindowSize;
        
        // TODO: multi-window solutions
        mapUID2CallbackData.insert_or_assign(pWindow->GetUID(), std::make_unique<CallbackData>(CallbackData{
            this,
            pWindow,
            std::string{"#canvas"}
        }));
        
        auto cbData = mapUID2CallbackData.at(pWindow->GetUID()).get();

        mapUID2CanvasId.insert_or_assign(pWindow->GetUID(), cbData->canvasId);
        mapCanvasId2PTR.insert_or_assign(cbData->canvasId, pWindow);
        
        emscripten_set_canvas_element_size(cbData->canvasId.c_str(), vWinSize.x, vWinSize.y);
        pWindow->SetWindowSize(vWinSize);

        // Keyboard Callbacks
        emscripten_set_keydown_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, keyboard_callback);
        emscripten_set_keyup_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, keyboard_callback);

        // Mouse Callbacks
        emscripten_set_wheel_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, wheel_callback);
        emscripten_set_mousedown_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, mouse_callback);
        emscripten_set_mouseup_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, mouse_callback);
        emscripten_set_mousemove_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, mouse_callback);

        // Touch Callbacks
        emscripten_set_touchstart_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, touch_callback);
        emscripten_set_touchmove_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, touch_callback);
        emscripten_set_touchend_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, touch_callback);

        // Canvas Focus Callbacks
        emscripten_set_blur_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, focus_callback);
        emscripten_set_focus_callback(cbData->canvasId.c_str(), reinterpret_cast<void*>(cbData), 1, focus_callback);

        // Canvas Resize Callbacks
        emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, reinterpret_cast<void*>(cbData), 1, resize_callback);
        emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, reinterpret_cast<void*>(cbData), 1, fullscreen_change_callback);

        // trigger resize after a short pause
        emscripten_sleep(50);
        resize_callback(EMSCRIPTEN_EVENT_RESIZE, nullptr, reinterpret_cast<void*>(cbData));

        return true;
    }

    //TY Moros
    EM_BOOL Host_Web_Emscripten::keyboard_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
    {
        CallbackData* pCallbackData = reinterpret_cast<CallbackData*>(userData);

        // we maintain our own state for teh number pad, default true
        static bool numPadActive = true;
        
        // THANK GOD!! for this compute function. And thanks Dandistine for pointing it out!
        int pk_code = emscripten_compute_dom_pk_code(e->code);
        
        if(!numPadActive)
        {
            /**
             * we need to react differently if the numlock is not
             * active. this block ensures uniform behavior with
             * windows and linux, MacOS is a lost cause due to GLUT.
             */
            switch(pk_code)
            {
                case DOM_PK_NUMPAD_7: pk_code = DOM_PK_HOME; break;
                case DOM_PK_NUMPAD_8: pk_code = DOM_PK_ARROW_UP; break;
                case DOM_PK_NUMPAD_9: pk_code = DOM_PK_PAGE_UP; break;
                case DOM_PK_NUMPAD_4: pk_code = DOM_PK_ARROW_LEFT; break;
                case DOM_PK_NUMPAD_5: pk_code = DOM_PK_UNKNOWN; break;
                case DOM_PK_NUMPAD_6: pk_code = DOM_PK_ARROW_RIGHT; break;
                case DOM_PK_NUMPAD_1: pk_code = DOM_PK_END; break;
                case DOM_PK_NUMPAD_2: pk_code = DOM_PK_ARROW_DOWN; break;
                case DOM_PK_NUMPAD_3: pk_code = DOM_PK_PAGE_DOWN; break;
                case DOM_PK_NUMPAD_0: pk_code = DOM_PK_INSERT; break;
                case DOM_PK_NUMPAD_DECIMAL: pk_code = DOM_PK_DELETE; break;
                default:
                    break;
            }
        }

        // check for keydown + numlock and act appropriately
        if (eventType == EMSCRIPTEN_EVENT_KEYDOWN && pk_code == DOM_PK_NUM_LOCK)
        {
            numPadActive = !numPadActive;
        }

        if (eventType == EMSCRIPTEN_EVENT_KEYDOWN)
        {
            if(pCallbackData->pHost->mapKeys.contains(pk_code))
            {
                olc_OnKeyPress(pCallbackData->pWindow, pCallbackData->pHost->mapKeys[pk_code], true);
            }
        }
            

        if (eventType == EMSCRIPTEN_EVENT_KEYUP)
        {
            if(pCallbackData->pHost->mapKeys.contains(pk_code))
            {
                olc_OnKeyPress(pCallbackData->pWindow, pCallbackData->pHost->mapKeys[pk_code], false);
            }
        }
            

        //Consume keyboard events so that keys like F1 and F5 don't do weird things
        return EM_TRUE;
    }

    //TY Moros
    EM_BOOL Host_Web_Emscripten::wheel_callback(int eventType, const EmscriptenWheelEvent* e, void* userData)
    {
        CallbackData* pCallbackData = reinterpret_cast<CallbackData*>(userData);
        
        if (eventType == EMSCRIPTEN_EVENT_WHEEL)
            olc_OnMouseWheel(pCallbackData->pWindow, -1 * e->deltaY);
    
        return EM_TRUE;
    }

    olc::Window* Host_Web_Emscripten::GetWindowFromCanvasId(std::string canvasId)
    {
        auto itr = mapCanvasId2PTR.find(canvasId);
        if(itr != mapCanvasId2PTR.end())
        {
            return itr->second;
        }

        throw std::runtime_error("failed to get window for canvas id: " + canvasId);
        return nullptr;
    }

    //TY Moros
    EM_BOOL Host_Web_Emscripten::mouse_callback(int eventType, const EmscriptenMouseEvent* e, void* userData)
    {
        CallbackData* pCallbackData = reinterpret_cast<CallbackData*>(userData);
        
        //Mouse Movement
        if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
            olc_OnMouseMove(pCallbackData->pWindow, {e->targetX, e->targetY});


        //Mouse button press
        if (e->button == 0) // left click
        {
            if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
                olc_OnMouseButton(pCallbackData->pWindow, 0, true);
            else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP)
                olc_OnMouseButton(pCallbackData->pWindow, 0, false);
        }

        if (e->button == 2) // right click
        {
            if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
                olc_OnMouseButton(pCallbackData->pWindow, 1, true);
            else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP)
                olc_OnMouseButton(pCallbackData->pWindow, 1, false);    
        }

        if (e->button == 1) // middle click
        {
            if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
                olc_OnMouseButton(pCallbackData->pWindow, 2, true);
            else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP)
                olc_OnMouseButton(pCallbackData->pWindow, 2, false);

            //at the moment only middle mouse needs to consume events.
            return EM_TRUE;
        }

        return EM_FALSE;
    }

    //TY Bispoo
    EM_BOOL Host_Web_Emscripten::touch_callback(int eventType, const EmscriptenTouchEvent* e, void* userData)
    {
        // TODO: Implement touch more effectively.
        //       For now, emulate single pointer mouse.
        
        CallbackData* pCallbackData = reinterpret_cast<CallbackData*>(userData);
        
        // Move
        if (eventType == EMSCRIPTEN_EVENT_TOUCHMOVE)
        {
            olc_OnMouseMove(pCallbackData->pWindow, {e->touches->targetX, e->touches->targetY});
        }

        // Start
        if (eventType == EMSCRIPTEN_EVENT_TOUCHSTART)
        {
            olc_OnMouseMove(pCallbackData->pWindow, {e->touches->targetX, e->touches->targetY});
            olc_OnMouseButton(pCallbackData->pWindow, 0, true);
        }

        // End
        if (eventType == EMSCRIPTEN_EVENT_TOUCHEND)
        {
            olc_OnMouseButton(pCallbackData->pWindow, 0, false);
        }

        return EM_TRUE;
    }
    //TY Gorbit
    EM_BOOL Host_Web_Emscripten::focus_callback(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData)
    {
        CallbackData* pCallbackData = reinterpret_cast<CallbackData*>(userData);
 
        if (eventType == EMSCRIPTEN_EVENT_BLUR)
        {
            // ptrPGE->olc_UpdateKeyFocus(false);
            olc_OnMouseFocus(pCallbackData->pWindow, false);
        }
        else if (eventType == EMSCRIPTEN_EVENT_FOCUS)
        {
            // ptrPGE->olc_UpdateKeyFocus(true);
            olc_OnMouseFocus(pCallbackData->pWindow, true);
        }

        return 0;
    }

    //TY Moros
    EM_BOOL Host_Web_Emscripten::fullscreen_change_callback(int eventType, const EmscriptenFullscreenChangeEvent *event, void *userData)
    {
        // trigger resize after a short pause
        emscripten_sleep(50);
        resize_callback(EMSCRIPTEN_EVENT_RESIZE, nullptr, userData);
        return 0;
    }
		
    //TY Moros
    EM_BOOL Host_Web_Emscripten::resize_callback(int eventType, const EmscriptenUiEvent *event, void *userData)
    {
        CallbackData* pCallbackData = reinterpret_cast<CallbackData*>(userData);

        // HACK ALERT!
        // 
        // Here we assume any html shell that uses 3 or more instance of the class "emscripten"
        // is using one of the default or minimal emscripten page layouts
        static bool assumeDefaultShell = EM_ASM_INT( return (document.querySelectorAll('.emscripten').length >= 3) ? 1 : 0; );
        static bool firstTry = false;

        // we to apply this style once
        if(!firstTry && assumeDefaultShell)
        {
            EM_ASM({ Module.canvas.parentNode.setAttribute('style', 'width: 100%; height: 70vh; margin-left: auto; margin-right: auto;'); });
            firstTry = true;
        }

        // get and keep the aspect ratio of the canvas
        static double aspect = EM_ASM_DOUBLE( return Module.canvas.clientWidth; ) / EM_ASM_DOUBLE( return Module.canvas.clientHeight; );

        double parentWidth = EM_ASM_DOUBLE( return (!!document.fullscreenElement) ? window.innerWidth : Module.canvas.parentElement.clientWidth; );
        double parentHeight = EM_ASM_DOUBLE( return (!!document.fullscreenElement) ? window.innerHeight : Module.canvas.parentElement.clientHeight; );

        double width = parentWidth;
        double height = parentWidth / aspect;

        if (height > parentHeight)
        {
            height = parentHeight;
            width = height * aspect;
        }
        
        // resize the canvas
        emscripten_set_canvas_element_size(pCallbackData->canvasId.c_str(), static_cast<int>(width), static_cast<int>(height));
        pCallbackData->pWindow->SetWindowSize(olc::vd2d{width, height});
        return 0;
    }

    bool Host_Web_Emscripten::CloseWindowFrame(olc::Window* pWindow)
    {
        std::cout << "Emscripten: CloseWindowFrame not implemented.\n";
        return true;
    }

    bool Host_Web_Emscripten::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        // not implemented for emscripten platform
        return true;
    }

    std::vector<void*> Host_Web_Emscripten::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2CanvasId.find(pWindow->GetUID());
        if(window_handle != mapUID2CanvasId.end())
        {
            return { (void*)(window_handle->second.c_str()) };
        }
        
        return {};
    }

    bool Host_Web_Emscripten::ConnectHostResourceToRenderer()
    {
        std::cout << "Emscripten: ConnectHostResourceToRenderer not implemented.\n";
        return true;
    }

    // Wait for entire host desktop refresh (for smooooth vsync)
    bool Host_Web_Emscripten::SyncWithDesktopComposite()
    {
        // SyncWithDesktopComposite not implemented on this platform
        return true;
    }
	
    olc::KeyboardLayout Host_Web_Emscripten::GetKeyboardLayout() const
	{
		return static_cast<olc::KeyboardLayout>(EM_ASM_INT({ return Module.keyboardLayout || 0; }));
    }

    bool Host_Web_Emscripten::olc_OnMouseButton(olc::Window* pWindow, const uint8_t nButton, const bool bPressed)
    {
        return pWindow->olc_OnMouseButton(nButton, bPressed);
    }

    bool Host_Web_Emscripten::olc_OnMouseMove(olc::Window* pWindow, const olc::vi2d& vMousePos)
    {
        return pWindow->olc_OnMouseMove(vMousePos);
    }

    bool Host_Web_Emscripten::olc_OnMouseWheel(olc::Window* pWindow, const int32_t nScroll)
    {
        return pWindow->olc_OnMouseWheel(nScroll);
    }

    bool Host_Web_Emscripten::olc_OnMouseFocus(olc::Window* pWindow, const bool bHasFocus)
    {
        return pWindow->olc_OnMouseFocus(bHasFocus);
    }

    bool Host_Web_Emscripten::olc_OnKeyPress(olc::Window* pWindow, const olc::Key key, const bool bPressed)
    {
        return pWindow->olc_OnKeyPress(key, bPressed);
    }

    bool Host_Web_Emscripten::olc_OnWindowPosition(olc::Window* pWindow, const olc::vi2d& vWindowPos)
    {
        return pWindow->olc_OnWindowPosition(vWindowPos);
    }

    bool Host_Web_Emscripten::olc_OnWindowSize(olc::Window* pWindow, const olc::vi2d& vWindowSize)
    {
        return pWindow->olc_OnWindowSize(vWindowSize);
    }

    bool Host_Web_Emscripten::olc_OnWindowClose(olc::Window* pWindow)
    {
        return pWindow->olc_OnWindowClose();
    }


}
//! END IMPLEMENTATION
