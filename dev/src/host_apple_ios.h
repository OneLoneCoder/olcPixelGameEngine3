#pragma once
#include "config.h"

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <exception>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
#include "api_ios_wrapper.hpp"
#include "window.h"
//! END CUSTOMHEADER

//! START IOS_CONFIG
//! END IOS_CONFIG

#if OLC_HOST == OLC_HOST_IOS
//! START DECLARATION
#include <dispatch/dispatch.h>  // Grand Central Dispatch: Apple's C-based API for managing concurrent operations on iOS
#include <pthread.h>            // POSIX threads: Used for pthread_main_np() to check if current thread is the main thread

namespace olc
{
    namespace host
    {
        // Manages our iOS Host
        class Host_Apple_iOS : public olc::host::Host
        {
            
        public:
            olc::Window* pPGEwindow = nullptr;                  // Pointer to PGE Window
            
        public:
            // Default constructor
            Host_Apple_iOS();
            // Constructor with application path
            Host_Apple_iOS(std::string appPath) : sIOSApplicationPath(appPath) {};
            virtual ~Host_Apple_iOS() {};
            
        public:
            // Check/Get last error
            HostError GetLastError() const { return lastError; }

        public:
            virtual bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
            virtual bool CloseWindowFrame(olc::Window* pWindow) override;
            virtual bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

            virtual std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

            // Wait for entire host desktop refresh (for smooth vsync)
            virtual bool SyncWithDesktopComposite() override;

            public: // Platform specific Mouse Control
            // Force the mouse position in pixels relative to window
            virtual bool SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos) override;
            // Show or hide mouse cursor for given window
            virtual bool SetMouseVisible(olc::Window* pWindow, const bool bVisible) override;
            // Set a window to fullscreen or not fullscreen
            virtual bool SetFullScreen(olc::Window* pWindow, const bool bFullScreen) override;

        public: // OS Specific Environment Information
            virtual olc::KeyboardLayout GetKeyboardLayout() const override;

            public: // Platform Specific OS<->PGE Linkage
            // Called at very start of application
            virtual bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
            // Called to start the host - this may mean different things on different hosts
            // It MUST block until system is requested to exit
            virtual bool StartSystem() override;
            // Called to stop the host, and shutdown all resources
            virtual bool StopSystem() override;
            // Called at start of system event loop
            virtual bool OnSystemThreadStart() override;
            // Called to perform primary window update
            virtual bool OnSystemTick() override;
            // Called at end of system event loop
            virtual bool OnSystemThreadEnd() override;
            // Called at very end of application
            virtual bool OnApplicationEnd() override;

        protected:
            HostError lastError = HostError::None;

        public:
            // Internal iOS functions
            // iOS Application and View Controller pointers
            std::unique_ptr<olc::apis::ios::Application> pIOSApplication = nullptr;
            std::unique_ptr<olc::apis::ios::ViewController> pIOSViewController = nullptr;
            std::unique_ptr<olc::apis::ios::GLKView> pIOSGLKView = nullptr;
            std::shared_ptr<olc::apis::ios::OpenGLRenderer> pIOSOpenGLRenderer = nullptr;

            void* pIOSGLContextObj = nullptr;
            std::once_flag initialAppFlag;

            // Map of system keycodes to olc::Keycodes
            std::unordered_map<int32_t, olc::Key> mapKeys;
            
            // IOS Application path
            std::string sIOSApplicationPath;
            
            void (*pPGEEngineFunc)(void*);
            void (*pPGECoreUpdateFunc)(void*);
            void* pPGEUserData;
            
            bool iosAppIsActive = false; // Flag indicating if the iOS app is active (foreground)

            
        private:
            
            enum MAINTASKS{
                NONE,
                INTIALIZE_PGE_RENDERER,
                RESIZE_WINDOW,
                MAIN_THREAD_DRAW,
                START_DRAWING,
                STOP_DRAWING
            };
            
            
            // Internal Mac OS functions
            bool ExecutePendingMainThreadTasks(void);       // Execute pending tasks on main thread
            bool MainThreadTasks(void);                     // Handle main thread tasks
            bool AddPendingMainThreadTask(MAINTASKS task);  // Add a pending task to main thread. Note: You should ever add tasks that require main thread execution only from the PGE thread
            // Thread synchronization for PGE Thread V Main thread
            mutable std::mutex      mainThreadPendingTasksMutex;    // Mutex for main thread pending tasks
            std::condition_variable mainThreadResetCondition;       // Condition variable for main thread reset
            std::atomic<bool>       isMainThreadResetting{false};   // Atomic flag for resetting main thread
            std::vector<MAINTASKS> vPendingMainThreadTasks;         // Vector of pending main thread tasks
            
            mutable std::mutex      pgeThreadPendingTasksMutex;    // Mutex for PGE thread pending tasks
            std::condition_variable pgeThreadResetCondition;       // Condition variable for PGE thread reset
            std::atomic<bool>       isPGEThreadResetting{true};    // Atomic flag for resetting PGE thread

            std::vector<void*> vIOSWindowDescriptors;     // Vector to hold view descriptors
            bool enableVSync = false;                     // VSync enabled flag
            
            bool bIOSDrawing = false;                     // Flag indicating if iOS drawing is active
            bool bPGEInitialized = false;                 // Flag indicating if PGE has been initialized

            struct sFrameBounds
            {
                double x = 0.0;
                double y = 0.0;
                double width = 800.0;
                double height = 600.0;
            } frameBounds;
            
            olc::apis::ios::DeviceOrientation CurrentOrientation = olc::apis::ios::DeviceOrientation::Unknown;
            void UpdateIOSViewFrameBounds(olc::apis::ios::DeviceOrientation orientation);

            // TODO: Should these be private?
            void IOSApplicationEventsHandler();
            void IOSViewControllerEventsHandler();
            void IOSGLKViewEventHandler();
            void IOSOpenGLContextEventsHandler();
        };
    }
}

//! END DECLARATION
#endif /* OLC_HOST == OLC_HOST_IOS */
