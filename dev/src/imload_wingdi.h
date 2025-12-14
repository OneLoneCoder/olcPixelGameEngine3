#pragma once

//! START CUSTOMHEADER
#include "imload_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <filesystem>
//! END STDHEADER

//! START WINAPI_CONFIG 
#if OLC_HOST == OLC_HOST_WINDOWS	// Johnnyg63: Add guard for Windows only
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
//#if !defined(_WIN32_WINNT)
//	#ifdef HAVE_MSMF
//		#define _WIN32_WINNT 0x0600 // Windows Vista
//	#else
//		#define _WIN32_WINNT 0x0500 // Windows 2000
//	#endif
//#endif

// Embrace MSVC superiority
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")
#include <objidl.h>
#include <gdiplus.h>
#include <gdiplusinit.h>
#include <shlwapi.h>
#undef _WINSOCKAPI_
#endif
//! END WINAPI_CONFIG

//! START DECLARATION
#if OLC_HOST == OLC_HOST_WINDOWS
#if !defined(PGE_IMAGELOADER_WINGDI_DECLARED)
namespace olc
{
	namespace imload
	{
		class ImageLoader_WinGDI : public ImageLoader
		{
		public:		
			
			// Create an image resource based on an image file asset on disk
			bool CreateImageFromFile(olc::Image& image, const std::string& sFileName) override;

			// Create an image resource based on an image file asset in memory
			bool CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes) override;

			// Create an image resource based on an image file asset in memory
			bool CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data) override;

			// Store an image as a file asset on disk
			bool WriteImageToFile(const olc::Image& image, const std::string& sFileName) override;

			// Store an image as a file asset in memory
			bool WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data) override;

		};
	}
}

#define PGE_IMAGELOADER_WINGDI_DECLARED 1
#endif
#endif // OLC_HOST == OLC_HOST_WINDOWS
//! END DECLARATION

