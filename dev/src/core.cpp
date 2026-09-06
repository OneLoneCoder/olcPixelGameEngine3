#include "core.h"

#if OLC_GPU == OLC_GPU_NONE
#include "gpu_none.h"
#endif

#if OLC_GPU == OLC_GPU_OPENGL33
#include "gpu_opengl33.h"
#endif

#if OLC_HOST == OLC_HOST_NONE
#include "host_none.h"
#endif

#if OLC_HOST == OLC_HOST_WINDOWS
#include "host_win_winapi.h"
#endif

// Johnnyg63: Added define for MACOS/iOS
#if OLC_HOST == OLC_HOST_MACOS
#include "host_apple_macos.h"
#endif

#if OLC_HOST == OLC_HOST_IOS
#include "host_apple_ios.h"
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
#include "host_lin_x11.h"
#endif

#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
#include "host_lin_wayland.h"
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
#include "host_web_emscripten.h"
#endif

#if OLC_HOST == OLC_HOST_ANDROID
#include "host_android.h"
#endif

#if OLC_IMAGELOADER == OLC_IMAGELOADER_WINGDI
#include "imload_wingdi.h"
#endif

#if OLC_IMAGELOADER == OLC_IMAGELOADER_MACOS
#include "imload_macos.h"
#endif

#if OLC_IMAGELOADER == OLC_IMAGELOADER_IOS
#include "imload_ios.h"
#endif

#if OLC_IMAGELOADER == OLC_IMAGELOADER_LIB_PNG
#include "imload_lib_png.h"
#endif

#if OLC_IMAGELOADER == OLC_IMAGELOADER_NDK_IMAGEDECODER
#include "imload_android.h"
#endif

#if OLC_IMAGELOADER == OLC_IMAGELOADER_STB_IMAGE
#include "imload_stb_image.h"
#endif

//! START IMPLEMENTATION
namespace olc
{
	PGEWindow::PGEWindow() : Window(), draw()
	{
	}

	PGEWindow::PGEWindow(const WindowConfig& config) : Window(config), draw()
	{
	}

