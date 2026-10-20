#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <iostream>
//! END STDHEADER
 

//! START CUSTOMHEADER GLOBAL
#include "config.h"
//! END CUSTOMHEADER


//! START DECLARATION
#if !defined(PGE_PIXEL_DECLARED)
#if OLC_HOST == OLC_HOST_MACOS || OLC_HOST == OLC_HOST_IOS
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy" // Silence warnings about implicitly generated copy constructor for Pixel
#endif
namespace olc
{
	class Pixel
	{
	public:
		union
		{
#pragma warning(disable:4201) // Top MSVC whinging about anonymous structs
			struct // Ooooohh be afraid, I'm anonymous! The C++ gremlins will come and get me!
			{
#if PGE_PIXEL_LAYOUT == PGE_PIXEL_LAYOUT_RGBA
				uint8_t r; // 8-bit Red component
				uint8_t g; // 8-bit Green component
				uint8_t b; // 8-bit Blue component
				uint8_t a; // 8-bit Alpha component
#endif

#if PGE_PIXEL_LAYOUT == PGE_PIXEL_LAYOUT_ABGR
				uint8_t a; // 8-bit Alpha component
				uint8_t b; // 8-bit Blue component
				uint8_t g; // 8-bit Green component
				uint8_t r; // 8-bit Red component
#endif
			};
#pragma warning(default:4201)

#if PGE_PIXEL_LAYOUT == PGE_PIXEL_LAYOUT_ABGR			
			//             AABBGGRR
			uint32_t n = 0xFF000000; // 4-Byte AGBR Component
#endif
#if PGE_PIXEL_LAYOUT == PGE_PIXEL_LAYOUT_RGBA
			//             RRGGBBAA
			uint32_t n = 0x000000FF; // 4-Byte RGBA Component
#endif
		};


	public:
		Pixel() = default;

		// Construction via copy
		inline constexpr Pixel(const olc::Pixel& col)
			: n(col.n)
		{ }

		// Construction via copy
		inline constexpr Pixel(const olc::Pixel& col, const uint8_t alpha)
			: n(col.n)
		{		
			a = alpha;
		}


		// Construction via individual channels
		inline constexpr Pixel(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha = 0xFF)
#if PGE_PIXEL_LAYOUT == PGE_PIXEL_LAYOUT_RGBA
			:  r(red), g(green), b(blue), a(alpha)
#endif
#if PGE_PIXEL_LAYOUT == PGE_PIXEL_LAYOUT_ABGR
			: a(alpha), b(blue), g(green), r(red)
#endif

		{ }

		// Construction via numeric assignment (e.g. #00DDBBFF)
		inline constexpr Pixel(const uint32_t col)
			: n(col)
		{ }

		// Multiplicatively Blends two colours
		inline constexpr Pixel blend(const Pixel& p) const
		{
			uint8_t nR = uint8_t(std::clamp((int(r) * int(p.r)) >> 8, 0, 255));
			uint8_t nG = uint8_t(std::clamp((int(g) * int(p.g)) >> 8, 0, 255));
			uint8_t nB = uint8_t(std::clamp((int(b) * int(p.b)) >> 8, 0, 255));
			uint8_t nA = uint8_t(std::clamp((int(a) * int(p.a)) >> 8, 0, 255));
			return Pixel(nR, nG, nB, nA);
		}

		// Chromatically inverts pixel
		inline constexpr Pixel inv() const
		{
			// Note we must force to a wider signed integer type to prohibit
			// the values wrapping/truncating
			uint8_t nR = uint8_t(std::clamp(255 - int(r), 0, 255));
			uint8_t nG = uint8_t(std::clamp(255 - int(g), 0, 255));
			uint8_t nB = uint8_t(std::clamp(255 - int(b), 0, 255));
			return Pixel(nR, nG, nB, a);
		}

		// Test if two pixels ARE equal
		inline constexpr bool operator==(const Pixel& p) const
		{
			return n == p.n;
		}

		// Test if two pixels ARE NOT equal
		inline constexpr bool operator!=(const Pixel& p) const
		{
			return n != p.n;
		}

