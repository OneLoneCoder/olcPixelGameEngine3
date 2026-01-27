#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER
 
//! START WINAPI_CONFIG 
#if defined(UNICODE) || defined(_UNICODE)
	#define olcT(s) L##s
#else
	#define olcT(s) s
#endif

#define _WINSOCKAPI_

#if !defined(VC_EXTRALEAN)
#define VC_EXTRALEAN
#endif

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

// In Code::Blocks
#if !defined(_WIN32_WINNT)
	#ifdef HAVE_MSMF
		#define _WIN32_WINNT 0x0600 // Windows Vista
	#else
		#define _WIN32_WINNT 0x0500 // Windows 2000
	#endif
#endif

// Embrace MSVC superiority
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Dwmapi.lib")

#include <dwmapi.h>
#include <windows.h>
#undef _WINSOCKAPI_
//! END WINAPI_CONFIG

//! START DECLARATION
namespace olc
{
	namespace host
	{



		class Host_Windows_WinAPI : public olc::host::Host
		{
			

		public:
			Host_Windows_WinAPI();
			virtual ~Host_Windows_WinAPI() {};


		public:
			bool StartSystemEventLoop(bool bBlockIfPossible = false);
			bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen);			
			bool CloseWindowFrame(olc::Window* pWindow);
			bool UpdateWindowFrameTitle(olc::Window* pWindow);
			
			std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow);
			bool ConnectHostResourceToRenderer();


			// Wait for entire host desktop refresh (for smooooth vsync)
			bool SyncWithDesktopComposite();

			LRESULT OnWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

			olc::KeyboardLayout GetKeyboardLayout() const override;

			std::string test;

		private:
			std::unordered_map<size_t, HWND> mapUID2HWND;
			std::unordered_map<HWND, olc::Window*> mapHWND2PTR;
			std::wstring ConvertS2W(std::string s);

			// Map of system keycodes to olc::Keycodes
			std::unordered_map<int32_t, olc::Key> mapKeys;

		};
	}
}
//! END DECLARATION