	bool PGEWindow::Create(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize)
	{
		//pRenderer->RetargetDevice(pHost->GetHostWindowDescriptor(this));
		pRenderer->PrepareWindowTarget(pHost->GetHostWindowDescriptor(this));
		CreateImage(GetScreen(), vScreenSize);
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
		float fDT = fElapsedTime;

		// Input Changes
		mouse.UpdateState();
		keyboard.UpdateState();
		touch.UpdateState();
		
		draw.SetGPU(pRenderer);
		draw.SetTarget(GetScreen());

		pRenderer->DisplayPrepare(fDT, fTotalElapsedTime);

#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
		pRenderer->RetargetDevice(pHost->GetHostWindowDescriptor(this));
#endif
		pRenderer->ApplyDefaultShader();

		bool bBlockUserUpdate = false;
		for (const auto& pgex : vecWindowExtensions)
		{
			bBlockUserUpdate |= pgex->OnBeforeUserUpdate(this, fDT);
		}

		if (!bBlockUserUpdate)
		{
			// User Update
			if (!OnUserUpdate(fDT) || bRequestToClose)
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

			if (GetScreen().GetConfig().MSAA)
			{
				pRenderer->ResolveMSAA(uint32_t(GetScreen().GetGPUID()));
			}

			draw.ResetShader();
			draw.WorldReset();		
		}

		for (const auto& pgex : vecWindowExtensions)
		{
			if (pgex->OnAfterUserUpdate(this, fDT))
			{
				// Finialise any outstanding tasks
				draw.ProcessGPUTasks();

				if (GetScreen().GetConfig().MSAA)
				{
					pRenderer->ResolveMSAA(uint32_t(GetScreen().GetGPUID()));
				}

				draw.ResetShader();
				draw.WorldReset();				
			}
		}



		// === Viewport Handling ===
		if (config.bRetainAspectRatio)
		{
			// Set the viewport to maintain the aspect ratio of GetDefaultImage() and maximise to 
			// fit within the window client area
			float fAspectScreen = float(GetScreen().Size().x) / float(GetScreen().Size().y);

			
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

		// Take the window's completed "screen" and draw it as a textured quad to the backbuffer
		pRenderer->AssignTextureTarget(0, 0);

		// Present final composite
		pRenderer->SetViewport(vViewPos, vViewSize);
		pRenderer->ClearViewport(config.colClear, true, true);
		draw.SetBlendMode(olc::BlendMode::Alpha);
		draw.SetCullMode(olc::CullMode::None);
		draw.ImageRect(GetScreen().flipV(), { 0.0,0.0 }, vViewSize);
		draw.ProcessGPUTasks();

		// Update Window's primary surface
		//pRenderer->RetargetDevice(pHost->GetHostWindowDescriptor(this));
		pRenderer->DisplayDraw(pHost->GetHostWindowDescriptor(this));

		return true;
	}

	bool PGEWindow::InstallWindowExtension(olc::PGEWindowExtension* pgex)
	{
		vecWindowExtensions.push_back(pgex);
		return pgex->OnInstall(this);
	}

	bool PGEWindow::CreateImage(olc::Image& image, const olc::vi2d& size, const ImageConfig& cfg)
	{
		// Create CPU Image
		if (!image.CreateNoGPU(size, cfg))
			return false;

		// Create GPU Image
		auto id = pRenderer->CreateTexture(image.Size(), cfg);
		if (id == 0)
		{
			image.CreateNoGPU({ 0,0 });
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
				image.CreateNoGPU({ 0,0 });
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
		if (pImageLoader->CreateImageFromMemory(image, data, bytes))
		{
			// Image has loaded ok, and populated into pixel vector
			// 
			// Create GPU Image
			auto id = pRenderer->CreateTexture(image.Size(), cfg);
			if (id == 0)
			{
				image.CreateNoGPU({ 0,0 });
				return false;
			}

			// Associate CPU object with GPU Resource
			image.SetGPUID(id);
			return true;
		}

		std::cout << "Create From Memory Failed\n";
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
		image.CreateNoGPU({ 0,0 });
	}

	void PGEWindow::LinkToRenderer(olc::gpu::Renderer* gpu)
	{
		pRenderer = gpu;
	}

	void PGEWindow::LinkToImageLoader(olc::imload::ImageLoader* imload)
	{
		pImageLoader = imload;
	}

	olc::Image& PGEWindow::GetScreen()
	{
		return imgPrimary;
	}

	olc::Draw& PGEWindow::GetDraw()
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
		return GetScreen().Size();
	}

	void PGEWindow::SetMousePosition(const olc::vi2d& vPos)
	{
		// Scale mouse from view coordinates into window coordinates
		olc::vi2d pos = (olc::vf2d(vPos) / olc::vf2d(GetScreen().Size()) * olc::vf2d(vViewSize)) + vViewPos;
		SetWindowMousePosition(pos);
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
			(olc::vf2d(pos) / olc::vf2d(vWindowSize - (vViewPos * 2)) * GetScreen().Size())
			.clamp({ 0.0f, 0.0f }, olc::vf2d(GetScreen().Size()-1)));
		return true;
	}

	bool PGEWindow::olc_OnTouch(const uint32_t nID, const olc::vf2d& vPos, const bool bPress, const bool bRelease, const olc::vf2d& vSize, const bool stylus, const float pressure, const float orientation, const olc::vf2d& tilt)
	{
		olc::vf2d pos = vPos;
		pos.x -= float(vViewPos.x);
		pos.y -= float(vViewPos.y);

		olc::vf2d vScale = (1.0f / olc::vf2d(vWindowSize - (vViewPos * 2))) * GetScreen().Size();

		touch.UpdateTouch(nID, pos * vScale, 
			bPress, bRelease, vSize * vScale, stylus, pressure, orientation, tilt);
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
		return Construct(config);
	}

	bool PixelGameEngine::Construct(const PGEConfig& cfg)
	{		
		config = cfg;
		// Also assign the window level config since that is what the Host will see
		Window::config = cfg;

		// Check for constructor sAppName, if not set use Config sAppName
		if (sAppName.empty())
		{
			sAppName = config.sAppName;
		}
		else
		{
			if (config.sAppName == "PGE3")
			{
				// User has not set Config sAppName to something specific, so set the Config sAppName to constructor sAppName
				config.sAppName = sAppName;
			}
			else
			{
				// User has set config.sAppName therefore we override constructor sAppname , 
				sAppName = config.sAppName;
			}
		}

		// This is the earliest point within familiar PGE ecosystem
		// where we can instatiate the host interface.

		// Initialise Host Interface
		host = std::make_unique<olc::host::OLC_FRIENDLY_HOST>();


		// DEVS!! Please don't merge these just yet

		// Initialise ImageLoader Interface
		imageloader = std::make_unique<olc::imload::OLC_IMAGELOADER_CLASS>();

		// Allow host to prepare itself
		return host->OnApplicationStart(this);
	}

