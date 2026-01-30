#include "core.h"

#include "gpu_opengl33.h"

#if OLC_HOST == OLC_HOST_WINDOWS
#include "host_win_winapi.h"
#include "imload_wingdi.h"
#endif

// Johnnyg63: Added define for MACOS
#if OLC_HOST == OLC_HOST_MACOS
#include "host_apple_macos.h"
#include "imload_macos.h"
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
#include "host_lin_x11.h"
#include "imload_lib_png.h"
#endif

#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
#include "host_lin_wayland.h"
#include "imload_lib_png.h"
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
#include "host_web_emscripten.h"
#include "imload_lib_png.h"
#endif

#if OLC_HOST == OLC_HOST_ANDROID
#include "host_android.h"
#include "imload_android.h"
#endif

//! START IMPLEMENTATION
namespace olc
{
	bool PGEWindow::Create(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize)
	{
		//pRenderer->RetargetDevice(pHost->GetHostWindowDescriptor(this));
		pRenderer->PrepareWindowTarget(pHost->GetHostWindowDescriptor(this));
		CreateImage(GetDefaultImage(), vScreenSize);
		SetWindowSize(vScreenSize * vPixelSize);

		// Assume 1:1 Relationship for now
		vViewPos = { 0,0 };
		vViewSize = vScreenSize * vPixelSize;
		return true;
	}

	bool PGEWindow::OnUserCreate()
	{
		return true;
	}

	bool PGEWindow::OnUserUpdate(float fElapsedTime)
	{
		olc_IgnoreUnused(fElapsedTime);
		return true;
	}

	bool PGEWindow::OnUserDestroy()
	{
		return true;
	}

	bool PGEWindow::olc_WindowUpdate(const float fElapsedTime, const float fTotalElapsedTime)
	{
		// Input Changes
		mouse.UpdateState();
		keyboard.UpdateState();
		touch.UpdateState();
		
		draw.SetGPU(pRenderer);
		draw.SetTarget(GetDefaultImage());

		pRenderer->DisplayPrepare(fElapsedTime, fTotalElapsedTime);

#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
		pRenderer->RetargetDevice(pHost->GetHostWindowDescriptor(this));
#endif
		pRenderer->ApplyDefaultShader();



		// User Update
		if (!OnUserUpdate(fElapsedTime) || bRequestToClose)
		{
			// User has requested termination of window by returning false
			if (OnUserDestroy())
			{
				// User has confirmed window destruction by returning true
				bShouldRemove = true;
			}
			else
				bRequestToClose = false; // User vetoed closure
		}


		// Finialise any outstanding tasks
		draw.ProcessGPUTasks();

		if (GetDefaultImage().GetConfig().MSAA)
		{
			pRenderer->ResolveMSAA(uint32_t(GetDefaultImage().GetGPUID()));
		}

		draw.ResetShader();
		draw.WorldReset();		

		// Take the window's completed "screen" and draw it as a textured quad to the backbuffer
		pRenderer->AssignTextureTarget(0, 0);


		// === Viewport Handling ===
		if (config.bRetainAspectRatio)
		{
			// Set the viewport to maintain the aspect ratio of GetDefaultImage() and maximise to 
			// fit within the window client area
			float fAspectScreen = float(GetDefaultImage().Size().x) / float(GetDefaultImage().Size().y);

			vViewSize.x = (int32_t)vWindowSize.x;
			vViewSize.y = (int32_t)((float)vViewSize.x / fAspectScreen);

			if (vViewSize.y > vWindowSize.y)
			{
				vViewSize.y = vWindowSize.y;
				vViewSize.x = (int32_t)((float)vViewSize.y * fAspectScreen);
			}

			vViewPos = (vWindowSize - vViewSize) / 2;			
		}
		else
		{
			// Stretch to fit window (or 1:1 pixel mapping)
			vViewPos = { 0,0 };
			vViewSize = vWindowSize;	

			// Note: Important to keep "View" updated as mouse will need to
			// know for scaling
		}

		// Present final composite
		pRenderer->SetViewport(vViewPos, vViewSize);
		pRenderer->ClearViewport(config.colClear, true, true);
		draw.ImageRect(GetDefaultImage().flipV(), { 0.0,0.0 }, vViewSize);
		draw.ProcessGPUTasks();

		// Update Window's primary surface
		//pRenderer->RetargetDevice(pHost->GetHostWindowDescriptor(this));
		pRenderer->DisplayDraw(pHost->GetHostWindowDescriptor(this));

		return true;
	}

