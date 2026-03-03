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
#include "api_macos_wrapper.hpp"
#include "window.h"

//! END CUSTOMHEADER

//! START MACOS_CONFIG

//! END MACOS_CONFIG

#if OLC_HOST == OLC_HOST_MACOS
//! START DECLARATION
#include <dispatch/dispatch.h>  // Grand Central Dispatch:  Apple's C-based API for managing concurrent operations on macOS and iOS.
#include <pthread.h>            // POSIX threads: Used for pthread_main_np() to check if current thread is the main thread
namespace olc
{
    namespace host
    {
        // Manages our MacOS Host
        class Host_Apple_MacOS : public olc::host::Host
        {
        public:
            olc::Window* pPGEwindow = nullptr;                  // Pointer to PGE Window
            
        public:
            Host_Apple_MacOS();
            virtual ~Host_Apple_MacOS() {};
            
        public:
            HostError GetLastError() const { return lastError; }

        public:
			virtual bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
			virtual bool CloseWindowFrame(olc::Window* pWindow) override;
			virtual bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

			virtual std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

			// Wait for entire host desktop refresh (for smooooth vsync),
			virtual bool SyncWithDesktopComposite() override;
            
        public: // Platform specific Mouse Control
            // Force the mouse position in pixels relative to window
            virtual bool SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos) override;
            // Show or hide mouse cursor for given window
            virtual bool SetMouseVisible(olc::Window* pWindow, const bool bVisible) override;
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
            // MacOS Application and Window pointers
            std::unique_ptr<olc::apis::macos::Application> pMacApplication = nullptr;
            std::unique_ptr<olc::apis::macos::Window> pMacOSWindow = nullptr;
            std::unique_ptr<olc::apis::macos::EventHandler> pMacOSEventHandler = nullptr;
            std::shared_ptr<olc::apis::macos::OpenGLRenderer> pMacOSOpenGLRenderer = nullptr;

            void* pMacGLConextObj = nullptr;
            std::once_flag intialAppFlag;
            
            // Map of system keycodes to olc::Keycodes
            std::unordered_map<int32_t, olc::Key> mapKeys;
    
        private:      
            enum MAINTASKS{
                NONE,
                CREATE_OPENGL_RENDERER,
                RESIZE_WINDOW,
                BECOME_ACTIVE,
                RESIGN_ACTIVE,
                MINIMIZE_WINDOW,
                DEMINIMIZE_WINDOW
            };
            
            // Internal Mac OS functions
            bool ExecutePendingMainThreadTasks(void);       // Execute pending tasks on main thread
            bool MainThreadTasks(void);                     // Handle main thread tasks
            bool AddPendingMainThreadTask(MAINTASKS task);  // Add a pending task to main thread. Note: You should ever add tasks that require main thread execution only from the PGE thread
            bool CreateCGLContextObj();                     // Create CGL Context Object
            std::vector<MAINTASKS> vPendingMainThreadTasks; // Vector of pending main thread tasks
            
            std::vector<void*> vMacOSWindowDescriptors; // Vector to hold window descriptors
            bool enableVSync = false;                   // VSync enabled flag
            bool bSkipFrame = false;                    // Flag to indicate if frame should be skipped 

            // Thread synchronization for PGE Thread V Main thread
            mutable std::mutex      mainThreadPendingTasksMutex;    // Mutex for main thread pending tasks
            std::condition_variable mainThreadResetCondition;       // Condition variable for main thread reset
            std::atomic<bool>       isMainThreadResetting{false};   // Atomic flag for resetting main thread 

            mutable std::mutex      pgeThreadPendingTasksMutex;    // Mutex for PGE thread pending tasks
            std::condition_variable pgeThreadResetCondition;       // Condition variable for PGE thread reset
            std::atomic<bool>       isPGEThreadResetting{true};    // Atomic flag for resetting PGE thread
            
            std::atomic<bool>       systemActive = false;          // Atomic flag for system active state

            struct sFrameBounds
            {
                double x = 0.0;
                double y = 0.0;
                double width = 800.0;
                double height = 600.0;
            } frameBounds;

            void MacApplicationEventsHandler();
            void MacWindowEventsHandler();
            void MacEventsHandler();
            void MacOpenGLContextEventsHandler();
            void KeyboardEventHandler(const olc::apis::macos::KeyEvent& event, bool isPressed);
            bool bNumLockActive = true;         // Num Lock state, we assume it's active at start
            
        };
    }
}

//! END DECLARATION
#endif /* olc_HOST == olc_HOST_MACOS */

