#include "config.h"
#include "host_apple_macos.h"
#include "core.h"
#include <dispatch/queue.h>
#if OLC_HOST == OLC_HOST_MACOS

//! START IMPLEMENTATION
namespace olc::host {


    // NSEventModifierFlags values
    // constexpr unsigned int NSEventModifierNoFlags        = 1 << 8;  // 0x100     // Temp remove unused variable warning
    // constexpr unsigned int NSEventModifierFlagCapsLock   = 1 << 16; // 0x10000   // Temp remove unused variable warning
    constexpr unsigned int NSEventModifierFlagShift      = 1 << 17; // 0x20000
    constexpr unsigned int NSEventModifierFlagControl    = 1 << 18; // 0x40000
    // constexpr unsigned int NSEventModifierFlagOption     = 1 << 19; // 0x80000   // Temp remove unused variable warning
    constexpr unsigned int NSEventModifierFlagCommand    = 1 << 20; // 0x100000
    // constexpr unsigned int NSEventModifierFlagNumericPad = 1 << 21; // 0x200000  // Temp remove unused variable warning
    // constexpr unsigned int NSEventModifierFlagHelp       = 1 << 22; // 0x400000  // Temp remove unused variable warning
    // constexpr unsigned int NSEventModifierFlagFunction   = 1 << 23; // 0x800000  // Temp remove unused variable warning

    // enum for window appearance and behavior bit flags
    enum class NSWindowStyleMask : uint16_t {
        Titled                   = (1 << 0),     // Window has a title bar
        Closable                 = (1 << 1),     // Window can be closed
        Miniaturizable           = (1 << 2),     // Window can be minimized
        Resizable                = (1 << 3),     // Window can be resized
        UtilityWindow            = (1 << 4),     // Utility window style
        DocModalWindow           = (1 << 6),     // Document-modal window
        NonactivatingPanel       = (1 << 7),     // Non-activating panel
        TexturedBackground       = (1 << 8),     // Textured background
        HUDWindow                = (1 << 13),    // Heads-up display window
        UnifiedTitleAndToolbar   = (1 << 12),    // Unified title and toolbar
        FullScreen               = (1 << 14),    // Full-screen window
        FullSizeContentView      = (1 << 15)     // Full-size content view
    };


