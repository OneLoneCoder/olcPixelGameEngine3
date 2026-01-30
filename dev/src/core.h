#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <deque>
#include <chrono>
#include <thread>
//! END STDHEADER GLOBAL

//! START CUSTOMHEADER GLOBAL
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "window.h"
#include "gpu_iface.h"
#include "host_iface.h"
#include "imload_iface.h"
#include "font.h"
//! END CUSTOMHEADER GLOBAL

//! START DECLARATION
#if !defined(PGE_CORE_DECLARED)

namespace olc
{
	// A grouping of all settable PGE properties
	struct PGEConfig
	{
		// Size of "screen" in PGE pixels
		olc::vi2d vScreenSize = { 256, 240 };
		// Size of a PGE pixel
		olc::vi2d vPixelSize = { 4, 4 };
		// Top left location of shown main window
		olc::vi2d vWindowOffset = { 30,30 };
		// Start in full-screen mode
		bool bFullScreen = false;
		// Allow full screen as an option with ALT-ENTER
		bool bFullScreenable = true;
		// Allow the window to be resized by user
		bool bResizeable = true;
		// Synchronise rendering with monitor
		bool bVSync = OLC_DEFAULT_VSYNC;
		// Behave like a host window, resizing the screen in response to window resize
		bool bRealWindow = false;
		// Ensure aspect ratio of "screen" is mainatined regardless of window size
		bool bRetainAspectRatio = true;
		// Force "screen" pixels to be integer in size
		bool bForceIntegerPixelSize = false;
		// Allow main PGE window to spawn child windows
		bool bAllowChildWindows = true;
		// Creates a "DefaultImage" with anti-aliased properties
		bool bAntiAliasMainScreen = false;
		// Default clear colour for the primary drawing surface
		olc::Pixel colClear = olc::Colour::BLACK;
	};

	// A PGE Window is a window with drawing and input capabilities a la olc::PixelGameEngine
	class PGEWindow : public Window
	{
	public:
		bool Create(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize);
	
	public:
		// Return true if window is to continue
		virtual bool OnUserCreate();
		// Return true if window is to continue
		virtual bool OnUserUpdate(float fElapsedTime);
		// Return true if window is to close
		virtual bool OnUserDestroy();
		
	
	public:	// olc::Image Handling
		// Create an image resource
		bool CreateImage(olc::Image& image, const olc::vi2d& size, const ImageConfig& cfg = olc::ImageConfig());
		// Create an image resource based on an image file asset on disk
		bool CreateImageFromFile(olc::Image& image, const std::string& sFileName, const ImageConfig& cfg = olc::ImageConfig());
		// Create an image resource based on an image file asset in memory
		bool CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes, const ImageConfig& cfg = olc::ImageConfig());
		// Store an image as a file asset on disk
		bool WriteImageToFile(const olc::Image& image, const std::string& sFileName);
		// Store an image as a file asset in memory
		//bool WriteImageToMemory(const olc::Image& image, std::vector<uint8_t> bytes, const std::string& sFileName);
		// Destroy an image
		void DestroyImage(olc::Image& image);

	public:
		void LinkToRenderer(olc::gpu::Renderer* gpu);
		void LinkToImageLoader(olc::imload::ImageLoader* imload);

	public:
		// Returns the image that represents the primary drawing surface
		olc::Image& GetDefaultImage();
		olc::Draw2D& GetDraw();

		// Input devices are handled by a regular olc::Window, but for convenience...
		olc::hw::Mouse& GetMouse();
		olc::hw::Keyboard& GetKeyboard();
		
		// Returns the current size of the "screen" in pixels
		const olc::vi2d& ScreenSize();

	protected:
		bool olc_OnMouseMove(const olc::vi2d& vMousePos) override;
		bool olc_OnTouchMove(const int nTouch, const olc::vf2d& vPos) override;

	public:
		virtual bool olc_WindowUpdate(const float fElapsedTime, const float fTotalElapsedTime);

	protected:
		olc::Draw2D draw;
		
		
	private:
		olc::Image imgPrimary;
		olc::gpu::Renderer* pRenderer = nullptr;
		olc::imload::ImageLoader* pImageLoader = nullptr;
		olc::vi2d vViewPos = { 0,0 };
		olc::vi2d vViewSize = { 0,0 };

	protected:
		// PGE Configuration
		PGEConfig config;
	};

	// The olc::PixelGameEngine3 core, manages the main window, child windows, engine loop, timing and devices
	class PixelGameEngine : public PGEWindow
	{
	public:
		PixelGameEngine();
		virtual ~PixelGameEngine();

		PixelGameEngine(const PixelGameEngine&) = delete;
		PixelGameEngine& operator=(const PixelGameEngine&) = delete;

	public:
		// Construct the PGE main engine window with traditional parameters
		bool Construct(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize, bool bFullScreen = false);
		// Construct the PGE main engine window with verbose configuration structure
		bool Construct(const PGEConfig& cfg = PGEConfig{});
		
		// Start the PGE main engine loop (on its own thread)
		bool Start();


	public:
		float FrameTimeElapsed() const;
		double TotalTimeElapsed() const;

	public: // Child Windows
		bool AddChildWindow(std::shared_ptr<olc::PGEWindow> window, const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize);
	
	public: // Core Update
		static void CoreUpdate(void* userdata);
		
	private:
		// Window Management
		std::deque<std::shared_ptr<PGEWindow>> deqChildWindows;

		// Frame Timing & Overall Clocking
		std::chrono::steady_clock::time_point timeFrame1;
		std::chrono::steady_clock::time_point timeFrame2;
		std::chrono::duration<float> durationFrame{ 0 };
		std::chrono::duration<float> durationFrameCount{ 0 };
		std::chrono::duration<double> durationTotalElapsed{ 0 };
		size_t frameCount = 0;

		// Core Thread
		std::thread coreThread;
		std::atomic<bool> coreActive;
		void EngineThread();

		// These interfaces are created dynamically by the PGE core
		// after the environment is understood (or specified by config)
		std::unique_ptr<olc::gpu::Renderer> gpu;
		std::unique_ptr<olc::host::Host> host;
		std::unique_ptr<olc::imload::ImageLoader> imageloader;
	};
}
#define PGE_CORE_DECLARED 1
#endif
//! END DECLARATION