	bool PixelGameEngine::Start()
	{		
		

		bool bStartCheck = host->StartSystem(); // Must block until system is shutdown
		if(!bStartCheck)
			std::cout << "PGE Start() Error: Host failed to start\n";

		bool bEndCheck = host->OnApplicationEnd();
		if (!bEndCheck)
			std::cout << "PGE Start() Error: Host failed to shutdown cleanly\n";

		return bStartCheck && bEndCheck;
	}

	bool PixelGameEngine::OnPreContextStart()
	{
		// Create the window!
		return host->AddWindowFrame(this, { 30,30 }, config.vPixelSize * config.vScreenSize, false);		
	}

	bool PixelGameEngine::OnContextStart()
	{
		// The "context" refers to the thread (or execution pathway)
		// that will be hosting the main rendering loop. Things like
		// OpenGL contexts are thread specific, so all GPU initialisation
		// must be performed here for example

		// If the host doesnt use a thread for its event loop, then
		// this function will probably be called on the main application 
		// thread


		// Initialise GPU Interface
		olc::gpu::RendererConfig cfgRenderer;
		cfgRenderer.VerticalSync = config.bVSync;
		gpu = std::make_unique<olc::gpu::OLC_GPU_CLASS>();

		gpu->CreateDevice(host->GetHostWindowDescriptor(this), cfgRenderer);
		if (gpu->GetLastError() != olc::gpu::RendererError::NoError)
		{
			std::cout << "PGE OnContextStart() Error: Could not create Renderer\n";
			return false;
		}

		// Link this olc::Window to host
		LinkToHost(host.get());
		// Link this olc::PGEWindow to renderer and imageloader
		LinkToRenderer(gpu.get());
		LinkToImageLoader(imageloader.get());

		// These things require a valid GPU context

		// Create Primary olc::Image - aka "The Screen"
		olc::ImageConfig cfg;
		cfg.MSAA = config.bAntiAliasMainScreen;
		CreateImage(GetScreen(), config.vScreenSize, cfg);

		// Initialise "Classic" Font System
		olc::pgeguts::CreateClassicFont(this);

		// Prepare Draw2D system
		draw.SetGPU(gpu.get());
		gpu->ApplyDefaultShader();
		draw.SetTarget(GetScreen());

		for (const auto& pgex : vecSystemExtensions)
		{
			if(!pgex->OnBeforeUserCreate(this))
			{
				std::cout << "PGE OnContextStart(): User aborted in extension OnBeforeUserCreate()\n";
				return false;
			}
		}

		// It's possible to draw things in create so flush any pending GPU tasks
		draw.ProcessGPUTasks();

		// Set to known default state
		draw.SetTarget(GetScreen());
		draw.WorldReset();
		gpu->ApplyDefaultShader();

		// User Create GOOOOOOOOO!!!!
		if (!OnUserCreate())
		{
			std::cout << "PGE OnContextStart(): User aborted OnUserCreate()\n";
			return false;
		}

		// It's possible to draw things in create so flush any pending GPU tasks
		draw.ProcessGPUTasks();

		// Set to known default state
		draw.SetBlendMode(olc::BlendMode::Alpha);
		draw.SetCullMode(olc::CullMode::None);
		draw.SetTarget(GetScreen());
		draw.WorldReset();
		gpu->ApplyDefaultShader();

		for (const auto& pgex : vecSystemExtensions)
		{
			if (!pgex->OnAfterUserCreate(this))
			{
				std::cout << "PGE OnContextStart(): User aborted in extension OnAfterUserCreate()\n";
				return false;
			}
		}

		// It's possible to draw things in create so flush any pending GPU tasks
		draw.ProcessGPUTasks();

		// Set to known default state
		draw.SetTarget(GetScreen());
		draw.WorldReset();
		gpu->ApplyDefaultShader();

		// Fire up the engine!
		using namespace std::chrono_literals;
		durationFrameCount = 0s;
		durationTotalElapsed = 0s;
		timeFrame1 = std::chrono::steady_clock::now();
		timeFrame2 = std::chrono::steady_clock::now();
		frameCount = 0;
		return true;
	}