    Host_Apple_MacOS::Host_Apple_MacOS()
    {
         // Reference: https://eastmanreference.com/complete-list-of-applescript-key-codes
        mapKeys[0x00] = Key::NONE;

        // Map macOS key codes to olc::Key codes
        mapKeys[0] = Key::A;
        mapKeys[11] = Key::B;
        mapKeys[8] = Key::C;
        mapKeys[2] = Key::D;
        mapKeys[14] = Key::E;
        mapKeys[3] = Key::F;
        mapKeys[5] = Key::G;
        mapKeys[4] = Key::H;
        mapKeys[34] = Key::I;
        mapKeys[38] = Key::J;
        mapKeys[40] = Key::K;
        mapKeys[37] = Key::L;
        mapKeys[46] = Key::M;
        mapKeys[45] = Key::N;
        mapKeys[31] = Key::O;
        mapKeys[35] = Key::P;
        mapKeys[12] = Key::Q;
        mapKeys[15] = Key::R;
        mapKeys[1] = Key::S;
        mapKeys[17] = Key::T;
        mapKeys[32] = Key::U;
        mapKeys[9] = Key::V;
        mapKeys[13] = Key::W;
        mapKeys[7] = Key::X;
        mapKeys[16] = Key::Y;
        mapKeys[6] = Key::Z;

        // Numeric keys
        mapKeys[29] = Key::K0;
        mapKeys[18] = Key::K1;
        mapKeys[19] = Key::K2;
        mapKeys[20] = Key::K3;
        mapKeys[21] = Key::K4;
        mapKeys[23] = Key::K5;
        mapKeys[22] = Key::K6;
        mapKeys[26] = Key::K7;
        mapKeys[28] = Key::K8;
        mapKeys[25] = Key::K9;

        // Function Keys
        mapKeys[122] = Key::F1;
        mapKeys[120] = Key::F2;
        mapKeys[99] = Key::F3;
        mapKeys[118] = Key::F4;
        mapKeys[96] = Key::F5;
        mapKeys[97] = Key::F6;
        mapKeys[98] = Key::F7;
        mapKeys[100] = Key::F8;
        mapKeys[101] = Key::F9;
        mapKeys[109] = Key::F10;
        mapKeys[103] = Key::F11;
        mapKeys[111] = Key::F12;

        // Arrow Keys
        mapKeys[125] = Key::DOWN; 
        mapKeys[123] = Key::LEFT;
        mapKeys[124] = Key::RIGHT;
        mapKeys[126] = Key::UP;

        // Other Keys
        mapKeys[51] = Key::BACK;        // Delete (Backspace)
        mapKeys[53] = Key::ESCAPE;      // Escape
        mapKeys[36] = Key::ENTER;       // Return
        mapKeys[113] = Key::PAUSE;      // F16 (often used as pause)
        mapKeys[107] = Key::SCROLL;     // F14 (scroll lock equivalent)
        mapKeys[48] = Key::TAB;         // Tab
        mapKeys[117] = Key::DEL;        // Forward Delete
        mapKeys[115] = Key::HOME;       // Home
        mapKeys[119] = Key::END;        // End
        mapKeys[116] = Key::PGUP;       // Page Up
        mapKeys[121] = Key::PGDN;       // Page Down
        mapKeys[114] = Key::INS;        // Help (Insert equivalent)
        mapKeys[56] = Key::SHIFT;       // Left Shift
        mapKeys[59] = Key::CTRL;        // Left Control
        mapKeys[49] = Key::SPACE;       // Space
        mapKeys[57] = Key::CAPS_LOCK;   // Caps Lock

        // Numpad
        mapKeys[82] = Key::NP0;
        mapKeys[83] = Key::NP1;
        mapKeys[84] = Key::NP2;
        mapKeys[85] = Key::NP3;
        mapKeys[86] = Key::NP4;
        mapKeys[87] = Key::NP5;
        mapKeys[88] = Key::NP6;
        mapKeys[89] = Key::NP7;
        mapKeys[91] = Key::NP8;
        mapKeys[92] = Key::NP9;
        mapKeys[67] = Key::NP_MUL;      // Numpad *
        mapKeys[69] = Key::NP_ADD;      // Numpad +
        mapKeys[75] = Key::NP_DIV;      // Numpad /
        mapKeys[78] = Key::NP_SUB;      // Numpad -
        mapKeys[65] = Key::NP_DECIMAL;  // Numpad .

        // Symbol Keys (OEM equivalents)
        mapKeys[41] = Key::OEM_1;       // On US and UK keyboards this is the ';:' key
        mapKeys[44] = Key::OEM_2;       // On US and UK keyboards this is the '/?' key
        mapKeys[50] = Key::OEM_3;       // On US and UK keyboards this is the '`~' key (Grave accent `)
        mapKeys[33] = Key::OEM_4;       // On US and UK keyboards this is the '[{' key
        mapKeys[42] = Key::OEM_5;       // On US keyboard this is '\|' key. 
        mapKeys[30] = Key::OEM_6;       // On US and UK keyboards this is the ']}' key
        mapKeys[39] = Key::OEM_7;       // On US keyboard this is the single/double quote key. On UK, this is the single quote/@ symbol key
        mapKeys[10] = Key::OEM_8;       // Section sign § (varies by keyboard)
        mapKeys[24] = Key::EQUALS;      // Equal sign =
        mapKeys[43] = Key::COMMA;       // Comma ,
        mapKeys[27] = Key::MINUS;       // Minus -
        mapKeys[47] = Key::PERIOD;      // Period .

    }


