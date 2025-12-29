#pragma once

//! START STDHEADER
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "vector2d.h"
#include "pixel.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_IMAGE_DECLARED)
namespace olc
{
	struct ImageConfig
	{
		bool Filtered = false;
		bool Clamp = false;
		bool InRAM = true;
		bool InVRAM = true;
	};

	struct ImageRegion;

	class Image
	{
	public:
		// Constructs a general purpose image
		Image() = default;
		virtual ~Image() = default;

	public:
		bool Create(const olc::vi2d& size, const ImageConfig& cfg = olc::ImageConfig());

	public:
		// Returns size (x, y) in pixels
		const olc::vi2d& Size() const;
		// Returns read/write pointer to start of 1D stream of pixel data
		olc::Pixel* Data();
		// [UNSAFE] Returns pixel at location
		olc::Pixel& Pixel(const olc::vi2d& pos);
		// Returns how this image was configured upon creation
		const ImageConfig& GetConfig() const;
		// Return GPU Resource ID
		int32_t GetGPUID() const;
		// Set GPU Resource ID (0 to eliminate)
		void SetGPUID(const int32_t id);
		// Get underlying vector of pixels
		std::vector<olc::Pixel>& GetPixels();

		olc::Pixel Sample(const olc::vf2d& uv);

		void Resize(const olc::vi2d& size);
		
		bool BoundToGPU() const;
		bool BoundToCPU() const;

		olc::ImageRegion region(const olc::vf2d pos, const olc::vf2d& size);
		olc::ImageRegion region(const olc::vf2d& vTL, const olc::vf2d& vTR, const olc::vf2d& vBL, const olc::vf2d& vBR);


	public: // Make friendly private later
		void BindGPU();
		void BindCPU();

	protected:
		ImageConfig config;
		olc::vi2d dimensions;
		std::vector<olc::Pixel> pixels;
		int32_t gpuResourceID = 0;
		bool onGPU = false;
		bool onCPU = true;
	};

	struct ImageRegion
	{
		olc::Image& image;		
		olc::vf2d regionsize;
		union
		{
			std::array<olc::vf2d, 4> coords;
			olc::vf2d tl;
			olc::vf2d tr;
			olc::vf2d br;
			olc::vf2d bl;
		};

		ImageRegion(olc::Image& i, const olc::vf2d& vTL = { 0,0 }, const olc::vf2d& vTR = { 1,0 }, const olc::vf2d& vBL = { 0,1 }, const olc::vf2d& vBR = { 1,1 })
			: image(i)
		{
			coords = { vTL, vTR, vBR, vBL };
			regionsize = (vBR - vTL) * image.Size();
		}

		ImageRegion& operator=(ImageRegion& o)
		{
            return *this; // TODO: Johnngy63 implement properly if needed temp code to get Macos to compile
		}
	};

	
}
#define PGE_IMAGE_DECLARED 1
#endif
//! END DECLARATION
