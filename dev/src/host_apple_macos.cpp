#include "config.h"
#include "host_apple_macos.h"
#include <dispatch/queue.h>
#if OLC_HOST == OLC_HOST_MACOS

//! START IMPLEMENTATION
namespace olc::host {
    
    bool Host_Apple_MacOS::StartSystemEventLoop(bool bBlockIfPossible)
    {
        (void)(bBlockIfPossible); // Remove unused variable warning
        //TODO: Johnngy63 find out what bBlockIfPossible is supposed to do

        // Create MacOS Application instance
        pMacApplication = std::make_unique<olc::apis::macos::Application>();

        // Set up application delegate event handlers
        MacApplicationEventsHandler();

        // Initialize and activate application first
        pMacApplication->initialize();
        pMacApplication->activate();
        
        // Initialize the MacOS Window
        pMacOSWindow = std::make_unique<olc::apis::macos::Window>(frameBounds.width, frameBounds.height, "OLC PGE 3 MacOS Demo");
        
        // Set up window event handlers
        MacWindowEventsHandler();
        
        // Set Window Position
        pMacOSWindow->setPosition(frameBounds.x, frameBounds.y);

        // Create Input Event handler
        pMacOSEventHandler = std::make_unique<olc::apis::macos::EventHandler>(*pMacOSWindow);
            
        // Setup Event handlers
        MacEventsHandler();
        
        // Create the window
        pMacOSWindow->show();
        pMacOSEventHandler->enable();
        
        // Tell the PGE engine we have an OpenGL context ready
        bInitializeOpenGLRenderer = true;
        
        // Start the main event loop (this will block)
        pMacApplication->run();

        return true;
    }

    bool Host_Apple_MacOS::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        
        pPGEwindow = pWindow;
        pPGEwindow->SetWindowPosition(vWindowPos);
        pPGEwindow->SetWindowSize(vWindowSize);
        pPGEwindow->LinkToHost(this);

        frameBounds.x = static_cast<double>(vWindowPos.x);
        frameBounds.y = static_cast<double>(vWindowPos.y);
        frameBounds.width = static_cast<double>(vWindowSize.x);
        frameBounds.height = static_cast<double>(vWindowSize.y);
        