		// Compare memory values for use in hashed containers
		inline constexpr bool operator < (const Pixel& p) const
		{
			return n < p.n;
		}

		// Scales pixel colours, except alpha
		template<typename T>
		inline constexpr Pixel operator * (const T i) const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be numeric");
			T fR = std::clamp(r * i, T(0), T(255));
			T fG = std::clamp(g * i, T(0), T(255));
			T fB = std::clamp(b * i, T(0), T(255));
			return Pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
		}

		// Scales pixel colours, except alpha
		template<typename T>
		inline constexpr Pixel operator / (const T i) const
		{
			static_assert(std::is_arithmetic<T>::value, "T must be numeric");
			T fR = std::clamp(r / i, T(0), T(255));
			T fG = std::clamp(g / i, T(0), T(255));
			T fB = std::clamp(b / i, T(0), T(255));
			return Pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
		}

		template<typename T>
		inline constexpr Pixel& operator *= (const T i)
		{
			static_assert(std::is_arithmetic<T>::value, "T must be numeric");
			this->r = uint8_t(std::clamp(r * i, T(0), T(255)));
			this->g = uint8_t(std::clamp(g * i, T(0), T(255)));
			this->b = uint8_t(std::clamp(b * i, T(0), T(255)));
			return *this;
		}

		template<typename T>
		inline constexpr Pixel& operator /= (const T i)
		{
			static_assert(std::is_arithmetic<T>::value, "T must be numeric");
			this->r = uint8_t(std::clamp(r / i, T(0), T(255)));
			this->g = uint8_t(std::clamp(g / i, T(0), T(255)));
			this->b = uint8_t(std::clamp(b / i, T(0), T(255)));
			return *this;
		}

		// Adds two pixels, except alpha
		inline constexpr Pixel operator + (const Pixel& p) const
		{
			// Note we must force to a wider signed integer type to prohibit
			// the values wrapping/truncating
			uint8_t nR = uint8_t(std::clamp(int(r) + int(p.r), 0, 255));
			uint8_t nG = uint8_t(std::clamp(int(g) + int(p.g), 0, 255));
			uint8_t nB = uint8_t(std::clamp(int(b) + int(p.b), 0, 255));
			return Pixel(nR, nG, nB, a);
		}

		// Subtracts two pixels, except alpha
		inline constexpr Pixel operator - (const Pixel& p) const
		{
			// Note we must force to a wider signed integer type to prohibit
			// the values wrapping/truncating
			uint8_t nR = uint8_t(std::clamp(int(r) - int(p.r), 0, 255));
			uint8_t nG = uint8_t(std::clamp(int(g) - int(p.g), 0, 255));
			uint8_t nB = uint8_t(std::clamp(int(b) - int(p.b), 0, 255));
			return Pixel(nR, nG, nB, a);
		}

		inline constexpr Pixel& operator += (const Pixel& p)
		{
			// Note we must force to a wider signed integer type to prohibit
			// the values wrapping/truncating
			this->r = uint8_t(std::clamp(int(r) + int(p.r), 0, 255));
			this->g = uint8_t(std::clamp(int(g) + int(p.g), 0, 255));
			this->b = uint8_t(std::clamp(int(b) + int(p.b), 0, 255));
			return *this;
		}

		inline constexpr Pixel& operator -= (const Pixel& p  )
		{
			// Note we must force to a wider signed integer type to prohibit
			// the values wrapping/truncating
			this->r = uint8_t(std::clamp(int(r) - int(p.r), 0, 255));
			this->g = uint8_t(std::clamp(int(g) - int(p.g), 0, 255));
			this->b = uint8_t(std::clamp(int(b) - int(p.b), 0, 255));
			return *this; 
		}

