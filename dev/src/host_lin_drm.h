#pragma once
#include "core.h"

//! START STDHEADER GLOBAL
#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>
#include <unordered_map>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
//! END STDHEADER


//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <gbm.h>

namespace olc::host
{

    class Host_Linux_DRM : public olc::host::Host
    {
    public:
        Host_Linux_DRM();

        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

        olc::KeyboardLayout GetKeyboardLayout() const override;
        void UpdateKeyboardLayout();

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;

    public:
        bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
        bool StartSystem() override;
        bool StopSystem() override;
        bool OnSystemThreadStart() override;
        bool OnSystemTick() override;
        bool OnSystemThreadEnd() override;
        bool OnApplicationEnd() override;
    
    public:
        struct DRMContext
        {
            int drm_fd{0};
            drmModeRes* resources{nullptr};
            drmModeConnector* connector{nullptr};
            drmModeModeInfo mode;
            uint32_t crtc_id{0};
            struct gbm_device* gbm{nullptr};
            struct gbm_surface* gbm_surface{nullptr};
        };

    private:
        DRMContext drmContext;
        std::unordered_map<size_t, DRMContext*> mapUID2DRMContext;
        std::unordered_map<DRMContext*, olc::Window*> mapDRMContext2PTR;
        std::atomic<bool> systemActive {true};

        std::unordered_map<uint32_t, olc::Key> mapKeys;

        // Keyboard Layout Variables
        olc::KeyboardLayout keyboardLayout = OLC_DEFAULT_KEYBOARD_LAYOUT;
        bool kbExtensionsFound = false;
        int xkbEventBase = 0;
        int xkbErrorBase = 0;

    };
}

//! END DECLARATION