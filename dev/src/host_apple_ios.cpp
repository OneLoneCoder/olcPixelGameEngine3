#include "config.h"
#include "host_apple_ios.h"
#include "core.h"
#include <dispatch/queue.h>

#if OLC_HOST == OLC_HOST_IOS

//! START IMPLEMENTATION
namespace olc::host {
    

    // NSEventModifierFlags values
    constexpr unsigned int NSEventModifierFlagShift      = 1 << 17; // 0x20000
    constexpr unsigned int NSEventModifierFlagControl    = 1 << 18; // 0x40000
    constexpr unsigned int NSEventModifierFlagCommand    = 1 << 20; // 0x100000

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

    Host_Apple_iOS::Host_Apple_iOS()
    {
         // Reference: https://eastmanreference.com/complete-list-of-applescript-key-codes
        mapKeys[0x00] = Key::NONE;

        // Map macOS key codes to olc::Key codes
        mapKeys[0]  = Key::A;
        mapKeys[11] = Key::B;
        mapKeys[8]  = Key::C;
        mapKeys[2]  = Key::D;
        mapKeys[14] = Key::E;
        mapKeys[3]  = Key::F;
        mapKeys[5]  = Key::G;
        mapKeys[4]  = Key::H;
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
        mapKeys[1]  = Key::S;
        mapKeys[17] = Key::T;
        mapKeys[32] = Key::U;
        mapKeys[9]  = Key::V;
        mapKeys[13] = Key::W;
        mapKeys[7]  = Key::X;
        mapKeys[16] = Key::Y;
        mapKeys[6]  = Key::Z;

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
        mapKeys[99]  = Key::F3;
        mapKeys[118] = Key::F4;
        mapKeys[96]  = Key::F5;
        mapKeys[97]  = Key::F6;
        mapKeys[98]  = Key::F7;
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
        mapKeys[51]  = Key::BACK;        // Delete (Backspace)
        mapKeys[53]  = Key::ESCAPE;      // Escape
        mapKeys[36]  = Key::ENTER;       // Return
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
        // TODO: Gracefully close the view controller and clean up resources
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
        // if we're already in the specified state, return early
        //if(pMacOSWindow->isFullScreen() == bFullScreen)
        //    return true;

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
        pIOSApplication->setApplicationPath(sIOSApplicationPath);
        
        // Set up application delegate event handlers
        IOSApplicationEventsHandler();
        
        // Initialize and activate application first
        pIOSApplication->initialize();

        return true;

    }

    bool Host_Apple_iOS::StopSystem()
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            // clean up and close application
            /*
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
             
             */

        });
        //systemActive = false;
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
            
            //vPendingMainThreadTasks.push_back(INTIALIZE_PGE_RENDERER);
            //vPendingMainThreadTasks.push_back(RESIZE_WINDOW);
            //AddPendingMainThreadTask(START_DRAWING);
            //pPrimaryPGE->OnContextStart();
            std::cout << "iOS Application did finish launching." << std::endl;
        });
        
        pIOSApplication->setWillTerminateCallback([&]() {
            // TODO: Implement olc_OnApplicationTerminate in window.h/cpp
            std::cout << "iOS Application will terminate." << std::endl;
        });
        
        pIOSApplication->setDidBecomeActiveCallback([&]() {
            std::cout << "iOS Application did become active." << std::endl;
        });
        
        pIOSApplication->setWillResignActiveCallback([]() {
            // App will become inactive (background)
            std::cout << "iOS Application will resign active." << std::endl;
        });
        
        pIOSApplication->setDidEnterBackgroundCallback([]() {
            // App entered background
            std::cout << "iOS Application did enter background." << std::endl;
        });
        
        pIOSApplication->setWillEnterForegroundCallback([]() {
            // App will enter foreground
            std::cout << "iOS Application will enter foreground." << std::endl;
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
            std::cout << "iOS View Controller did load." << std::endl;
        });

        pIOSViewController->setViewWillAppearCallback([&]() {
            // View will appear
            
            std::cout << "iOS View Controller will appear." << std::endl;
        });

        pIOSViewController->setViewDidAppearCallback([&]() {
            // View appeared
            
            if(!bIOSDrawing)
            {
                pIOSViewController->setPaused(false); // Start drawing
            }
            std::cout << "iOS View Controller did appear." << std::endl;
        });

        pIOSViewController->setViewWillDisappearCallback([&]() {
            // View will disappear
            pPGEwindow->olc_OnWindowClose();
            std::cout << "iOS View Controller will disappear." << std::endl;
        });

        pIOSViewController->setViewDidDisappearCallback([&]() {
            // View disappeared
                        
            pPGEwindow->olc_ShouldRemove();
            std::cout << "iOS View Controller did disappear." << std::endl;
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
        else
        {
            //pPrimaryPGE->OnContextTick();
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
        std::cout << "ID: " << event.touchID << " Touch Began at (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
    });

    pIOSGLKView->setTouchMovedCallback([&](const olc::apis::ios::TouchEvent& event) {
          pPGEwindow->olc_OnTouch(event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                false, false,
                {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
        std::cout << "ID: " << event.touchID << " Touch Moved to (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
    });

    pIOSGLKView->setTouchEndedCallback([&](const olc::apis::ios::TouchEvent& event) {
          pPGEwindow->olc_OnTouch(event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                false, true,
                {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
        std::cout << "ID: " << event.touchID << " Touch Ended at (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
    });

    pIOSGLKView->setTouchCancelledCallback([&](const olc::apis::ios::TouchEvent& event) {
         pPGEwindow->olc_OnTouch(event.touchID,
                {static_cast<float>(event.x), static_cast<float>(event.y)},
                false, true,  // treat cancel as release
                {static_cast<float>(event.sizeX), static_cast<float>(event.sizeY)});
        std::cout << "ID: " << event.touchID << " Touch Cancelled at (" << event.x << ", " << event.y << ") with size (" << event.sizeX << ", " << event.sizeY << ")" << std::endl;
    });

    // Set up keyboard event handlers
    pIOSGLKView->setKeyDownCallback([&](int keyCode, const std::string& characters) {
        std::cout << "Key Down - Code: " << keyCode << ", Chars: '" << characters << "'" << std::endl;
        
        // Handle special keys
        switch (keyCode) {
            case 53: // Escape
                std::cout << "Escape key pressed!" << std::endl;
                break;
            case 36: // Return
                std::cout << "Return key pressed!" << std::endl;
                break;
            case 49: // Space
                std::cout << "Space key pressed!" << std::endl;
                break;
            default:
                if (!characters.empty()) {
                    std::cout << "Character key: '" << characters << "'" << std::endl;
                }
                break;
        }
    });

    pIOSGLKView->setKeyUpCallback([&](int keyCode, const std::string& characters) {
        std::cout << "Key Up - Code: " << keyCode << std::endl;
    });
}
}

//! END IMPLEMENTATION
#endif /* OLC_HOST == OLC_HOST_IOS */
