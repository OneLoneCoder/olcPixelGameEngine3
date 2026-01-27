#pragma once

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <atomic>
//! END STDHEADER

//! START DECLARATION
#include <android_native_app_glue.h>
#include <android/log.h>

// We allow users to create a normal main function for android apps
extern int main(int argc, char** argv);

namespace olc::host
{
    using AndroidApp = struct android_app;
    class Host_Android : public olc::host::Host
    {
    public:
        Host_Android();
        bool StartSystemEventLoop(bool bBlockIfPossible) override;
        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

        bool ConnectHostResourceToRenderer() override;

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;

        olc::KeyboardLayout GetKeyboardLayout() const override;

        void OnAppCmd(AndroidApp* app, int32_t cmd);
        int32_t OnInputEvent(AndroidApp* app, AInputEvent* event);

        bool IsInitialized() const { return initialized.load(); }

        static AndroidApp* androidApp;
    protected:
        olc::Window* pgeWindow = nullptr;
        std::atomic<bool> initialized{false};
    };
}
//! END DECLARATION
