#pragma once
#include "core.h"

//! START STDHEADER GLOBAL
#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
#include <dlfcn.h>

#define ZERO_DECLARE_SYM(ns, func) static decltype(ns::func)* zero_##func;
#define ZERO_LOAD_SYM(lib, sym) \
    *reinterpret_cast<void**>(&zero_##sym) = dlsym(lib, #sym);

#include <GL/gl.h>
namespace X11
{
    #include <X11/X.h>
    #include <X11/Xlib.h>
    #include <X11/XKBlib.h>
    #include <X11/Xutil.h>
    #include <GL/glx.h>
    #undef None
    constexpr int None = 0L;
}

ZERO_DECLARE_SYM(X11, XCreateBitmapFromData)
ZERO_DECLARE_SYM(X11, XCreateColormap)
ZERO_DECLARE_SYM(X11, XCreatePixmapCursor)
ZERO_DECLARE_SYM(X11, XCreateWindow)
ZERO_DECLARE_SYM(X11, XDefineCursor)
ZERO_DECLARE_SYM(X11, XDestroyWindow)
ZERO_DECLARE_SYM(X11, XFlush)
ZERO_DECLARE_SYM(X11, XFree)
ZERO_DECLARE_SYM(X11, XFreeCursor)
ZERO_DECLARE_SYM(X11, XFreePixmap)
ZERO_DECLARE_SYM(X11, XGetAtomName)
ZERO_DECLARE_SYM(X11, XGetWindowAttributes)
ZERO_DECLARE_SYM(X11, XGrabPointer)
ZERO_DECLARE_SYM(X11, XInitThreads)
ZERO_DECLARE_SYM(X11, XInternAtom)
ZERO_DECLARE_SYM(X11, XLookupString)
ZERO_DECLARE_SYM(X11, XMapWindow)
ZERO_DECLARE_SYM(X11, XNextEvent)
ZERO_DECLARE_SYM(X11, XOpenDisplay)
ZERO_DECLARE_SYM(X11, XPending)
ZERO_DECLARE_SYM(X11, XSetWMProtocols)
ZERO_DECLARE_SYM(X11, XStoreName)
ZERO_DECLARE_SYM(X11, XUndefineCursor)
ZERO_DECLARE_SYM(X11, XUngrabPointer)
ZERO_DECLARE_SYM(X11, XWarpPointer)
ZERO_DECLARE_SYM(X11, XkbFreeKeyboard)
ZERO_DECLARE_SYM(X11, XkbGetMap)
ZERO_DECLARE_SYM(X11, XkbGetNames)
ZERO_DECLARE_SYM(X11, XkbGetState)
ZERO_DECLARE_SYM(X11, XkbQueryExtension)
ZERO_DECLARE_SYM(X11, XkbSelectEventDetails)

ZERO_DECLARE_SYM(X11, glXChooseVisual)
ZERO_DECLARE_SYM(X11, glXCreateContext)
ZERO_DECLARE_SYM(X11, glXDestroyContext)
ZERO_DECLARE_SYM(X11, glXGetProcAddress)
ZERO_DECLARE_SYM(X11, glXMakeCurrent)
ZERO_DECLARE_SYM(X11, glXSwapBuffers)

ZERO_DECLARE_SYM( , glGenTextures)
ZERO_DECLARE_SYM( , glBindTexture)
ZERO_DECLARE_SYM( , glTexParameteri)
ZERO_DECLARE_SYM( , glTexEnvf)
ZERO_DECLARE_SYM( , glDeleteTextures)
ZERO_DECLARE_SYM( , glTexImage2D)
ZERO_DECLARE_SYM( , glClear)
ZERO_DECLARE_SYM( , glViewport)
ZERO_DECLARE_SYM( , glClearColor)
ZERO_DECLARE_SYM( , glReadPixels)
ZERO_DECLARE_SYM( , glCullFace)
ZERO_DECLARE_SYM( , glEnable)
ZERO_DECLARE_SYM( , glDisable)
ZERO_DECLARE_SYM( , glDrawArrays)
ZERO_DECLARE_SYM( , glBlendFunc)
ZERO_DECLARE_SYM( , glDepthFunc)
ZERO_DECLARE_SYM( , glGetTexImage)
ZERO_DECLARE_SYM( , glHint)
ZERO_DECLARE_SYM( , glPolygonMode)
ZERO_DECLARE_SYM( , glFrontFace)
ZERO_DECLARE_SYM( , glFinish)


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
    public:
        Host_Linux_X11();
        ~Host_Linux_X11();

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

    public:
        bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
        bool StartSystem() override;
        bool StopSystem() override;
        bool OnSystemThreadStart() override;
        bool OnSystemTick() override;
        bool OnSystemThreadEnd() override;
        bool OnApplicationEnd() override;
    
    private:
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
    private:
        void* libx11_handle{nullptr};
        void* libgl_handle{nullptr};
        void* libglx_handle{nullptr};
    };
}

//! END DECLARATION