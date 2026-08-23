#include "config.h"
#include "host_apple_ios.h"
#include "core.h"
#include <dispatch/queue.h>

#if OLC_HOST == OLC_HOST_IOS

//! START IMPLEMENTATION
namespace olc::host {
    

    // NSEventModifierFlags values
    constexpr unsigned int NSEventModifierFlagCapsLock   = 1 << 16; // 0x10000
    constexpr unsigned int NSEventModifierFlagShift      = 1 << 17; // 0x20000
    constexpr unsigned int NSEventModifierFlagControl    = 1 << 18; // 0x40000
    constexpr unsigned int NSEventModifierFlagCommand    = 1 << 20; // 0x100000

    Host_Apple_iOS::Host_Apple_iOS()
    {
         // Reference: https://eastmanreference.com/complete-list-of-applescript-key-codes
        mapKeys[0x00] = Key::NONE;

        // Map macOS key codes to olc::Key codes
        mapKeys[4]  = Key::A;
        mapKeys[5]  = Key::B;
        mapKeys[6]  = Key::C;
        mapKeys[7]  = Key::D;
        mapKeys[8]  = Key::E;
        mapKeys[9]  = Key::F;
        mapKeys[10] = Key::G;
        mapKeys[11] = Key::H;
        mapKeys[12] = Key::I;
        mapKeys[13] = Key::J;
        mapKeys[14] = Key::K;
        mapKeys[15] = Key::L;
        mapKeys[16] = Key::M;
        mapKeys[17] = Key::N;
        mapKeys[18] = Key::O;
        mapKeys[19] = Key::P;
        mapKeys[20] = Key::Q;
        mapKeys[21] = Key::R;
        mapKeys[22] = Key::S;
        mapKeys[23] = Key::T;
        mapKeys[24] = Key::U;
        mapKeys[25] = Key::V;
        mapKeys[26] = Key::W;
        mapKeys[27] = Key::X;
        mapKeys[28] = Key::Y;
        mapKeys[29] = Key::Z;

        // Numeric keys
        mapKeys[30] = Key::K1;
        mapKeys[31] = Key::K2;
        mapKeys[32] = Key::K3;
        mapKeys[33] = Key::K4;
        mapKeys[34] = Key::K5;
        mapKeys[35] = Key::K6;
        mapKeys[36] = Key::K7;
        mapKeys[37] = Key::K8;
        mapKeys[38] = Key::K9;
        mapKeys[39] = Key::K0;
        
        // Other Keys
        mapKeys[40]  = Key::ENTER;      // Return
        mapKeys[41]  = Key::ESCAPE;     // Escape
        mapKeys[42]  = Key::BACK;       // Delete (Backspace)
        mapKeys[43]  = Key::TAB;        // Tab
        mapKeys[44]  = Key::SPACE;      // Space
        mapKeys[45]  = Key::DEL;        // Forward Delete
        mapKeys[57]  = Key::CAPS_LOCK;  // Caps Lock
        mapKeys[114] = Key::INS;        // Help (Insert equivalent)

        // Function Keys
        mapKeys[58] = Key::F1;
        mapKeys[59] = Key::F2;
        mapKeys[60] = Key::F3;
        mapKeys[61] = Key::F4;
        mapKeys[62] = Key::F5;
        mapKeys[63] = Key::F6;
        mapKeys[64] = Key::F7;
        mapKeys[65] = Key::F8;
        mapKeys[66] = Key::F9;
        mapKeys[67] = Key::F10;
        mapKeys[68] = Key::F11;
        mapKeys[69] = Key::F12;

        mapKeys[74] = Key::HOME;       // Home
        mapKeys[75] = Key::PGUP;       // Page Up
        mapKeys[77] = Key::END;        // End
        mapKeys[78] = Key::PGDN;       // Page Down
        
        // Arrow Keys
        mapKeys[79] = Key::RIGHT;
        mapKeys[80] = Key::LEFT;
        mapKeys[81] = Key::DOWN;
        mapKeys[82] = Key::UP;
        
        // CHRL, SHIFT Keys
        mapKeys[224]  = Key::CTRL;        // Left Control
        mapKeys[225]  = Key::SHIFT;       // Left Shift
        mapKeys[229]  = Key::SHIFT;       // Left Shift
       
        // Symbol Keys (OEM equivalents)
        mapKeys[51] = Key::OEM_1;       // On US and UK keyboards this is the ';:' key
        mapKeys[56] = Key::OEM_2;       // On US and UK keyboards this is the '/?' key
        mapKeys[53] = Key::OEM_3;       // On US and UK keyboards this is the '`~' key (Grave accent `)
        mapKeys[47] = Key::OEM_4;       // On US and UK keyboards this is the '[{' key
        mapKeys[49] = Key::OEM_5;       // On US keyboard this is '\|' key.
        mapKeys[48] = Key::OEM_6;       // On US and UK keyboards this is the ']}' key
        mapKeys[52] = Key::OEM_7;       // On US keyboard this is the single/double quote key. On UK, this is the single quote/@ symbol key
        mapKeys[24] = Key::EQUALS;      // Equal sign =
        mapKeys[43] = Key::COMMA;       // Comma ,
        mapKeys[27] = Key::MINUS;       // Minus -
        mapKeys[47] = Key::PERIOD;      // Period .
        
        // Unknown and may not be supported, but included for completeness will come back to this
        mapKeys[900] = Key::NP0;
        mapKeys[901] = Key::NP1;
        mapKeys[902] = Key::NP2;
        mapKeys[903] = Key::NP3;
        mapKeys[904] = Key::NP4;
        mapKeys[905] = Key::NP5;
        mapKeys[906] = Key::NP6;
        mapKeys[907] = Key::NP7;
        mapKeys[908] = Key::NP8;
        mapKeys[909] = Key::NP9;
        mapKeys[910] = Key::NP_MUL;      // Numpad *
        mapKeys[911] = Key::NP_ADD;      // Numpad +
        mapKeys[912] = Key::NP_DIV;      // Numpad /
        mapKeys[913] = Key::NP_SUB;      // Numpad -
        mapKeys[914] = Key::NP_DECIMAL;  // Numpad .
        mapKeys[915] = Key::PAUSE;       // F16 (often used as pause)
        mapKeys[916] = Key::SCROLL;      // F14 (scroll lock equivalent)
        mapKeys[917] = Key::OEM_8;       // Section sign # (varies by keyboard)

    }