        return true;
    }


    bool Host_Apple_MacOS::CloseWindowFrame(olc::Window* pWindow)
    {
        // TODO: Gracefully close the window and clean up resources
        if (!pMacOSWindow) return false;
        if (!pWindow) return false;
        pWindow->olc_OnWindowClose();
        return true;
    }

    bool Host_Apple_MacOS::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        if (!pMacOSWindow) return false;
        dispatch_async(dispatch_get_main_queue(), ^{
            pMacOSWindow->setTitle(pWindow->GetWindowTitle().c_str());
        });
        return true;
    }

    std::vector<void*> Host_Apple_MacOS::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        // We need to manage a race condition here. The window is created on the main thread
        while(!bInitializeOpenGLRenderer)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(raceConditionTimeoutMS));
        }

        if(pMacOSOpenGLRenderer == nullptr)
        {
            vMacOSWindowDescriptors.clear(); // ensure we are starting fresh
            pMacOSOpenGLRenderer = std::make_shared<olc::apis::macos::OpenGLRenderer>();
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                pMacOSOpenGLRenderer->attachToWindow(*pMacOSWindow);
                pMacOSOpenGLRenderer->setupContext();
            });
            
            pMacGLConextObj = pMacOSOpenGLRenderer->getCGLContextObj();
            
            pMacOSOpenGLRenderer->setVsync(false);
            
            vMacOSWindowDescriptors.push_back(pMacGLConextObj);
        }
        
        // Set up OpenGL renderer for visual feedback
        pMacOSOpenGLRenderer->makeCurrentContext();
        return vMacOSWindowDescriptors;
       
    }

    bool Host_Apple_MacOS::ConnectHostResourceToRenderer()
    {
        return false;
    }

    bool Host_Apple_MacOS::SyncWithDesktopComposite()
    {
        if(!enableVSync)
        {
            pMacOSOpenGLRenderer->enableVsync();
            enableVSync = true;
        }
        
        return enableVSync;
    }


    void Host_Apple_MacOS::MacApplicationEventsHandler()
    {
        // Application event handling code here
        // Set application delegate event handlers
       pMacApplication->setWillFinishLaunchingCallback([]() {
           //std::cout << "--> Application delegate: Will finish launching" << std::endl;
       });
       
       pMacApplication->setDidFinishLaunchingCallback([&]() {
           //std::cout << "--> Application delegate: Did finish launching" << std::endl;
           // Tell the PGE 3.0 we have looded the application
           bApplicationInitialized = true;
       });
       
       pMacApplication->setWillTerminateCallback([&]() {
           //std::cout << "--> Application delegate: Will terminate" << std::endl;
           // TODO: Johnngy63 - Implement olc_OnApplicationTerminate in window.h/cpp
           
       });
       
       pMacApplication->setDidBecomeActiveCallback([]() {
           //std::cout << "--> Application delegate: Did become active" << std::endl;
       });
       
       pMacApplication->setWillResignActiveCallback([]() {
           //std::cout << "--> Application delegate: Will resign active" << std::endl;
       });
        
    }

    void Host_Apple_MacOS::MacWindowEventsHandler()
    {
        // Window event handling code here
        pMacOSWindow->setWindowDidResizeCallback([&]() {
            double width, height;
            pMacOSWindow->getWindowSize(width, height);
            pPGEwindow->olc_OnWindowSize({static_cast<int>(width), static_cast<int>(height)});

        });

        pMacOSWindow->setWindowWillCloseCallback([&]() {
            // TODO: Johnngy63 - Implement any pre-close logic if needed
            pPGEwindow->olc_OnWindowClose();
            pPGEwindow->olc_ShouldRemove();
        });

        pMacOSWindow->setWindowDidBecomeKeyCallback([&]() {
            //TODO: Johnngy63 - Implement olc_OnWindowFocus in window.h/cpp
        });

        pMacOSWindow->setWindowDidResignKeyCallback([&]() {
            //TODO: Johnngy63 - Implement olc_OnWindowFocus in window.h/cpp
        });
       
        pMacOSWindow->setWindowDidMiniaturizeCallback([]() {
            //todo: Johnngy63 - Implement olc_OnWindowMinimize in window.h/cpp if needed
        });
       
        pMacOSWindow->setWindowDidDeminiaturizeCallback([]() {
            //TODO: Johnngy63 - Implement olc_OnWindowFocus in window.h/cpp if needed
        });
        
        // Tell the PGE engine we have a window initialized
        bWindowInitialized = true;
    }


    void Host_Apple_MacOS::MacEventsHandler()
    {
        // General MacOS event handling code here
        // Reference: https://eastmanreference.com/complete-list-of-applescript-key-codes

        // Set up keyboard event handlers
        pMacOSEventHandler->onKeyDown([](const olc::apis::macos::KeyEvent& event) {
            std::cout << "Key Down - Code: " << event.keyCode
                        << ", Chars: '" << event.characters << "'" << std::endl;
            
            // Handle special keys
            switch (event.keyCode) {
                case 53: // Escape
                    std::cout << "Escape key pressed!" << std::endl;
                    break;
                case 36: // Return
                    std::cout << "Return key pressed!" << std::endl;
                    break;
                case 49: // Space
                    std::cout << "Space key pressed!" << std::endl;
                    break;
                case 123: // Left arrow
                    std::cout << "Left arrow pressed!" << std::endl;
                    break;
                case 124: // Right arrow
                    std::cout << "Right arrow pressed!" << std::endl;
                    break;
                case 125: // Down arrow
                    std::cout << "Down arrow pressed!" << std::endl;
                    break;
                case 126: // Up arrow
                    std::cout << "Up arrow pressed!" << std::endl;
                    break;
                default:
                    if (!event.characters.empty()) {
                        std::cout << "Character key: '" << event.characters << "'" << std::endl;
                    }
                    break;
            }
        });
        
        pMacOSEventHandler->onKeyUp([](const olc::apis::macos::KeyEvent& event) {
            std::cout << "Key Up - Code: " << event.keyCode << std::endl;
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
            pPGEwindow->olc_OnMouseWheel({static_cast<int>(event.deltaY)});
        });
        
        // Tell the PGE engine we have an event handler initialized
        bEventHandlerInitialized = true;

    }


}
//! END IMPLEMENTATION
#endif /* OLC_HOST == OLC_HOST_MACOS */
