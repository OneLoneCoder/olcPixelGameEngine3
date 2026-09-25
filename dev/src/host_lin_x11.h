#pragma once
#include "core.h"

//! START STDHEADER GLOBAL
#include <atomic>
#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
#include <GL/gl.h>
#include <poll.h>
namespace X11
{
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>
#include <GL/glx.h>
#undef None
constexpr int None = 0L;
}

namespace olc::host
{
    class Host_Linux_X11 : public olc::host::Host
    {
	private:
		X11::Display* olc_Display = nullptr;
		X11::Window					 olc_WindowRoot;
		X11::XVisualInfo* olc_VisualInfo;
		X11::Colormap                olc_ColourMap;
		X11::XSetWindowAttributes    olc_SetWindowAttribs;

        int xinput_extension_code{};
    public:
        Host_Linux_X11();

        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

        olc::KeyboardLayout GetKeyboardLayout() const override;
        void UpdateKeyboardLayout();

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;

        // Force the mouse position in pixels relative to window
        bool SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos) override;
        // Show or hide mouse cursor for given window
        bool SetMouseVisible(olc::Window* pWindow, const bool bVisible) override;
        bool SetFullScreen(olc::Window* pWindow, const bool bFullScreen) override;

    public:
        bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
        bool StartSystem() override;
        bool StopSystem() override;
        bool OnSystemThreadStart() override;
        bool OnSystemTick() override;
        bool OnSystemThreadEnd() override;
        bool OnApplicationEnd() override;
    
    private:
        void enableTouch(X11::Display* display, X11::Window window);
        void handleTouchEvent(X11::XIDeviceEvent* event);
        std::unordered_map<size_t, X11::Window> mapUID2X11Window;
        std::unordered_map<X11::Window, olc::Window*> mapX11Window2PTR;
        std::atomic<bool> systemActive {true};

        std::unordered_map<uint32_t, olc::Key> mapKeys;
        std::unordered_map<int, int> mapMouseButtons;

        // Keyboard Layout Variables
        olc::KeyboardLayout keyboardLayout = OLC_DEFAULT_KEYBOARD_LAYOUT;
        bool kbExtensionsFound = false;
        int xkbEventBase = 0;
        int xkbErrorBase = 0;

        // Mouse Variables
        std::unordered_map<size_t, X11::Cursor> mapUID2X11Cursor;
        bool bMouseIsVisible = true;
    };
}

//! END DECLARATION