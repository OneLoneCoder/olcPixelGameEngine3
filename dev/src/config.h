#pragma once

//! START CONFIGURATION
// Version number (accessible as value for incremental comparisons)
#define PGE_VER 300



// Pixel Format in memory 													  
#define PGE_PIXEL_LAYOUT_RGBA 1
#define PGE_PIXEL_LAYOUT_ABGR 2

#if !defined(PGE_PIXEL_LAYOUT)
	#define PGE_PIXEL_LAYOUT PGE_PIXEL_LAYOUT_RGBA
#endif



// Use UK spellings where appropriate
#define PGE_SPELL_CORRECTLY 1
#define PGE_SPELL_INCORRECTLY 2

#if !defined(PGE_SPELLING)
	#define PGE_SPELLING PGE_SPELL_CORRECTLY
#endif


// Choose "Operating System"
#define OLC_HOST_WINDOWS 1
#define OLC_HOST_LINUX_X11 2
#define OLC_HOST_LINUX_WAYLAND 3
#define OLC_HOST_MACOS 4
#define OLC_HOST_EMSCRIPTEN 5
#define OLC_HOST_ANDROID 6
#define OLC_HOST_IOS 7
#define OLC_HOST_LINUX_DRM 8

#if !defined(OLC_HOST)
	#if defined(_WIN32)
		#define OLC_HOST OLC_HOST_WINDOWS
	#endif

	#if (defined(__linux__) || defined(__FreeBSD__)) && !defined(__ANDROID__)
		// Note: Assumes X11 atm
		#define OLC_HOST OLC_HOST_LINUX_X11
	#endif

	#if defined(__APPLE__)
		#define GL_SILENCE_DEPRECATION	
		#define OLC_HOST OLC_HOST_MACOS
	#endif
	
	#if defined(__EMSCRIPTEN__)
		#define OLC_HOST OLC_HOST_EMSCRIPTEN
	#endif

	#if defined(__ANDROID__)
		#define OLC_HOST OLC_HOST_ANDROID
	#endif

	#if defined(__iOS__)
		#define OLC_HOST OLC_HOST_IOS
	#endif
#endif


// Choose "GPU Interface"
#define OLC_GPU_NONE 1
#define OLC_GPU_OPENGL33 2

#if !defined(OLC_GPU)
	#define OLC_GPU OLC_GPU_OPENGL33
#endif

#define OLC_IMAGELOADER_NONE 1
#define OLC_IMAGELOADER_WINGDI 2
#define OLC_IMAGELOADER_MACOS 3
#define OLC_IMAGELOADER_LIB_PNG 4
#define OLC_IMAGELOADER_NDK_IMAGEDECODER 5
#define OLC_IMAGELOADER_STB_IMAGE 6

#if defined(OLC_USE_STB_IMAGE)
	#define OLC_IMAGELOADER OLC_IMAGELOADER_STB_IMAGE
	#define OLC_IMAGELOADER_CLASS ImageLoader_STB_Image
#endif

#if !defined(OLC_IMAGELOADER)
	#if OLC_HOST == OLC_HOST_WINDOWS
		#define OLC_IMAGELOADER OLC_IMAGELOADER_WINGDI
		#define OLC_IMAGELOADER_CLASS ImageLoader_WinGDI
	#endif

	#if OLC_HOST == OLC_HOST_MACOS
		#define OLC_IMAGELOADER OLC_IMAGELOADER_MACOS
		#define OLC_IMAGELOADER_CLASS ImageLoader_MacOS
	#endif

	#if OLC_HOST == OLC_HOST_LINUX_X11 || OLC_HOST == OLC_HOST_LINUX_WAYLAND || OLC_HOST == OLC_HOST_LINUX_DRM
		#define OLC_IMAGELOADER OLC_IMAGELOADER_LIB_PNG
		#define OLC_IMAGELOADER_CLASS ImageLoader_LibPNG
	#endif

	#if OLC_HOST == OLC_HOST_EMSCRIPTEN
		#define OLC_IMAGELOADER OLC_IMAGELOADER_LIB_PNG
		#define OLC_IMAGELOADER_CLASS ImageLoader_LibPNG
	#endif

	#if OLC_HOST == OLC_HOST_ANDROID
		#define OLC_IMAGELOADER OLC_IMAGELOADER_NDK_IMAGEDECODER
		#define OLC_IMAGELOADER_CLASS ImageLoader_NDKImageDecoder
	#endif
#endif

#define OLC_MULTIWINDOW_NO 1
#define OLC_MULTIWINDOW_YES 2

#if !defined(OLC_MULTIWINDOW)
	#define OLC_MULTIWINDOW OLC_MULTIWINDOW_NO
#endif


#define OLC_MOUSE_BUTTONS 5

#define OLC_DEFAULT_KEYBOARD_LAYOUT olc::KeyboardLayout::QWERTY_UK

#define OLC_GPU_MAX_VERTICES 8192
#define OLC_GPU_ERRORCHECK 0
#define OLC_MSAA_SAMPLES 4
#define OLC_DEFAULT_CIRCLE_FACETS 16
#define OLC_DEFAULT_VSYNC true

#define LICENCE_DEFAULT "OneLoneCoder.com - Pixel Game Engine 3 - "

// De-Noise in MSVC (C++20) /Wall
#pragma warning(disable:4820) // Disable Padding Warnings
#pragma warning(disable:5045) // Disable Spectre Mitigation Warnings
#pragma warning(disable:4514) // Disable Unreferenced Inline Function Warnings

template<typename... Args>
inline constexpr void olc_IgnoreUnused(Args&&...) noexcept {}

#if OLC_HOST == OLC_HOST_WINDOWS
#define OLC_FRIENDLY_HOST Host_Windows_WinAPI
#endif

#if OLC_HOST == OLC_HOST_MACOS
#define OLC_FRIENDLY_HOST Host_Apple_MacOS
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
#define OLC_FRIENDLY_HOST Host_Linux_X11
#endif

#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
#define OLC_FRIENDLY_HOST Host_Linux_Wayland
#endif

#if OLC_HOST == OLC_HOST_LINUX_DRM
#define OLC_FRIENDLY_HOST Host_Linux_DRM
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
#define OLC_FRIENDLY_HOST Host_Web_Emscripten
#endif

#if OLC_HOST == OLC_HOST_ANDROID
#define OLC_FRIENDLY_HOST Host_Android
#endif

//! END CONFIGURATION