		// Return RGBA string
		inline std::string str() const
		{
			return std::string("(") + std::to_string(this->r) + "," + std::to_string(this->g) + "," + std::to_string(this->b) + "," + std::to_string(this->a) + ")";
		}
	};

	// Construct pixel from normalised values
	inline constexpr Pixel PixelF(const float red, const float green, const float blue, const float alpha = 1.0f)
	{
		return Pixel(uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), uint8_t(alpha * 255.0f));
	}


	// C++20 does not recognise these functions as constexpr in std, but this hack
	// works around that restriction.
	namespace internal
	{
		template<typename T>
		constexpr T abs(T v)
		{
			return std::abs(v);
		}

		template<typename T>
		constexpr T fmodf(T v, T u)
		{
			return ::fmodf(v, u);
		}
	}

	// Construct pixel from HSV values
	inline constexpr Pixel PixelHSV(const float h, const float s, const float v, const float alpha = 1.0f)
	{
		const float hue = std::clamp(h, 0.0f, 360.0f);
		const float saturation = std::clamp(s, 0.0f, 1.0f);
		const float value = std::clamp(v, 0.0f, 1.0f);

		const float chroma = value * saturation;
		const float x = chroma * (1.0f - internal::abs(internal::fmodf((hue / 60.0f), 2.0f) - 1.0f));
		const float m = value - chroma;

		float rawRed, rawGreen, rawBlue;

		if (saturation == 0.0f) {
			rawRed = rawGreen = rawBlue = value;
		}
		else if (hue < 60.0f) {
			rawRed = chroma; rawGreen = x; rawBlue = 0.0f;
		}
		else if (60.0f <= hue && hue < 120.0f) {
			rawRed = x;	rawGreen = chroma;	rawBlue = 0.0f;
		}
		else if (120.0f <= hue && hue < 180.0f) {
			rawRed = 0.0f;	rawGreen = chroma;	rawBlue = x;
		}
		else if (180.0f <= hue && hue < 240.0f) {
			rawRed = 0;	rawGreen = x; rawBlue = chroma;
		}
		else if (240.0f <= hue && hue < 300.0f) {
			rawRed = x;	rawGreen = 0.0f; rawBlue = chroma;
		}
		else {
			rawRed = chroma; rawGreen = 0.0f; rawBlue = x;
		}

		return olc::PixelF(rawRed + m, rawGreen + m, rawBlue + m, alpha);
	}

	template<typename T>
	inline constexpr Pixel PixelLerp(const olc::Pixel& p1, const olc::Pixel& p2, const T t)
	{
		static_assert(std::is_floating_point<T>::value, "T must be a floating point type");
		return (p2 * t) + p1 * (T(1) - t);
	}

	// Allow olc::Pixel to play nicely with std::cout	
	inline std::ostream& operator << (std::ostream& os, const olc::Pixel& rhs)
	{
		os << rhs.str();
		return os;
	}

	// A small set of convenient "olc::classic" colour constants
	namespace Colour
	{
		inline constexpr olc::Pixel
			GREY(192, 192, 192), 
			DARK_GREY(128, 128, 128), 
			VERY_DARK_GREY(64, 64, 64),
			RED(255, 0, 0), 
			DARK_RED(128, 0, 0), 
			VERY_DARK_RED(64, 0, 0),
			YELLOW(255, 255, 0), 
			DARK_YELLOW(128, 128, 0), 
			VERY_DARK_YELLOW(64, 64, 0),
			GREEN(0, 255, 0), 
			DARK_GREEN(0, 128, 0), 
			VERY_DARK_GREEN(0, 64, 0),
			CYAN(0, 255, 255), 
			DARK_CYAN(0, 128, 128), 
			VERY_DARK_CYAN(0, 64, 64),
			BLUE(0, 0, 255), 
			DARK_BLUE(0, 0, 128), 
			VERY_DARK_BLUE(0, 0, 64),
			MAGENTA(255, 0, 255), 
			DARK_MAGENTA(128, 0, 128), 
			VERY_DARK_MAGENTA(64, 0, 64),
			WHITE(255, 255, 255), 
			BLACK(0, 0, 0), 
			BLANK(0, 0, 0, 0),
			TANGERINE(255, 165, 0);
	}
}

#if OLC_HOST == OLC_HOST_MACOS || OLC_HOST == OLC_HOST_IOS 
#pragma clang diagnostic pop
#endif

#define PGE_PIXEL_DECLARED 1
#endif
//! END DECLARATION