	bool PixelGameEngine::OnContextTick()
	{
		// This is called once per frame from the host's system loop

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

		using namespace std::chrono_literals;
		
		// Calculate last frame time
		timeFrame1 = std::chrono::steady_clock::now();
		durationFrame = timeFrame1 - timeFrame2;
		timeFrame2 = timeFrame1;

		// Accumulate total time elapsed since application start
		durationTotalElapsed += durationFrame;

		// Our time per frame coefficient
		float fDT = durationFrame.count();

		// Our Total Time accumulator
		float fTT = float(durationTotalElapsed.count());

		// Calculate FPS every second
		frameCount++;
		durationFrameCount += durationFrame;
		if (durationFrameCount >= 1s)
		{
			durationFrameCount -= 1s;
			std::string sTitle = "OneLoneCoder.com - Pixel Game Engine 3 - " + sAppName + " - FPS: " + std::to_string(frameCount);
			SetWindowTitle(sTitle);
			fps = frameCount;
			frameCount = 0;
		}

		// If primary window is to be closed, signal application termination
		if (olc_ShouldRemove())
		{
			return false; // Terminate application
		}
		else
		{
			for (const auto& pgex : vecSystemExtensions)
			{
				if (!pgex->OnBeforeSystemUpdate(this, fDT))
				{
					std::cout << "PGE OnContextTick(): User aborted in extension OnBeforeSystemUpdate()\n";
					return false;
				}
			}

			// Update Primary Window
			olc_WindowUpdate(fDT, fTT);

			for (const auto& pgex : vecSystemExtensions)
			{
				if (!pgex->OnAfterSystemUpdate(this, fDT))
				{
					std::cout << "PGE OnContextTick(): User aborted in extension OnAfterSystemUpdate()\n";
					return false;
				}
			}

			// Wait for vertical sync with desktop compositor if required. 
			
			// Note: Child windows will never vsync as waiting for each buffer swap with vsync
			// divides up the frame rate budget across the windows.
			if (gpu->GetConfig().VerticalSync)
			{
				host->SyncWithDesktopComposite();
			}
		}

		return true; // Keep going
	}

	bool PixelGameEngine::OnContextEnd()
	{
		return true;
	}

	bool PixelGameEngine::OnPostContextEnd()
	{
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

	size_t PixelGameEngine::GetFPS() const
	{
		return fps;
	}

	bool PixelGameEngine::AddChildWindow(std::shared_ptr<olc::PGEWindow> window, const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize)
	{
#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
		// Link this olc::Window to a host resource
		if (host)
		{
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

	bool PixelGameEngine::InstallSystemExtension(olc::PGESystemExtension* pgex)
	{
		vecSystemExtensions.push_back(pgex);
		return pgex->OnInstall(this);
	}

}
//! END IMPLEMENTATION


//	void PixelGameEngine::CoreUpdate(void* userdata)
//	{
//		
//		auto pge = reinterpret_cast<olc::PixelGameEngine*>(userdata);	
//#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
//			// Multiwindow system uses one event loop (non blocking) for all windows
//			pge->host->StartSystemEventLoop(false);
//#endif
//
//			
//		
//	}	
	
//	void PixelGameEngine::EngineThread()
//	{
//		using namespace std::chrono_literals;
//		timeFrame2 = std::chrono::steady_clock::now();
//		timeFrame1 = std::chrono::steady_clock::now();
//
//#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
//		// Create Primary Window on EngineThread, event loop also exists for all windows
//		// on this thread, and all windows will be created on this thread
//		host->AddWindowFrame(this, { 30,30 }, config.vPixelSize * config.vScreenSize, false);
//#endif
//		
//



// Javid dump for later

// //#if OLC_MULTIWINDOW == OLC_MULTIWINDOW_YES
//			// Update Child Windows (if any)
//			for (auto& winChild : deqChildWindows)
//				winChild->olc_WindowUpdate(fDT, fTT);
//
//			// Remove child windows that have requested closure
//			if (!deqChildWindows.empty())
//			{
//				deqChildWindows.erase(std::remove_if(deqChildWindows.begin(), deqChildWindows.end(),
//					[this](const std::shared_ptr<PGEWindow>& w)
//					{
//						if (w->olc_ShouldRemove())
//						{
//							host->CloseWindowFrame(w.get());
//						}
//						return w->olc_ShouldRemove();
//					}
//				), deqChildWindows.end());
//			}
//#endif