    bool Host_Apple_MacOS::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen){
        olc_IgnoreUnused(bFullScreen);
        pPGEwindow = pWindow;
        pPGEwindow->SetWindowPosition(vWindowPos);
        pPGEwindow->SetWindowSize(vWindowSize);
        pPGEwindow->LinkToHost(this);

        frameBounds.x = 0.0;
        frameBounds.y = 0.0;
        frameBounds.width = static_cast<double>(vWindowSize.x);
        frameBounds.height = static_cast<double>(vWindowSize.y);
        
        return true;
    }

    bool Host_Apple_MacOS::CloseWindowFrame(olc::Window* pWindow){
        pWindow->olc_OnWindowClose();
        return true;
    }

    bool Host_Apple_MacOS::UpdateWindowFrameTitle(olc::Window* pWindow){
        if (!pMacOSWindow) return false;
        dispatch_async(dispatch_get_main_queue(), ^{
            pMacOSWindow->setTitle(pWindow->GetWindowTitle().c_str());
        });
        return true;
    }

    std::vector<void*> Host_Apple_MacOS::GetHostWindowDescriptor(olc::Window* pWindow){
        olc_IgnoreUnused(pWindow);
        // Ensure OpenGL renderer is created
        if(pMacOSOpenGLRenderer == nullptr)
            CreateCGLContextObj();

        return vMacOSWindowDescriptors;
       
    }


    bool Host_Apple_MacOS::SyncWithDesktopComposite()
    {
        /*
         core.h SyncWithDesktopComposite is only called when vSync is enabled on each frame,
         the method of enabling vSync varies between platforms, For macos we use a local var enableVSync,
         set to false and toggle it on first call, so that vSync is only enabled once
         */
        
        if(!enableVSync)
        {
            pMacOSOpenGLRenderer->enableVsync();
            enableVSync = true;
        }
        
        return enableVSync;
    }

    bool Host_Apple_MacOS::SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos)
    {
        olc_IgnoreUnused(pWindow);
        vPositionMouse = vPos;
        this->vPendingMainThreadTasks.push_back(SET_MOUSE_POSITION);
        return false;
    }

    bool Host_Apple_MacOS::SetMouseVisible(olc::Window* pWindow, const bool bVisible)
    {
        olc_IgnoreUnused(pWindow);
        dispatch_async(dispatch_get_main_queue(), ^{
            pMacOSWindow->setCursorVisibility(bVisible);
        });
        return true;
    }

    bool Host_Apple_MacOS::SetFullScreen(olc::Window* pWindow, const bool bFullScreen)
    {
        olc_IgnoreUnused(pWindow);
        // if we're already in the specified state, return early
        if(pMacOSWindow->isFullScreen() == bFullScreen)
            return true;

        dispatch_async(dispatch_get_main_queue(), ^{
            pMacOSWindow->toggleFullScreen();
        });
        return true;
    }

    uint16_t Host_Apple_MacOS::ConvertPGE2WindowStyle()
    {
        uint16_t nsStyle = 0;
        
        // Note for MacOS: You cannot fully hide both the title bar and border, therefore we return titled when both are disabled, which is the closest we can get to a borderless window
        if (!pPrimaryPGE->config.bShowWindowBorder || !pPrimaryPGE->config.bShowWindowTilebar) return static_cast<unsigned int>(NSWindowStyleMask::Titled);

        // On MacOS, the maximize button is tied to the resizable style, therefore there is no need to implemenent a separate bShowWindowMaximiseButton config,
        // For MacOS you can only disable the buttons, you can't hide them
        if (pPrimaryPGE->config.bFullScreen)               nsStyle |= static_cast<unsigned int>(NSWindowStyleMask::FullSizeContentView);      // Fullscreen window
        if (pPrimaryPGE->config.bShowWindowTilebar)        nsStyle |= static_cast<unsigned int>(NSWindowStyleMask::Titled);          // Add a title bar
        if (pPrimaryPGE->config.bShowWindowBorder)         nsStyle |= static_cast<unsigned int>(NSWindowStyleMask::Titled);          // Add a border
        if (pPrimaryPGE->config.bResizeable)               nsStyle |= static_cast<unsigned int>(NSWindowStyleMask::Resizable);       // Enable resizing
        if (pPrimaryPGE->config.bShowWindowMinimiseButton) nsStyle |= static_cast<unsigned int>(NSWindowStyleMask::Miniaturizable);  // Add Min Button
        if (pPrimaryPGE->config.bShowWindowCloseButton)    nsStyle |= static_cast<unsigned int>(NSWindowStyleMask::Closable);        // Add Close Button

        return nsStyle;
        
    }

    bool Host_Apple_MacOS::OnApplicationStart(olc::PixelGameEngine* pPrimary){
        pPrimaryPGE = pPrimary;
        return true;
    }

    bool Host_Apple_MacOS::StartSystem(){
                
        // Create MacOS Application instance
        pMacApplication = std::make_unique<olc::apis::macos::Application>();

        // Set up application delegate event handlers
        MacApplicationEventsHandler();

        // Initialize and activate application first
        pMacApplication->initialize();
        pMacApplication->activate();
        
        // Pre-context start hook
        pPrimaryPGE->OnPreContextStart();
        
        // Initialize the MacOS Window
        pMacOSWindow = std::make_unique<olc::apis::macos::Window>(frameBounds.width, frameBounds.height, "OLC PGE 3 MacOS Demo");
        pMacOSWindow->setPosition(frameBounds.x, frameBounds.y);
        pMacOSWindow->setContentViewPosition(0, 0);
        
        // Set up window event handlers
        MacWindowEventsHandler();
        
        // Create Input Event handler
        pMacOSEventHandler = std::make_unique<olc::apis::macos::EventHandler>(*pMacOSWindow);
            
        // Setup Event handlers
        MacEventsHandler();
        
        // Create the window
        unsigned long styleMask = ConvertPGE2WindowStyle();
        pMacOSWindow->show(styleMask);
        pMacOSEventHandler->enable();
        
        //--- Start up our engine threading system ----
        // Start the PGE context on the main thread
        // Mark system as active
        systemActive = true;

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

        
        // Start the main event loop (this will block)
        pMacApplication->run();
                
        // Once the application run loop ends, join the system thread
        if(threadSystem.joinable())
            threadSystem.join();

        // Post-context end hook
        return pPrimaryPGE->OnPostContextEnd();

    }

    bool Host_Apple_MacOS::StopSystem()
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            // clean up and close application
            if (pMacOSOpenGLRenderer)
            {
                pMacOSOpenGLRenderer->destoryContext();
                pMacOSOpenGLRenderer = nullptr;
            }
            if (pMacOSWindow)
            {
                pMacOSWindow->destoryWindow();
                pMacOSWindow = nullptr;
            }
            if (pMacApplication)
            {
                pMacApplication->stop();
            }

        });
        systemActive = false;
        return true;
    }

    bool Host_Apple_MacOS::OnSystemThreadStart()
    {
        // Hold back threading until application is fully initialized
        bSkipFrame = ExecutePendingMainThreadTasks();
        return pPrimaryPGE->OnContextStart();
    }

    bool Host_Apple_MacOS::OnSystemTick()
    {
        // Execute any pending main thread tasks
        bSkipFrame = ExecutePendingMainThreadTasks();
        return pPrimaryPGE->OnContextTick();
    }

    bool Host_Apple_MacOS::OnSystemThreadEnd()
    {
        return pPrimaryPGE->OnContextEnd();
    }

    bool Host_Apple_MacOS::OnApplicationEnd()
    {
        return true;
    }


