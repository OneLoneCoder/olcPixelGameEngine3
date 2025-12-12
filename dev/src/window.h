#pragma once

//! START STDHEADER	GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>

//! END STDHEADER

//! START CUSTOMHEADER GLOBAL
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "draw2d.h"
#include "hw_mouse.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_WINDOW_DECLARED)

#if OLC_HOST == OLC_HOST_WINDOWS
	#define FRIENDLY_HOST Host_Windows_WinAPI
#endif

#if OLC_HOST == OLC_HOST_MACOS
    #define FRIENDLY_HOST Host_Apple_MacOS
#endif

namespace olc
{
	namespace host
	{
		class FRIENDLY_HOST;
		class Host;
	}

	namespace gpu
	{
		class Renderer;
	}

	namespace imload
	{
		class ImageLoader;
	}

	namespace hw
	{
		class Mouse;
	}

	class Window
	{
		friend class olc::host::FRIENDLY_HOST;
		

	public:
		Window();
		virtual ~Window();
										
		void LinkToHost(olc::host::Host* host);

	

	private: // These are called externally from the host
		// Set Mouse Device State
		virtual bool olc_OnMouseButton(const uint8_t nButton, const bool bPressed);
		virtual bool olc_OnMouseMove(const olc::vi2d& vMousePos);
		virtual bool olc_OnMouseWheel(const int32_t nScroll);
		virtual bool olc_OnMouseFocus(const bool bHasFocus);
		
		// Set Window State
		virtual bool olc_OnWindowPosition(const olc::vi2d& vWindowPos);
		virtual bool olc_OnWindowSize(const olc::vi2d& vWindowSize);
		virtual bool olc_OnWindowClose();

		// Set Keyboard State



	public:
		bool olc_ShouldRemove() const;
		

	public:
		size_t GetUID() const;

		const olc::vi2d& GetWindowSize() const;
		bool SetWindowSize(const olc::vi2d& vSize);

		const olc::vi2d& GetWindowPosition() const;
		bool SetWindowPosition(const olc::vi2d& vPosition);

		const std::string& GetWindowTitle() const;
		bool SetWindowTitle(const std::string& sTitle);

	protected:
		bool bRequestToClose = false;
		bool bShouldRemove = false;

	protected:
		size_t nUniqueID = -1;
		olc::vi2d vWindowPos;
		olc::vi2d vWindowSize;
		std::string sFrameTitle;
	
	protected:
		olc::host::Host* pHost = nullptr;

	protected:
		olc::hw::Mouse mouse;

	};
}
#define PGE_WINDOW_DECLARED 1
#endif
//! END DECLARATION