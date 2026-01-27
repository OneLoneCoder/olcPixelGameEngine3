#pragma once

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
//! END STDHEADER

//! START DECLARATION
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/log.h>

namespace olc::host
{
    class Host_Android : public olc::host::Host
    {
    private:
        struct android_app* olc_App = nullptr;
    public:
        bool StartSystemEventLoop(bool bBlockIfPossible) override;
        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

        bool ConnectHostResourceToRenderer() override;

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;

        void OnAppCmd(struct android_app* app, int32_t cmd);
        void SetAndroidApp(struct android_app* app);

        bool IsInitialized() const { return initialized.load(); }
    protected:
        olc::Window* pgeWindow = nullptr;
        std::atomic<bool> initialized{false};
    };
}
//! END DECLARATION