//-- OS Window Event Handling -----
   
    olc::KeyboardLayout Host_Apple_MacOS::GetKeyboardLayout() const
    {
        // Get system locale from MacOS Application
        // We need to wait until the application has launched to get the keyboard layout
        // Therefore this function is called again from setDidFinishLaunchingCallback event
        if (pMacApplication)
        {
            std::string locale = pMacApplication->getSystemLocale();
            if (locale == "en_GB")
            {
                return olc::KeyboardLayout::QWERTY_UK;
            }
            else if (locale == "en_US")
            {
                return olc::KeyboardLayout::QWERTY_US;
            }
            else if (locale == "fr_FR")
            {
                return olc::KeyboardLayout::AZERTY;
            }
            else if (locale == "de_DE")
            {
                return olc::KeyboardLayout::QWERTZ;
            }
        }
        // Default to QWERTY if unknown
        return olc::KeyboardLayout::QWERTY_UK;
    }   

// ------- Priavate Main Thread Task Handling for MacOS Host -------

    bool Host_Apple_MacOS::CreateCGLContextObj()
    {
        // This method should only be called on the PGE thread, use AddPendingMainThreadTask(CREATE_OPENGL_RENDERER); to queue it if needed
        if(pMacOSOpenGLRenderer == nullptr)
        {
           vMacOSWindowDescriptors.clear(); // ensure we are starting fresh
           pMacOSOpenGLRenderer = std::make_shared<olc::apis::macos::OpenGLRenderer>();
           
           dispatch_sync(dispatch_get_main_queue(), ^{
                // Edge case for when the window is auto resize due to MacOS clamping to screen size
               pMacOSWindow->getContentViewSize(frameBounds.width, frameBounds.height);
               pPGEwindow->olc_OnWindowSize({static_cast<int>(frameBounds.width), static_cast<int>(frameBounds.height)});

               pMacOSOpenGLRenderer->attachToWindow(*pMacOSWindow);
               pMacOSOpenGLRenderer->setupContext();
           });
           
           pMacGLConextObj = pMacOSOpenGLRenderer->getCGLContextObj();
           
           pMacOSOpenGLRenderer->setVsync(false);
           
           vMacOSWindowDescriptors.push_back(pMacGLConextObj); // Pointer to CGLContextObj
           vMacOSWindowDescriptors.push_back(&bSkipFrame);     // Pointer to skip frame flag

            // Set up OpenGL renderer for visual feedback
           pMacOSOpenGLRenderer->makeCurrentContext();
            
            // Finally we set full screen if needed to ensure all out OpenGL setup is done before toggling full screen,
            if(pPrimaryPGE->config.bFullScreen){
                SetFullScreen(pPGEwindow, true);
            }
           
        }
        
        return true;
    }

    bool Host_Apple_MacOS::ExecutePendingMainThreadTasks()
    {
        // 1: Check if main thread wants us to wait
        std::unique_lock<std::mutex> lock(pgeThreadPendingTasksMutex);
        
        if (isPGEThreadResetting.load()) {
            
            // 2. PGE Thread signals it's waiting
            {
                std::lock_guard<std::mutex> mainLock(mainThreadPendingTasksMutex);
                isMainThreadResetting = true;  // Signal to main thread we're waiting
            }
            mainThreadResetCondition.notify_all();  // Wake up main thread

            // Note: MainThreadTasks(); will be called by the main thread to process tasks
            
            // 3. PGE Thread waits for main thread to finish
            pgeThreadResetCondition.wait(lock, [this] { 
                return !isPGEThreadResetting.load(); 
            });

            //4: return true indicating we processed tasks
            return true;
        }
        else
        {
            // No pending tasks, just return
            return false;
        }
    }

    bool Host_Apple_MacOS::AddPendingMainThreadTask(MAINTASKS task)
    {
        // NOTE: Note: You should only add tasks that require main thread execution
        vPendingMainThreadTasks.push_back(task);
        MainThreadTasks();
            
        return true;
    }
    
    bool Host_Apple_MacOS::MainThreadTasks()
    {
        bool res = false;
        if(vPendingMainThreadTasks.empty())
            return res;         // edge case
        
        // 1. Main Thread locks PGE Thread
        {
            std::lock_guard<std::mutex> lock(pgeThreadPendingTasksMutex);
            isPGEThreadResetting = true;  // Signal PGE to stop
        }
        pgeThreadResetCondition.notify_all();  // Wake up PGE thread to check flag
        
        // 2. Main Thread waits for PGE Thread to acknowledge and wait
        std::unique_lock<std::mutex> lock(mainThreadPendingTasksMutex);
        mainThreadResetCondition.wait(lock, [this] {
            return isMainThreadResetting.load(); // Wait until PGE signals it's waiting
        });
        
        // Process any pending main thread tasks
        for (const auto& task : vPendingMainThreadTasks)
        {
            switch (task)
            {
                case CREATE_OPENGL_RENDERER:
                {
                    // In this case, the PGE will be waiting for main thread to singal, so the ContextOBJ can be created
                    res = false; // No need to skip frame
                    break;
                }
                case RESIZE_WINDOW:
                {
                    // Resize window on main thread
                    pMacOSWindow->getContentViewSize(frameBounds.width, frameBounds.height);
                    pPGEwindow->olc_OnWindowSize({static_cast<int>(frameBounds.width), static_cast<int>(frameBounds.height)});
                    pMacOSOpenGLRenderer->resetContextSize(frameBounds.width, frameBounds.height);
                    res = true; // Skip frame to allow resize to take effect
                    break;
                }
                case DEMINIMIZE_WINDOW:
                case BECOME_ACTIVE:
                {
                    pPGEwindow->olc_OnFocus(true);
                    break;
                }
                case MINIMIZE_WINDOW:
                case RESIGN_ACTIVE:
                {
                    pPGEwindow->olc_OnFocus(false);
                    break;
                }
                case SET_MOUSE_POSITION:
                {
                    // Set mouse position on main thread
                    pMacOSWindow->setCursorPosition(vPositionMouse.x, vPositionMouse.y);
                    res = false; // No need to skip frame
                    break;
                }
                case NONE:
                default:
                {
                    res = false;
                    break;
                }
                    
            }
        }
        vPendingMainThreadTasks.clear();
        
        // 4. Main Thread unlocks PGE Thread
        {
            std::lock_guard<std::mutex> lock(pgeThreadPendingTasksMutex);
            isPGEThreadResetting = false;  // Release PGE thread
            isMainThreadResetting = false; // Reset main thread flag
        }
        pgeThreadResetCondition.notify_all();  // Wake up PGE thread
        return res;
    }

