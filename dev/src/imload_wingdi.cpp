#include "imload_wingdi.h"

//! START IMPLEMENTATION
#if OLC_HOST == OLC_HOST_WINDOWS
namespace olc::imload
{
	namespace internals
	{
		std::wstring ConvertS2W(std::string s)
		{
#ifdef __MINGW32__
			wchar_t* buffer = new wchar_t[s.length() + 1];
			mbstowcs(buffer, s.c_str(), s.length());
			buffer[s.length()] = L'\0';
#else
			int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
			wchar_t* buffer = new wchar_t[count];
			MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
#endif
			std::wstring w(buffer);
			delete[] buffer;
			return w;
		}

		static class GDIPlusStartup
		{
		public:
			GDIPlusStartup()
			{
				Gdiplus::GdiplusStartupInput startupInput;
				GdiplusStartup(&token, &startupInput, NULL);
			}

			ULONG_PTR	token;

			~GDIPlusStartup()
			{
				Gdiplus::GdiplusShutdown(token);
			}

		} gdistartup;
	}

	bool ImageLoader_WinGDI::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
	{
		// Check file exists
		if (!std::filesystem::exists(sFileName))
			return false; // File doesn't exist
		
		// Load file into windows "bitmap". 1992 calling...
		Gdiplus::Bitmap* bmp = nullptr;
		bmp = Gdiplus::Bitmap::FromFile(internals::ConvertS2W(sFileName).c_str());
		if (bmp->GetLastStatus() != Gdiplus::Ok)
			return false; // File wasn't valid

		// Need to swizzle each pixel...
		image.Create(olc::vi2d(bmp->GetWidth(), bmp->GetHeight()));
		for (int y = 0; y < image.Size().y; y++)
			for (int x = 0; x < image.Size().x; x++)
			{
				Gdiplus::Color c;
				bmp->GetPixel(x, y, &c);
				image.Pixel(olc::vi2d(x, y)) = olc::Pixel(c.GetRed(), c.GetGreen(), c.GetBlue(), c.GetAlpha());
			}

		// All done
		delete bmp;
		return true;
	}

	bool ImageLoader_WinGDI::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
	{
		return false;
	}

	bool ImageLoader_WinGDI::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
	{
		return false;
	}

	bool ImageLoader_WinGDI::WriteImageToFile(const olc::Image& image, const std::string& sFileName)
	{
		return false;
	}

	bool ImageLoader_WinGDI::WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data)
	{
		return false;
	}
}
#endif // OLC_HOST == OLC_HOST_WINDOWS
//! END IMPLEMENTATION
