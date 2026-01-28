#include "window.h"

#include "host_iface.h"
#include "gpu_iface.h"
#include "imload_iface.h"

//! START IMPLEMENTATION
namespace olc
{
	Window::Window()
	{
		nUniqueID = pgeguts::CreateUID();
		
	}

	Window::~Window()
	{
	}

	void Window::LinkToHost(olc::host::Host* host)
	{
		pHost = host;
		keyboard.UseKeyboardLayout(pHost->GetKeyboardLayout());
		sFrameTitle = "OneLoneCoder.com - Pixel Game Engine 3";
		pHost->UpdateWindowFrameTitle(this);
	}


	bool Window::olc_OnMouseButton(const uint8_t nButton, const bool bPressed)
	{
		mouse.SetButton(nButton, bPressed);
		return false;
	}

	bool Window::olc_OnMouseMove(const olc::vi2d& vMousePos)
	{		
		mouse.SetPosition(olc::vf2d(vMousePos) / olc::vf2d(GetWindowSize()));
		return true;
	}

	bool Window::olc_OnMouseWheel(const int32_t nScroll)
	{
		mouse.SetWheel(nScroll);
		return true;
	}

	bool Window::olc_OnMouseFocus(const bool bHasFocus)
	{
		olc_IgnoreUnused(bHasFocus);
		return false;
	}

	bool Window::olc_OnWindowPosition(const olc::vi2d& vPos)
	{
		olc_IgnoreUnused(vPos);
		return false;
	}

	bool Window::olc_OnWindowSize(const olc::vi2d& vSize)
	{
		return SetWindowSize(vSize);		
	}

	bool Window::olc_OnWindowClose()
	{
		bRequestToClose = true;		
		return true;
	}

	bool Window::olc_OnKeyPress(const olc::Key key, const bool bPressed)
	{
		keyboard.SetKey(key, bPressed);
		return true;
	}

	bool Window::olc_ShouldRemove() const
	{
		return bShouldRemove;
	}


	size_t Window::GetUID() const
	{
		return nUniqueID;
	}

	const olc::vi2d& Window::GetWindowSize() const
	{
		return vWindowSize;
	}

	const olc::vi2d& Window::GetWindowPosition() const
	{
		return vWindowPos;
	}

	bool Window::SetWindowSize(const olc::vi2d& vSize)
	{
		// Request host to change geometry of window object

		vWindowSize = vSize;
		return true;
	}

	bool Window::SetWindowPosition(const olc::vi2d& vPosition)
	{
		// Request host to change geometry of window object

		vWindowPos = vPosition;
		return true;
	}

	const std::string& Window::GetWindowTitle() const
	{
		return sFrameTitle;
	}

	bool Window::SetWindowTitle(const std::string& sTitle)
	{
		sFrameTitle = sTitle;
		pHost->UpdateWindowFrameTitle(this);
		return false;
	}

};
//! END IMPLEMENTATION