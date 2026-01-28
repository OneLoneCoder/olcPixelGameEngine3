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
            Host_Apple_MacOS() = default;
            virtual ~Host_Apple_MacOS() {};
            
        public:
            HostError GetLastError() const { return lastError; }

        public:
            virtual bool StartSystemEventLoop(bool bBlockIfPossible = false) override;
			virtual bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
			virtual bool CloseWindowFrame(olc::Window* pWindow) override;
			virtual bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

			virtual std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;
			
			virtual bool ConnectHostResourceToRenderer() override;

			// Wait for entire host desktop refresh (for smooooth vsync),
			virtual bool SyncWithDesktopComposite() override;

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
            
        };
    }
}

//! END DECLARATION
#endif /* olc_HOST == olc_HOST_MACOS */