	bool PGEWindow::CreateImage(olc::Image& image, const olc::vi2d& size, const ImageConfig& cfg)
	{
		// Create CPU Image
		if (!image.Create(size, cfg))
			return false;

		// Create GPU Image
		auto id = pRenderer->CreateTexture(image.Size(), cfg);
		if (id == 0)
		{
			image.Create({ 0,0 });
			return false;
		}

		// Associate CPU object with GPU Resource
		image.SetGPUID(id);
		return true;
	}

	bool PGEWindow::CreateImageFromFile(olc::Image& image, const std::string& sFileName, const ImageConfig& cfg)
	{
		if (pImageLoader->CreateImageFromFile(image, sFileName))
		{
			// Image has loaded ok, and populated into pixel vector
			// 
			// Create GPU Image
			auto id = pRenderer->CreateTexture(image.Size(), cfg);
			if (id == 0)
			{
				image.Create({ 0,0 });
				return false;
			}

			// Associate CPU object with GPU Resource
			image.SetGPUID(id);
			return true;
		}

		return false;
	}

	bool PGEWindow::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes, const ImageConfig& cfg)
	{
		olc_IgnoreUnused(image, data, bytes, cfg);
		return false;
	}

	bool PGEWindow::WriteImageToFile(const olc::Image& image, const std::string& sFileName)
	{
		olc_IgnoreUnused(image, sFileName);
		return false;
	}

	void PGEWindow::DestroyImage(olc::Image& image)
	{
		// If image has gpu resource, remove it
		if (image.GetGPUID() != 0)
		{
			pRenderer->DeleteTexture(uint32_t(image.GetGPUID()));
			image.SetGPUID(0);
		}

		// Free any cpu memory associated with image
		image.Create({ 0,0 });
	}

	void PGEWindow::LinkToRenderer(olc::gpu::Renderer* gpu)
	{
		pRenderer = gpu;
	}

	void PGEWindow::LinkToImageLoader(olc::imload::ImageLoader* imload)
	{
		pImageLoader = imload;
	}

	olc::Image& PGEWindow::GetDefaultImage()
	{
		return imgPrimary;
	}

	olc::Draw2D& PGEWindow::GetDraw()
	{
		return draw;
	}

	olc::hw::Mouse& PGEWindow::GetMouse()
	{
		return mouse;
	}

	olc::hw::Keyboard& PGEWindow::GetKeyboard()
	{
		return keyboard;
	}

	const olc::vi2d& PGEWindow::ScreenSize()
	{
		return GetDefaultImage().Size();
	}

	bool PGEWindow::olc_OnMouseMove(const olc::vi2d& vMousePos)
	{
		olc::vi2d pos = vMousePos;

		// TODO: Concept in v2d prevents this from being cleaner
		// Full screen windows may have different scaling
		pos.x -= vViewPos.x;
		pos.y -= vViewPos.y;

		// Scale mouse into view coordinates
		mouse.SetPosition(
			(olc::vf2d(pos) / olc::vf2d(vWindowSize - (vViewPos * 2)) * GetDefaultImage().Size())
			.clamp({ 0.0f, 0.0f }, olc::vf2d(GetDefaultImage().Size()-1)));
		return true;
	}

    bool PGEWindow::olc_OnTouchMove(const int nTouch, const olc::vf2d &vPos)
    {
		olc::vf2d pos = vPos;

		// TODO: Concept in v2d prevents this from being cleaner
		// Full screen windows may have different scaling
		pos.x -= static_cast<float>(vViewPos.x);
		pos.y -= static_cast<float>(vViewPos.y);

		touch.SetPosition(
			nTouch,
			(olc::vf2d(pos) / olc::vf2d(vWindowSize - (vViewPos * 2)) * olc::vf2d(GetDefaultImage().Size()))
			.clamp({ 0.0f, 0.0f }, olc::vf2d(olc::vi2d(GetDefaultImage().Size()) - 1)));

        return true;
    }





	PixelGameEngine::PixelGameEngine() : PGEWindow()
	{
	}

	PixelGameEngine::~PixelGameEngine()
	{
	}

	bool PixelGameEngine::Construct(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize, bool bFullScreen)
	{
		config.vScreenSize = vScreenSize;
		config.vPixelSize = vPixelSize;
		config.bFullScreen = bFullScreen;
		return true;
	}

	bool PixelGameEngine::Construct(const PGEConfig& cfg)
	{		
		config = cfg;
		return true;
	}

	bool PixelGameEngine::Start()
	{
		// Initialise Host Interface
		#if OLC_HOST == OLC_HOST_WINDOWS
		host = std::make_unique<olc::host::Host_Windows_WinAPI>();
		#endif
		// Johnnyg63: Added MacOS Host Initialisation
		#if OLC_HOST == OLC_HOST_MACOS
		host = std::make_unique<olc::host::Host_Apple_MacOS>();
		#endif
		#if OLC_HOST == OLC_HOST_LINUX_X11
		host = std::make_unique<olc::host::Host_Linux_X11>();
		#endif
		#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
		host = std::make_unique<olc::host::Host_Linux_Wayland>();
		#endif
		#if OLC_HOST == OLC_HOST_EMSCRIPTEN
		host = std::make_unique<olc::host::Host_Web_Emscripten>();
        #endif
        #if OLC_HOST == OLC_HOST_ANDROID
        host = std::make_unique<olc::host::Host_Android>();
        auto hostPtr = (dynamic_cast<olc::host::Host_Android*>(host.get()));
        #endif
#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_NO
		// Create OS window on this thread
		host->AddWindowFrame(this, { 30,30 }, config.vPixelSize * config.vScreenSize, false);
		// Create EngineThread - no more windows will be created now. We needed one window
		// at least to initialise teh rendering subsystem... sigh.
		coreActive = true;

#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
		// Create EngineThread
		coreThread = std::thread(&PixelGameEngine::EngineThread, this);
		// Handle window events on this thread (and block)
		host->StartSystemEventLoop(true);		
		// Window has closed its event handler, so shut down gracefully
		coreActive = false;
		// Wait for engine thread to terminate
		coreThread.join();
#elif OLC_HOST == OLC_HOST_ANDROID
        // We need to wait for the APP_CMD_INIT_WINDOW command before starting the loop
        __android_log_print(ANDROID_LOG_DEBUG, "PGE ANDROID", "Initializing...");
        while (!hostPtr->IsInitialized()) {
			host->StartSystemEventLoop(false);
		}

        __android_log_print(ANDROID_LOG_DEBUG, "PGE ANDROID", "Initialized Successfully");

        EngineThread();
#else
        EngineThread();
#endif

#else
		
#endif
		
		return true;
	}

	float PixelGameEngine::FrameTimeElapsed() const
	{
		return durationFrame.count();
	}

	double PixelGameEngine::TotalTimeElapsed() const
	{
		return durationTotalElapsed.count();
	}

	bool PixelGameEngine::AddChildWindow(std::shared_ptr<olc::PGEWindow> window, const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize)
	{
#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
		// Link this olc::Window to a host resource
		if (host)
		{
			// TODO: Connect to config

			host->AddWindowFrame(window.get(), { 30,30 }, vScreenSize * vPixelSize, false);
			window->LinkToHost(host.get());
			window->LinkToRenderer(gpu.get());
			window->LinkToImageLoader(imageloader.get());

			//gpu->RetargetDevice(host->GetHostWindowDescriptor(window.get()));
			window->Create(vScreenSize, vPixelSize);
			
			deqChildWindows.push_back(window);
		}
		return true;
#else
		// Can't create new windows
		olc_IgnoreUnused(window, vScreenSize, vPixelSize);
		return false;
#endif
	}

	void PixelGameEngine::CoreUpdate(void* userdata)
	{
		using namespace std::chrono_literals;
		auto pge = reinterpret_cast<olc::PixelGameEngine*>(userdata);	
#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
			// Multiwindow system uses one event loop (non blocking) for all windows
			pge->host->StartSystemEventLoop(false);
#endif

			// Frame Delta Timing - "ElapsedTime" since last core update
			// ~~~~~~~~~~~~~~~~~~
			// All timing is synchronous to the primary window, i.e child
			// windows do not maintain their own frame timing. This is a
			// deliberate decision as PGE will maintain sync between windows.
			// Why? Multiple windows are not really the point of PGE and
			// indeed could just be addional unnecessary complexity. However,
			// by moving to a Window abstraction we kinda get it for free.
			// This freedom comes at the expense of complexity. If we allowed
			// windows to be wholly isolated from the core loop, then the
			// user is expected to maintain thread safety, context sharing
			// and resource management. That is not olc::PGE.

			pge->timeFrame1 = std::chrono::steady_clock::now();
			pge->durationFrame = pge->timeFrame1 - pge->timeFrame2;
			pge->timeFrame2 = pge->timeFrame1;

			pge->durationTotalElapsed += pge->durationFrame;

			// Our time per frame coefficient
			float fDT = pge->durationFrame.count();
			
			// Our Total Time accumulator
			float fTT = float(pge->durationTotalElapsed.count());

			pge->frameCount++;
			pge->durationFrameCount += pge->durationFrame;
			
			if (pge->durationFrameCount >= 1s)
			{
				pge->durationFrameCount -= 1s;
				std::string sTitle = "OneLoneCoder.com - Pixel Game Engine 3 - Test - FPS: " + std::to_string(pge->frameCount);
				pge->SetWindowTitle(sTitle);
				pge->frameCount = 0;
			}
				
			
			
			// Primary Window
			if (pge->olc_ShouldRemove())
			{
				// Application is to be terminated as primary window has closed
				pge->coreActive = false;
#if OLC_HOST == OLC_HOST_EMSCRIPTEN
				emscripten_cancel_main_loop();
#endif
			}
			else
			{
#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
				// Update Child Windows (if any)
				for (auto& winChild : deqChildWindows)
					winChild->olc_WindowUpdate(fDT, fTT);

				// Remove child windows that have requested closure
				if (!deqChildWindows.empty())
				{
					deqChildWindows.erase(std::remove_if(deqChildWindows.begin(), deqChildWindows.end(),
						[this](const std::shared_ptr<PGEWindow>& w)
						{
							if (w->olc_ShouldRemove())
							{
								host->CloseWindowFrame(w.get());
							}
							return w->olc_ShouldRemove();
						}
					), deqChildWindows.end());
				}
#endif

				// Update Primary Window
				pge->olc_WindowUpdate(fDT, fTT);

				// Wait for vertical sync if required. 
				// Note: Child windows will never vsync as waiting for each buffer swap with vsync
				// divides up the frame rate budget across the windows.
				if (pge->gpu->GetConfig().VerticalSync)
				{
					pge->host->SyncWithDesktopComposite();
				}
			}
		
	}

    void PixelGameEngine::EngineThread()
    {
        using namespace std::chrono_literals;
        timeFrame2 = std::chrono::steady_clock::now();
        timeFrame1 = std::chrono::steady_clock::now();

#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
        // Create Primary Window on EngineThread, event loop also exists for all windows
        // on this thread, and all windows will be created on this thread
        host->AddWindowFrame(this, { 30,30 }, config.vPixelSize * config.vScreenSize, false);
#endif

        // Initialise ImageLoader Interface
#if OLC_HOST == OLC_HOST_WINDOWS
        imageloader = std::make_unique<olc::imload::ImageLoader_WinGDI>();
#endif

        // Initialise ImageLoader Interface
#if OLC_HOST == OLC_HOST_MACOS
        imageloader = std::make_unique<olc::imload::ImageLoader_MacOS>();
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
        imageloader = std::make_unique<olc::imload::ImageLoader_LibPNG>();
#endif

#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
        imageloader = std::make_unique<olc::imload::ImageLoader_LibPNG>();
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
        imageloader = std::make_unique<olc::imload::ImageLoader_LibPNG>();
#endif

#if OLC_HOST == OLC_HOST_ANDROID
        imageloader = std::make_unique<olc::imload::ImageLoader_NDKImageDecoder>(
            olc::host::Host_Android::androidApp->activity->assetManager
        );
#endif

        // Initialise GPU Interface	- This thread is the context
        olc::gpu::RendererConfig cfgRenderer;
        cfgRenderer.VerticalSync = config.bVSync;

        gpu = std::make_unique<olc::gpu::Renderer_OGL33>();

        // Link this windows devices
        LinkToHost(host.get());
        LinkToRenderer(gpu.get());
        LinkToImageLoader(imageloader.get());

        // The GPU device can be based upon the primary window configuration. This
        // gives us completed gpu and host objects to pass to other windows as and
        // when required
        gpu->CreateDevice(host->GetHostWindowDescriptor(this), cfgRenderer);
        if (gpu->GetLastError() != olc::gpu::RendererError::NoError)
        {
            //const auto e = gpu->GetLastError(); // For debug visibility
            std::cout << "Error: Could not create Renderer\n";
            return;
        }



        olc::ImageConfig cfg;
        cfg.MSAA = config.bAntiAliasMainScreen;
        CreateImage(GetDefaultImage(), config.vScreenSize, cfg);


        // Initialise Font System
        olc::pgeguts::CreateClassicFont(this);


        draw.SetGPU(gpu.get());
        gpu->ApplyDefaultShader();
        draw.SetTarget(GetDefaultImage());

        if (!OnUserCreate())
        {
            // Creation process signalled abort
            return;
        }

        draw.ProcessGPUTasks();
        draw.SetTarget(GetDefaultImage());

        // Initialise Input Devices


        durationFrameCount = 0s;

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
        emscripten_set_main_loop_arg(PixelGameEngine::CoreUpdate, reinterpret_cast<void*>(this), 0, 1);
#else
        while (coreActive)
        {
#if OLC_HOST == OLC_HOST_ANDROID
            if (!host->StartSystemEventLoop(false)) {
                coreActive = false;
            }
#endif
            PixelGameEngine::CoreUpdate(this);
        }
#endif
    }

}
//! END IMPLEMENTATION