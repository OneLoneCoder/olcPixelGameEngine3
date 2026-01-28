#pragma once

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
#include <GL/gl.h>
namespace X11
{
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <GL/glx.h>
}

namespace olc::host
{
    class Host_Linux_X11 : public olc::host::Host
    {
	private:
		X11::Display* olc_Display = nullptr;
		X11::Window					 olc_WindowRoot;
		//X11::Window					 olc_Window;
		X11::XVisualInfo* olc_VisualInfo;
		X11::Colormap                olc_ColourMap;
		X11::XSetWindowAttributes    olc_SetWindowAttribs;
    public:
        Host_Linux_X11();
        bool StartSystemEventLoop(bool bBlockIfPossible = false) override;
        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;
        
        
        bool ConnectHostResourceToRenderer() override;
        
        olc::KeyboardLayout GetKeyboardLayout() const override;

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;
    private:
        std::unordered_map<size_t, X11::Window> mapUID2X11Window;
        std::unordered_map<X11::Window, olc::Window*> mapX11Window2PTR;
        std::atomic<bool> terminate {false};

        std::unordered_map<uint32_t, olc::Key> mapKeys;
    };
}

//! END DECLARATION