    bool Host_Apple_iOS::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        olc_IgnoreUnused(bFullScreen); 
        pPGEwindow = pWindow;
        pPGEwindow->SetWindowPosition(vWindowPos);
        pPGEwindow->SetWindowSize(vWindowSize);
        pPGEwindow->LinkToHost(this);

        // iOS typically uses full screen, but we'll store the requested dimensions
        frameBounds.x = static_cast<double>(vWindowPos.x);
        frameBounds.y = static_cast<double>(vWindowPos.y);
        frameBounds.width = static_cast<double>(vWindowSize.x);
        frameBounds.height = static_cast<double>(vWindowSize.y);
        
        return true;
    }

    bool Host_Apple_iOS::CloseWindowFrame(olc::Window* pWindow)
    {
        if (!pIOSViewController) return false;
        if (!pWindow) return false;
        pWindow->olc_OnWindowClose();
        return true;
    }

    bool Host_Apple_iOS::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        if (!pIOSViewController) return false;
        dispatch_async(dispatch_get_main_queue(), ^{
            pIOSViewController->setTitle(pWindow->GetWindowTitle().c_str());
        });
        return true;
    }

    std::vector<void*> Host_Apple_iOS::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        return vIOSWindowDescriptors;
    }

    bool Host_Apple_iOS::SyncWithDesktopComposite()
    {
        if(!enableVSync)
        {
            pIOSOpenGLRenderer->enableVsync();
            enableVSync = true;
        }
        
        return enableVSync;
    }

    bool Host_Apple_iOS::SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos)
    {
        olc_IgnoreUnused(pWindow);
        dispatch_sync(dispatch_get_main_queue(), ^{
            //pMacOSWindow->setCursorPosition(vPos.x, vPos.y);
        });
        return false;
    }

    bool Host_Apple_iOS::SetMouseVisible(olc::Window* pWindow, const bool bVisible)
    {
        olc_IgnoreUnused(pWindow);
        dispatch_sync(dispatch_get_main_queue(), ^{
            //pMacOSWindow->setCursorVisibility(bVisible);
        });
        return true;
    }

    bool Host_Apple_iOS::SetFullScreen(olc::Window* pWindow, const bool bFullScreen)
    {
        olc_IgnoreUnused(pWindow);
        dispatch_async(dispatch_get_main_queue(), ^{
            //pMacOSWindow->toggleFullScreen();
        });
        return true;
    }

     bool Host_Apple_iOS::OnApplicationStart(olc::PixelGameEngine* pPrimary){
        pPrimaryPGE = pPrimary;
        return true;
    }

    bool Host_Apple_iOS::StartSystem(){
        
        // Pre-context start hook
        pPrimaryPGE->OnPreContextStart();
        
        // Create iOS Application instance
        pIOSApplication = std::make_unique<olc::apis::ios::Application>();
        
        // Set up application delegate event handlers
        IOSApplicationEventsHandler();
        
        // Initialize and activate application first
        pIOSApplication->initialize();

        return true;

    }

    bool Host_Apple_iOS::StopSystem()
    {
        // Note: iOS applications typically don't have a "stop" method, but we can clean up resources here if needed
        //dispatch_sync(dispatch_get_main_queue(), ^{
            
        //});
        return true;
    }

    bool Host_Apple_iOS::OnSystemThreadStart()
    {
        return pPrimaryPGE->OnContextStart();
    }

    bool Host_Apple_iOS::OnSystemTick()
    {
        return pPrimaryPGE->OnContextTick();
    }

    bool Host_Apple_iOS::OnSystemThreadEnd()
    {
        return pPrimaryPGE->OnContextEnd();
    }

    bool Host_Apple_iOS::OnApplicationEnd()
    {
        return true;
    }

    std::string Host_Apple_iOS::GetApplicationPath()
    {
        if(pIOSApplication)
        {
            return pIOSApplication->getApplicationPath();
        }
        return "ACCESS-DEINED";
    }

    olc::KeyboardLayout Host_Apple_iOS::GetKeyboardLayout() const
    {
        // Get system locale from MacOS Application
        // We need to wait until the application has launched to get the keyboard layout
        // Therefore this function is called again from setDidFinishLaunchingCallback event
        if (pIOSApplication)
        {
            std::string locale = pIOSApplication->getSystemLocale();
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


    void Host_Apple_iOS::UpdateIOSViewFrameBounds(olc::apis::ios::DeviceOrientation orientation)
    {
        // NOTE: iOS handles rotation automatically, but we need to adjust the PGE window size accordingly
        // IMPORTANT: PortraitUpsideDown is not implemented, in short have a happy life and forget this exist ;)
        CurrentOrientation = orientation;
        switch (orientation) {
            case olc::apis::ios::DeviceOrientation::Portrait:
                pIOSViewController->getViewSize(frameBounds.width, frameBounds.height);
                pPGEwindow->olc_OnWindowSize({static_cast<int>(frameBounds.width), static_cast<int>(frameBounds.height)});
                break;
            case olc::apis::ios::DeviceOrientation::LandscapeLeft:
            case olc::apis::ios::DeviceOrientation::LandscapeRight:
                pIOSViewController->getViewSize(frameBounds.width, frameBounds.height);
                pPGEwindow->olc_OnWindowSize({static_cast<int>(frameBounds.height), static_cast<int>(frameBounds.width)});
                break;
            case olc::apis::ios::DeviceOrientation::FaceUp:
            case olc::apis::ios::DeviceOrientation::FaceDown:
            case olc::apis::ios::DeviceOrientation::PortraitUpsideDown:
            case olc::apis::ios::DeviceOrientation::Unknown:
            default:
                break;
        }
        

    }

    void Host_Apple_iOS::IOSApplicationEventsHandler()
    {
        // Application event handling code here
        // Set application delegate event handlers
        pIOSApplication->setDidFinishLaunchingCallback([&]() {
            // Tell the PGE 3.0 we have loaded the application
          
            pIOSApplication->activate();
            
            // Initialize the iOS View Controller
            pIOSViewController = std::make_unique<olc::apis::ios::ViewController>(frameBounds.width, frameBounds.height, "OLC PGE 3 iOS Demo");
            
            // Set up view controller event handlers
            IOSViewControllerEventsHandler();
            
            // Present the view controller
            pIOSViewController->present();
            
            // App became active (foreground)
            if(pIOSOpenGLRenderer == nullptr)
            {
                vIOSWindowDescriptors.clear(); // ensure we are starting fresh
                pIOSOpenGLRenderer = std::make_shared<olc::apis::ios::OpenGLRenderer>();
                
                pIOSOpenGLRenderer->attachToViewController(*pIOSViewController);
                pIOSOpenGLRenderer->setupContext();

                pIOSGLContextObj = pIOSOpenGLRenderer->getEAGLContext();
                
                pIOSOpenGLRenderer->setVsync(false);
                
                vIOSWindowDescriptors.push_back(pIOSGLContextObj);
            }
            
            // Set up OpenGL renderer for visual feedback
            pIOSOpenGLRenderer->makeCurrentContext();
            
            //std::cout << "iOS Application did finish launching." << std::endl;
        });
        
        pIOSApplication->setWillTerminateCallback([&]() {
            // TODO: Implement olc_OnApplicationTerminate in window.h/cpp
            //std::cout << "iOS Application will terminate." << std::endl;
        });
        
        pIOSApplication->setDidBecomeActiveCallback([&]() {
            // TODO: manage thread resuming
            
            if(pPGEwindow)
                pPGEwindow->olc_OnFocus(true);
            ///std::cout << "iOS Application did become active." << std::endl;
        });
        
        pIOSApplication->setWillResignActiveCallback([&]() {
            
            if(pPGEwindow)
                pPGEwindow->olc_OnFocus(false);
            // TODO: manage thread pausing
            //std::cout << "iOS Application will resign active." << std::endl;
        });
        
        pIOSApplication->setDidEnterBackgroundCallback([&]() {
            // App entered background
            if(pPGEwindow)
                pPGEwindow->olc_OnFocus(false);
            std::cout << "iOS Application did enter background." << std::endl;
        });
        
        pIOSApplication->setWillEnterForegroundCallback([&]() {
            // App will enter foreground
            if(pPGEwindow)
                pPGEwindow->olc_OnFocus(true);
            //std::cout << "iOS Application will enter foreground." << std::endl;
        });
    }

    void Host_Apple_iOS::IOSViewControllerEventsHandler()
    {
        // View controller event handling code here
        pIOSViewController->setViewDidLoadCallback([&]() {
            // View controller loaded
                      
            // Set up GLKView after presentation
            pIOSGLKView = std::make_unique<olc::apis::ios::GLKView>(*pIOSViewController);
            // Setup Event handlers (including touch events)
            IOSGLKViewEventHandler();
            if (!pIOSGLKView->initialize())
            {
                std::cerr << "Failed to initialize GLKView." << std::endl;
                return;
            }
            pIOSGLKView->enableTouchHandling();
            
            auto test = pIOSGLKView->isMultipleTouchEnabled();
            if(!test)
                pIOSGLKView->setMultipleTouchEnabled(true);
            pIOSGLKView->debugTouchSetup();
            //std::cout << "iOS View Controller did load." << std::endl;
        });

        pIOSViewController->setViewWillAppearCallback([&]() {
            // View will appear

            //std::cout << "iOS View Controller will appear." << std::endl;
        });

        pIOSViewController->setViewDidAppearCallback([&]() {
            // View appeared
            
            if(!bIOSDrawing)
            {
                pIOSViewController->setPaused(false); // Start drawing
            }
            //std::cout << "iOS View Controller did appear." << std::endl;
        });

        pIOSViewController->setViewWillDisappearCallback([&]() {
            // View will disappear
            pPGEwindow->olc_OnWindowClose();
            //std::cout << "iOS View Controller will disappear." << std::endl;
        });

        pIOSViewController->setViewDidDisappearCallback([&]() {
            // View disappeared
                        
            pPGEwindow->olc_ShouldRemove();
            //std::cout << "iOS View Controller did disappear." << std::endl;
        });
        
        pIOSViewController->setViewDidLayoutSubviewsCallback([&]() {
            
            UpdateIOSViewFrameBounds(CurrentOrientation);
        });
        
        pIOSViewController->setUpdateCallback( [&]() {
            // This is where we will call the engine's main loop for rendering
            
        });
        
        // Device orientation changes
        pIOSViewController->setOrientationChangedCallback([&](olc::apis::ios::DeviceOrientation orientation) {
            UpdateIOSViewFrameBounds(orientation);
        });
        
    }

    // handles both down and up strokes for every supported key that isn't a modifier
    void Host_Apple_iOS::KeyboardEventHandler(uint16_t nKeyCode, uint nModifierFlags, bool isPressed){
        // handle num clear/lock key only on the down stroke.
        if(isPressed && nKeyCode == 71)
        {
            bNumLockActive = !bNumLockActive;
            return;
        }
        
        if(!bNumLockActive)
        {
            // 84 down, 86 left, 88 right, 91 up >>> 125 down, 123 left, 124 right, 126 up
            switch(nKeyCode)
            {
                case 84: nKeyCode = 125; break;
                case 86: nKeyCode = 123; break;
                case 88: nKeyCode = 124; break;
                case 91: nKeyCode = 126; break;
                default: break;
            }
        }
        
        // Check Modifier flags for Shift, Control, Caps, and Command keys
        unsigned int changedFlags = nModifierFlags ^ prevFlags;
        
        // Check For Shift key
        if (changedFlags & NSEventModifierFlagShift) {
            bool isPressed = nModifierFlags & NSEventModifierFlagShift;
            pPGEwindow->olc_OnKeyPress(Key::SHIFT, isPressed);
        }
        
        // Check for Control key
        if (changedFlags & NSEventModifierFlagControl) {
            bool isPressed = nModifierFlags & NSEventModifierFlagControl;
            pPGEwindow->olc_OnKeyPress(Key::CTRL, isPressed);
        }

        // Check for Option / ALT key
        if (changedFlags & NSEventModifierFlagCommand) {
            bool isPressed = nModifierFlags & NSEventModifierFlagCommand;
            if(isPressed)
                std::cout << "PGE3 doesn't currently support ALT/Command keys but it should.\n";
        }

        prevFlags = nModifierFlags;
        
        pPGEwindow->olc_OnKeyPress(mapKeys[nKeyCode], isPressed);
        
   }

    void Host_Apple_iOS::IOSGLKViewEventHandler()
    {
        
        // Set up drawing callback for GLKView
        pIOSGLKView->setDrawCallback([&](double x, double y, double width, double height) {
            
            pIOSOpenGLRenderer->makeCurrentContext();
            
            if(!bPGEInitialized)
            {
                pPrimaryPGE->OnContextStart();
                bPGEInitialized = true;
            }
            pPrimaryPGE->OnContextTick();
        });
        
        // Set up touch event handlers (iOS primary input method)
        pIOSGLKView->setTouchBeganCallback([&](const olc::apis::ios::TouchEvent& event) {
            // Convert touch to mouse button press for compatibility
            pPGEwindow->olc_OnMouseButton(0, true); // Left click equivalent
            pPGEwindow->olc_OnTouch(event.touchID,
                    {static_cast<float>(event.x), static_cast<float>(event.y)},
                    true, false,
                    {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
            //std::cout << "ID: " << event.touchID << " Touch Began at (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
        });

        pIOSGLKView->setTouchMovedCallback([&](const olc::apis::ios::TouchEvent& event) {
              pPGEwindow->olc_OnTouch(event.touchID,
                    {static_cast<float>(event.x), static_cast<float>(event.y)},
                    false, false,
                    {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
            //std::cout << "ID: " << event.touchID << " Touch Moved to (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
        });

        pIOSGLKView->setTouchEndedCallback([&](const olc::apis::ios::TouchEvent& event) {
              pPGEwindow->olc_OnTouch(event.touchID,
                    {static_cast<float>(event.x), static_cast<float>(event.y)},
                    false, true,
                    {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
            //std::cout << "ID: " << event.touchID << " Touch Ended at (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
        });

        pIOSGLKView->setTouchCancelledCallback([&](const olc::apis::ios::TouchEvent& event) {
             pPGEwindow->olc_OnTouch(event.touchID,
                    {static_cast<float>(event.x), static_cast<float>(event.y)},
                    false, true,  // treat cancel as release
                    {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
            //std::cout << "ID: " << event.touchID << " Touch Cancelled at (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
        });

        // Set up keyboard event handlers
        pIOSGLKView->setKeyDownCallback([&](uint16_t keyCode, const std::string& characters, uint modifierFlags) {
            //std::cout << "Key Down - Code: " << keyCode << ", Chars: '" << characters << "'" << std::endl;
            KeyboardEventHandler(keyCode, modifierFlags, true);
        });

        pIOSGLKView->setKeyUpCallback([&](uint16_t keyCode, const std::string& characters, uint modifierFlags) {
            //std::cout << "Key Up - Code: " << keyCode << std::endl;
            KeyboardEventHandler(keyCode, modifierFlags, false);
        });
    }
}

//! END IMPLEMENTATION
#endif /* OLC_HOST == OLC_HOST_IOS */
