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
            //GLint glSwapInterval = 0;                         // VSync disbaled by default
            
        public:
            Host_Apple_MacOS() = default;
            virtual ~Host_Apple_MacOS() {};
            
        public:
            // Check/Get last error
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
            // Internal Mac OS functions
            // MacOS Application and Window pointers
            std::unique_ptr<olc::apis::macos::Application> pMacApplication = nullptr;
            std::unique_ptr<olc::apis::macos::Window> pMacOSWindow = nullptr;
            std::unique_ptr<olc::apis::macos::EventHandler> pMacOSEventHandler = nullptr;
            std::shared_ptr<olc::apis::macos::OpenGLRenderer> pMacOSOpenGLRenderer = nullptr;

            void* pMacGLConextObj = nullptr;
            std::once_flag intialAppFlag;
    
            
        private:
                        
            bool bApplicationInitialized = false;       // Flag to indicate application has initialized
            bool bWindowInitialized = false;            // Flag to indicate window has initialized
            bool bEventHandlerInitialized = false;      // Flag to indicate event handler has initialized
            bool bInitializeOpenGLRenderer = false;     // Flag to indicate OpenGL renderer should be initialized
            std::vector<void*> vMacOSWindowDescriptors; // Vector to hold window descriptors
            bool enableVSync = false;                   // VSync enabled flag
            const uint16_t raceConditionTimeoutMS = 1;  // Race condition sleep time in milliseconds

            struct sFrameBounds
            {
                double x = 0.0;
                double y = 0.0;
                double width = 800.0;
                double height = 600.0;
            } frameBounds;

             // TODO: Should these be private?
            void MacApplicationEventsHandler();
            void MacWindowEventsHandler();
            void MacEventsHandler();
            void MacOpenGLContextEventsHandler();
            
        };
    }
}

//! END DECLARATION
#endif /* olc_HOST == olc_HOST_MACOS */

