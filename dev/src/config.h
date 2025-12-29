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

#if !defined(OLC_HOST)
	#if defined(_WIN32)
		#define OLC_HOST OLC_HOST_WINDOWS
	#endif

	#if defined(__linux__) || defined(__FreeBSD__)
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

#if OLC_HOST == OLC_HOST_MACOS
	#undef OLC_IMAGELOADER
	#define OLC_IMAGELOADER OLC_IMAGELOADER_MACOS
#endif

#if !defined(OLC_IMAGELOADER)
	#define OLC_IMAGELOADER OLC_IMAGELOADER_WINGDI
#endif


#define OLC_MULTIWINDOW_NO 1
#define OLC_MULTIWINDOW_YES 2

#if !defined(OLC_MULTIWINDOW)
	#define OLC_MULTIWINDOW OLC_MULTIWINDOW_NO
#endif


#define OLC_MOUSE_BUTTONS 5

#define OLC_GPU_MAX_VERTICES 8192
#define OLC_GPU_ERRORCHECK 0

#define LICENCE_DEFAULT "OneLoneCoder.com - Pixel Game Engine 3 - "

// De-Noise in MSVC (C++20)
#pragma warning(disable:4820) // Disable Padding Warnings

//! END CONFIGURATION