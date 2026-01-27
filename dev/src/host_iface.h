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
		#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_ANDROID
		inline size_t CreateUID()
		{
			return uuid++;
		}		
		#endif
	}

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

		public: 
			virtual bool StartSystemEventLoop(bool bBlockIfPossible = false) = 0;
			virtual bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) = 0;
			virtual bool CloseWindowFrame(olc::Window* pWindow) = 0;
			virtual bool UpdateWindowFrameTitle(olc::Window* pWindow) = 0;

			virtual std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) = 0;
			
			
			virtual bool ConnectHostResourceToRenderer() = 0;

			// Wait for entire host desktop refresh (for smooooth vsync)
			virtual bool SyncWithDesktopComposite() = 0;

		protected:
			HostError lastError = HostError::None;
		};
	}
}
#define PGE_HOST_IFACE_DECLARED 1
#endif
//! END DECLARATION