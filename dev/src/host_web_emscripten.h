#pragma once
#include "core.h"

//! START STDHEADER GLOBAL
#include <atomic>
#include <cstdint>
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/key_codes.h>

namespace olc::host
{
    class Host_Web_Emscripten : public olc::host::Host
    {
    public:
        Host_Web_Emscripten();
        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;
        
        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;
        olc::KeyboardLayout GetKeyboardLayout() const override;
    
    public:
        // Called at very start of application
        bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
        // Called to start the host - this may mean different things on different hosts
        bool StartSystem() override;
        // Called to stop the host, and shutdown all resources
        bool StopSystem() override;
        // Called at start of system event loop
        bool OnSystemThreadStart() override;
        // Called to perform primary window update
        bool OnSystemTick() override;
        // Called at end of system event loop
        bool OnSystemThreadEnd() override;
        // Called at very end of application
        bool OnApplicationEnd() override;
        
        static void MainLoop(void* userData);

    public: // event callbacks
        static EM_BOOL keyboard_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
        static EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent* e, void* userData);
        static EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent* e, void* userData);
        static EM_BOOL touch_callback(int eventType, const EmscriptenTouchEvent* e, void* userData);
        static EM_BOOL fullscreen_change_callback(int eventType, const EmscriptenFullscreenChangeEvent *event, void *userData);
        static EM_BOOL resize_callback(int eventType, const EmscriptenUiEvent *event, void *userData);
        static EM_BOOL focus_callback(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData);
    
    private: // Window Wrappers
		// Set Mouse Device State
		static bool olc_OnMouseButton(olc::Window* pWindow, const uint8_t nButton, const bool bPressed);
		static bool olc_OnMouseMove(olc::Window* pWindow, const olc::vi2d& vMousePos);
		static bool olc_OnMouseWheel(olc::Window* pWindow, const int32_t nScroll);
		static bool olc_OnMouseFocus(olc::Window* pWindow, const bool bHasFocus);
		
        // Set Keyboard Device State
        static bool olc_OnKeyPress(olc::Window* pWindow, const olc::Key key, const bool bPressed);

		// Set Window State
		static bool olc_OnWindowPosition(olc::Window* pWindow, const olc::vi2d& vWindowPos);
		static bool olc_OnWindowSize(olc::Window* pWindow, const olc::vi2d& vWindowSize);
		static bool olc_OnWindowClose(olc::Window* pWindow);
    
    private: // helpers
        static olc::Window* GetWindowFromCanvasId(std::string canvasId);
        
    public: // Callback data type
        struct CallbackData {
            Host_Web_Emscripten* pHost;
            olc::Window* pWindow;
            std::string canvasId;
        };

    private:
        static std::unordered_map<size_t, std::string> mapUID2CanvasId;
        static std::unordered_map<size_t, std::unique_ptr<CallbackData>> mapUID2CallbackData;
        static std::unordered_map<std::string, olc::Window*> mapCanvasId2PTR;
        
        // Map of system keycodes to olc::Keycodes
        std::unordered_map<int32_t, olc::Key> mapKeys;
    };
    
    
}

//! END DECLARATION