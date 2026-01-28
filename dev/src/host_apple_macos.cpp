#include "config.h"
#include "host_apple_macos.h"
#include <dispatch/queue.h>
#if OLC_HOST == OLC_HOST_MACOS

//! START IMPLEMENTATION
namespace olc::host {

    bool Host_Apple_MacOS::StartSystemEventLoop(bool bBlockIfPossible){
        (void)(bBlockIfPossible); // Remove unused variable warning

        // Create MacOS Application instance
        pMacApplication = std::make_unique<olc::apis::macos::Application>();

        // Set up application delegate event handlers
        MacApplicationEventsHandler();

        // Initialize and activate application first
        pMacApplication->initialize();
        pMacApplication->activate();
        
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
        pMacOSWindow->show();
        pMacOSEventHandler->enable();
        
        // Start the main event loop (this will block)
        pMacApplication->run();

        return true;
    }

    bool Host_Apple_MacOS::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen){       
        pPGEwindow = pWindow;
        pPGEwindow->SetWindowPosition(vWindowPos);
        pPGEwindow->SetWindowSize(vWindowSize); // Temporary small size to avoid large window on creation
        pPGEwindow->LinkToHost(this);

        frameBounds.x = 0.0;
        frameBounds.y = 0.0;
        frameBounds.width = static_cast<double>(vWindowSize.x);
        frameBounds.height = static_cast<double>(vWindowSize.y);
        
        return true;
    }


    bool Host_Apple_MacOS::CloseWindowFrame(olc::Window* pWindow){
        if (!pMacOSWindow) return false;
        if (!pWindow) return false;
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
        // While the PGE is running, if there are pending main thread tasks, process them, this causes PGE to wait
        bSkipFrame = ExecutePendingMainThreadTasks();
        
        // Ensure OpenGL renderer is created
        if(pMacOSOpenGLRenderer == nullptr)
            CreateCGLContextObj();

        return vMacOSWindowDescriptors;
       
    }

    bool Host_Apple_MacOS::ConnectHostResourceToRenderer()
    {
        return false;
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
                case MINIMIZE_WINDOW:
                case DEMINIMIZE_WINDOW:
                case BECOME_ACTIVE:
                case RESIGN_ACTIVE:
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
       });
       
       pMacApplication->setWillTerminateCallback([&]() {
           // TODO: Johnngy63 - Implement olc_OnDestory in window.h/cpp
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
            // TODO: Johnngy63 - Implement any pre-close logic if needed
            pPGEwindow->olc_OnWindowClose();
            pPGEwindow->olc_ShouldRemove();
        });

        pMacOSWindow->setWindowDidBecomeKeyCallback([&]() {
            //TODO: Johnngy63 - Implement olc_OnWindowFocus in window.h/cpp
            AddPendingMainThreadTask(BECOME_ACTIVE);
        });

        pMacOSWindow->setWindowDidResignKeyCallback([&]() {
            //TODO: Johnngy63 - Implement olc_OnWindowFocus in window.h/cpp
        });
       
        pMacOSWindow->setWindowDidMiniaturizeCallback([&]() {
            AddPendingMainThreadTask(MINIMIZE_WINDOW);
        });
       
        pMacOSWindow->setWindowDidDeminiaturizeCallback([&]() {
            AddPendingMainThreadTask(DEMINIMIZE_WINDOW);
        });
        
    }


    void Host_Apple_MacOS::MacEventsHandler()
    {
        // General MacOS key event handling code here
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
            pPGEwindow->olc_OnMouseWheel(static_cast<int>(event.deltaY));
        });
        
    }

}
//! END IMPLEMENTATION
#endif /* OLC_HOST == OLC_HOST_MACOS */
