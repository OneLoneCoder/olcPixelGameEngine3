#include "host_lin_drm.h"

//! START IMPLEMENTATION
namespace olc::host
{
    Host_Linux_DRM::Host_Linux_DRM()
    {
    }

    bool Host_Linux_DRM::OnApplicationStart(olc::PixelGameEngine* pPrimary)
    {
        
        pPrimaryPGE = pPrimary;
        return true;
    }

    bool Host_Linux_DRM::StartSystem()
    {

        pPrimaryPGE->OnPreContextStart();

		// Create system thread - handles gpu context
		std::thread threadSystem([this]()
        {
            // Notify start of system thread
            if (!this->OnSystemThreadStart())
            {
                // PGE->OnContextStart() failed, or user aborted OnUserCreate()
                return;
            }

            // Main system loop
            while (systemActive)
            {
                // Perform primary window update
                if (!this->OnSystemTick())
                {
                    StopSystem();
                }
            }

            // Notify end of system thread
            if (!this->OnSystemThreadEnd())
            {
                // PGE->OnContextEnd() failed
                return;
            }
        });

        while(systemActive)
        {
            // event pumping
        }

        systemActive = false;
        if(threadSystem.joinable())
            threadSystem.join();
    
        return pPrimaryPGE->OnPostContextEnd();
    }

    bool Host_Linux_DRM::StopSystem()
    {
        systemActive = false;
        return true;
    }

    bool Host_Linux_DRM::OnSystemThreadStart()
    {
        return pPrimaryPGE->OnContextStart();
    }

    bool Host_Linux_DRM::OnSystemTick()
    {
        return pPrimaryPGE->OnContextTick();;
    }

    bool Host_Linux_DRM::OnSystemThreadEnd()
    {
        return pPrimaryPGE->OnContextEnd();
    }

    bool Host_Linux_DRM::OnApplicationEnd()
    {
        return true;
    }

    bool Host_Linux_DRM::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        drmContext.drm_fd = open("/dev/dri/card0", O_RDWR);
        if(drmContext.drm_fd < 0)
        {
            std::cout << "Failed to open DRM device.\n";
            return false;
        }
        
        drmContext.resources = drmModeGetResources(drmContext.drm_fd);
        if(!drmContext.resources)
        {
            std::cout << "Failed: drmModeGetResources.\n";
            return false;
        }
        
        for(int i = 0; i < drmContext.resources->count_connectors; i++)
        {
            drmContext.connector = drmModeGetConnector(drmContext.drm_fd, drmContext.resources->connectors[i]);
            if(drmContext.connector->connection == DRM_MODE_CONNECTED)
                break;
            
            drmModeFreeConnector(drmContext.connector);
            drmContext.connector = nullptr;
        }

        if(!drmContext.connector)
        {
            std::cout << "Failed to establish a drm connection.\n";
            return false;
        }

        std::cout << "DRM: found connector.\n";
        
        // select the first mode (usually the preferred/native resolution)
        drmContext.mode = drmContext.connector->modes[0];

        // find first CRTC
        drmContext.crtc_id = drmContext.resources->crtcs[0];
        
        drmContext.gbm = gbm_create_device(drmContext.drm_fd);
        if(!drmContext.gbm)
        {
            std::cout << "GBM: failed to create device.\n";
            return false;
        }

        std::cout << "GBM: created device.\n";

        drmContext.gbm_surface = gbm_surface_create(
            drmContext.gbm,
            drmContext.mode.hdisplay,
            drmContext.mode.vdisplay,
            GBM_BO_FORMAT_XRGB8888,
            GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
        );
        
        if(!drmContext.gbm_surface)
        {
            std::cout << "GBM: failed to create surface.\n";
            return false;
        }
        
        std::cout << "GBM: surface created successfully.\n";
        
        mapUID2DRMContext.insert_or_assign(pWindow->GetUID(), &drmContext);
		mapDRMContext2PTR.insert_or_assign(&drmContext, pWindow);

        return true;
    }

    bool Host_Linux_DRM::CloseWindowFrame(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2DRMContext.find(pWindow->GetUID());
        if (window_handle != mapUID2DRMContext.end()) {
            gbm_surface_destroy(window_handle->second->gbm_surface);
            gbm_device_destroy(window_handle->second->gbm);
            drmModeFreeConnector(window_handle->second->connector);
            drmModeFreeResources(window_handle->second->resources);
            mapUID2DRMContext.erase(window_handle);
        }
        return true;
    }
    
    bool Host_Linux_DRM::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        // Not implemented on this platform
        return true;
    }

    std::vector<void*> Host_Linux_DRM::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2DRMContext.find(pWindow->GetUID());
        if (window_handle != mapUID2DRMContext.end()) {
            return {
                reinterpret_cast<void*>(window_handle->second),
            };
        }
  		return {};
    }

    olc::KeyboardLayout Host_Linux_DRM::GetKeyboardLayout() const {
        return keyboardLayout;
    }

    void Host_Linux_DRM::UpdateKeyboardLayout()
    {
        // TODO: implement the update for keyboard layout
        // keyboardLayout = olc::KeyboardLayout::QWERTY_US;
        // keyboardLayout = olc::KeyboardLayout::QWERTY_UK;
        // keyboardLayout = olc::KeyboardLayout::QWERTZ;
        // keyboardLayout = olc::KeyboardLayout::AZERTY;
    }

    // Wait for entire host desktop refresh (for smooooth vsync)
    bool Host_Linux_DRM::SyncWithDesktopComposite()
    {
        // Not implemented on this platform
        return true;
    }
}
//! END IMPLEMENTATION