//------ Events Handlers -----

    void Host_Apple_MacOS::MacApplicationEventsHandler()
    {
       pMacApplication->setWillFinishLaunchingCallback([]() { });
       
       pMacApplication->setDidFinishLaunchingCallback([&]() {
           // Queue the Create OpenGL context task
           vPendingMainThreadTasks.push_back(CREATE_OPENGL_RENDERER);
           // We need to wait until the application has launched to get the keyboard layout
           pPGEwindow->keyboard.UseKeyboardLayout(GetKeyboardLayout());
        
           
       });
       
       pMacApplication->setWillTerminateCallback([&]() {
		   //todo : add any cleanup code here if needed
           });
       
       pMacApplication->setDidBecomeActiveCallback([]() { });
       
       pMacApplication->setWillResignActiveCallback([]() { });
        
    }

    void Host_Apple_MacOS::MacWindowEventsHandler()
    {
        pMacOSWindow->setWindowDidResizeCallback([&]() {
            AddPendingMainThreadTask(RESIZE_WINDOW);
        });

        pMacOSWindow->setWindowWillCloseCallback([&]() {
            // NOTE: Do not add this event to PendingMainThreadTasks as it will cause deadlock since the main thread is required to process the close event but the close event is waiting on the main thread tasks to process it
            pPGEwindow->olc_OnWindowClose();
            pPGEwindow->olc_ShouldRemove();
        });

        pMacOSWindow->setWindowDidBecomeKeyCallback([&]() {
            AddPendingMainThreadTask(BECOME_ACTIVE);
        });

        pMacOSWindow->setWindowDidResignKeyCallback([&]() {
            AddPendingMainThreadTask(RESIGN_ACTIVE);
        });
       
        pMacOSWindow->setWindowDidMiniaturizeCallback([&]() {
            AddPendingMainThreadTask(MINIMIZE_WINDOW);
        });
       
        pMacOSWindow->setWindowDidDeminiaturizeCallback([&]() {
            AddPendingMainThreadTask(DEMINIMIZE_WINDOW);
        });
        
    }
    
    // handles both down and up strokes for every supported key that isn't a modifier
    void Host_Apple_MacOS::KeyboardEventHandler(const olc::apis::macos::KeyEvent& event, bool isPressed)
    {
        unsigned short keyCode = event.keyCode;
        
        // handle num clear/lock key only on the down stroke.
        if(isPressed && keyCode == 71)
        {
            bNumLockActive = !bNumLockActive;
            return;
        }

        if(!bNumLockActive)
        {
            // 84 down, 86 left, 88 right, 91 up >>> 125 down, 123 left, 124 right, 126 up
            switch(keyCode)
            {
                case 84: keyCode = 125; break;
                case 86: keyCode = 123; break;
                case 88: keyCode = 124; break;
                case 91: keyCode = 126; break;
                default: break;
            }
        }

        // The @ symbol does not change position from US - UK keyboards on MacOS, so we handle it here
        if(event.modifierFlags & NSEventModifierFlagShift && event.keyCode == 39)
            keyCode = 50;
        
        pPGEwindow->olc_OnKeyPress(mapKeys[keyCode], isPressed);
    }

    void Host_Apple_MacOS::MacEventsHandler()
    {
        // General MacOS key event handling code here
        // Reference: https://eastmanreference.com/complete-list-of-applescript-key-codes

        // Set up keyboard event handlers
        pMacOSEventHandler->onKeyDown([&](const olc::apis::macos::KeyEvent& event) {
            KeyboardEventHandler(event, true);
        });

        pMacOSEventHandler->onKeyUp([&](const olc::apis::macos::KeyEvent& event) {
            KeyboardEventHandler(event, false);
        });

        // Set up keyboard flag event handlers
        pMacOSEventHandler->onFlagsChanged([&](const olc::apis::macos::FlagsChangedEvent& event) {

            static unsigned int prevFlags = 0;
            unsigned int changedFlags = event.modifierFlags ^ prevFlags;
            
            // Check For Shift key
            if (changedFlags & NSEventModifierFlagShift) {
                bool isPressed = event.modifierFlags & NSEventModifierFlagShift;
                pPGEwindow->olc_OnKeyPress(Key::SHIFT, isPressed);
            }
            
            // Check for Control key
            if (changedFlags & NSEventModifierFlagControl) {
                bool isPressed = event.modifierFlags & NSEventModifierFlagControl;
                pPGEwindow->olc_OnKeyPress(Key::CTRL, isPressed);
            }

            if (changedFlags & NSEventModifierFlagCommand) {
                bool isPressed = event.modifierFlags & NSEventModifierFlagCommand;
                if(isPressed)
                    std::cout << "PGE3 doesn't currently support ALT/Command keys but it should.\n";
                
                // pPGEwindow->olc_OnKeyPress(Key::ALT, isPressed);
            }

            // caps lock doesn't appear to trigger any event
            prevFlags = event.modifierFlags;
        });

        // Set up mouse event handlers
        pMacOSEventHandler->onMouseDown([&](const olc::apis::macos::MouseEvent& event) {
                pPGEwindow->olc_OnMouseButton(event.buttonNumber, true);
        });
        
        pMacOSEventHandler->onMouseUp([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseButton(event.buttonNumber, false);
        });
        
        pMacOSEventHandler->onMouseMoved([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseMove({static_cast<int>(event.x), static_cast<int>(event.y)});
        });
        
        pMacOSEventHandler->onMouseDragged([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseMove({static_cast<int>(event.x), static_cast<int>(event.y)});
        });

        pMacOSEventHandler->onRightMouseDragged([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseMove({static_cast<int>(event.x), static_cast<int>(event.y)});
        });

        pMacOSEventHandler->onOtherMouseUp([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseButton(event.buttonNumber, false);
        });

        pMacOSEventHandler->onRightMouseDown([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseButton(event.buttonNumber, true);
        });
        
        pMacOSEventHandler->onRightMouseUp([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseButton(event.buttonNumber, false);
            
        });

        pMacOSEventHandler->onOtherMouseDown([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseButton(event.buttonNumber, true);
        });

         pMacOSEventHandler->onOtherMouseUp([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseButton(event.buttonNumber, false);
        });

        pMacOSEventHandler->onOtherMouseDragged([&](const olc::apis::macos::MouseEvent& event) {
            pPGEwindow->olc_OnMouseMove({static_cast<int>(event.x), static_cast<int>(event.y)});
        });

        pMacOSEventHandler->onScrollWheel([&](const olc::apis::macos::ScrollWheelEvent& event) {
            // Although MacOS provides both deltaX and deltaY, we will only use deltaY for vertical scrolling
            pPGEwindow->olc_OnMouseWheel(static_cast<int>(event.deltaY));
        });

         // Touch events — map trackpad multi-touch to hw::Touch via olc_OnTouch
        pMacOSEventHandler->onTouchBegan([&](const olc::apis::macos::TouchEvent& event) {
            pPGEwindow->olc_OnTouch(event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                true, false,
                {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
        });

        pMacOSEventHandler->onTouchMoved([&](const olc::apis::macos::TouchEvent& event) {
            pPGEwindow->olc_OnTouch(event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                false, false,
                {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
        });

        pMacOSEventHandler->onTouchEnded([&](const olc::apis::macos::TouchEvent& event) {
            pPGEwindow->olc_OnTouch(event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                false, true,
                {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
        });

        pMacOSEventHandler->onTouchCancelled([&](const olc::apis::macos::TouchEvent& event) {
            pPGEwindow->olc_OnTouch(event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                false, true,  // treat cancel as release
                {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
        });

        // Stylus (tablet) events — full pressure, tilt and rotation data
        pMacOSEventHandler->onStylus([&](const olc::apis::macos::StylusEvent& event) {
            
            pPGEwindow->olc_OnTouch(
                event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                event.bPress,
                event.bRelease,
                {1.0f, 1.0f},       // stylus contact size — nominal 1x1
                true,               // bStylus = true
                event.pressure,
                event.rotation,
                {event.tiltX, event.tiltY}
            );
        });
        
    }

}
//! END IMPLEMENTATION

#endif /* OLC_HOST == OLC_HOST_MACOS */
