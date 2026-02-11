#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
#include <optional>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "window.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_HOST_IFACE_DECLARED)
namespace olc
{
	namespace pgeguts
	{
		inline static size_t uuid = 0;

		#if OLC_HOST == OLC_HOST_WINDOWS
		inline constexpr size_t CreateUID()
		{
			return uuid++;
		}
		#endif
		#if OLC_HOST == OLC_HOST_LINUX_X11
		inline size_t CreateUID()
		{
			return uuid++;
		}
		#endif
		#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
		inline size_t CreateUID()
		{
			return uuid++;
		}
		#endif
		#if OLC_HOST == OLC_HOST_MACOS
		// Clang compiler on MacOS requires constexpr to be removed
		inline size_t CreateUID()
		{
			return uuid++;
		}
		#endif
		#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_ANDROID || OLC_HOST == OLC_HOST_LINUX_DRM
		inline size_t CreateUID()
		{
			return uuid++;
		}		
		#endif
		// If this blocks looks funny it's because it is truly hilarious that the core dev team
		// thought this was a good idea. We need a voice of reason, clearly.
	}

	class PixelGameEngine;

	namespace host
	{

		

		struct HostConfig
		{
			
		};

		enum class HostError
		{
			None,
			
		};

		class Host
		{
		public:
			Host() = default;
			virtual ~Host() {};

		public:
			// Check/Get last error
			HostError GetLastError() const { return lastError; }

		public: // OS Window Handling
			// Make OS Create a window frame, associated with olc::Window
			virtual bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) = 0;			
			// Make OS Close a window frame, associated with olc::Window
			virtual bool CloseWindowFrame(olc::Window* pWindow) = 0;
			// Make OS Update a window frame title, associated with olc::Window
			virtual bool UpdateWindowFrameTitle(olc::Window* pWindow) = 0;
			// Get OS-specific window descriptor(s) for given olc::Window
			virtual std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) = 0;
			// Wait for OS desktop refresh (for smooooth vsync)
			virtual bool SyncWithDesktopComposite() = 0;

		public: // OS Specific Environment Information
			virtual olc::KeyboardLayout GetKeyboardLayout() const = 0;

		public: // Platform Specific OS<->PGE Linkage
			// Called at very start of application
			virtual bool OnApplicationStart(olc::PixelGameEngine* pPrimary) = 0;
			// Called to start the host - this may mean different things on different hosts
			// It MUST block until system is requested to exit
			virtual bool StartSystem() = 0;
			// Called to stop the host, and shutdown all resources
			virtual bool StopSystem() = 0;
			// Called at start of system event loop
			virtual bool OnSystemThreadStart() = 0;
			// Called to perform primary window update
			virtual bool OnSystemTick() = 0;
			// Called at end of system event loop
			virtual bool OnSystemThreadEnd() = 0;
			// Called at very end of application
			virtual bool OnApplicationEnd() = 0;

		protected:
			HostError lastError = HostError::None;
			olc::PixelGameEngine* pPrimaryPGE = nullptr;
		};
	}
}
#define PGE_HOST_IFACE_DECLARED 1
#endif
//! END DECLARATION