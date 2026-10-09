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
		bool MSAA = false;
		uint32_t MSAASamples = OLC_MSAA_SAMPLES;
	};

	struct ImageRegion;

	class PGEWindow;

	namespace imload
	{
		class ImageLoader;
	}

	class Image
	{
	public:
		// Constructs a general purpose image
		Image() = default;
		virtual ~Image() = default;

		// Prevent copying & accidental duplication
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		Image(Image&&) = default;
		Image& operator=(Image&&) = default;

	public:
		// Creates nothing but an array of pixels in system memory. Normal users
		// should never need to call this method. If you want to construct an
		// olc::Image object, use factory methods in olc::PGEWindow
		// CreateImage(...)
		bool CreateNoGPU(const olc::vi2d& size, const ImageConfig& cfg = olc::ImageConfig());
		

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
		uint32_t GetGPUID() const;
		// Set GPU Resource ID (0 to eliminate)
		void SetGPUID(const uint32_t id);
		// Get underlying vector of pixels
		std::vector<olc::Pixel>& GetPixels();

		olc::Pixel Sample(const olc::vf2d& uv);

		void Resize(const olc::vi2d& size);
		
		bool BoundToGPU() const;
		bool BoundToCPU() const;

	public:
		olc::ImageRegion all();
		olc::ImageRegion region(const olc::vf2d pos, const olc::vf2d& size);
		olc::ImageRegion region(const olc::vf2d& vTL, const olc::vf2d& vTR, const olc::vf2d& vBL, const olc::vf2d& vBR);
		olc::ImageRegion flipV();
		olc::ImageRegion flipH();
		olc::ImageRegion flipD();

	public: // Make friendly private later
		void BindGPU();
		void BindCPU();

	protected:
		ImageConfig config;
		olc::vi2d dimensions;
		std::vector<olc::Pixel> pixels;
		uint32_t gpuResourceID = 0;
		bool onGPU = false;
		bool onCPU = true;
	};

	struct ImageRegion
	{
		// Reference to source image, it is wrapped so that 
		// 1) it is syntactically clear to use
		// 2) it cannot be null
		// 3) it allows ImageRegion to be copyable
		std::reference_wrapper<olc::Image> image;	

		// Size of region in pixels. We store this so we can
		// transform teh region appropriately later
		olc::vf2d regionsize;

		// Texture coordinates in normalised space, unioned
		// for convenient access with direct accessors
		union
		{
			std::array<olc::vf2d, 4> coords;
			olc::vf2d tl;
			olc::vf2d tr;
			olc::vf2d br;
			olc::vf2d bl;
		};

		// Construct region from image and coordinates. By default
		// the entire image is the region. This allows ImageRegion
		// to be invisibly constructed from an Image reference.
		ImageRegion(olc::Image& i, const olc::vf2d& vTL = { 0,0 }, const olc::vf2d& vTR = { 1,0 }, const olc::vf2d& vBL = { 0,1 }, const olc::vf2d& vBR = { 1,1 })
			: image(i)
		{
			coords = { vTL, vTR, vBR, vBL };
			regionsize = (vBR - vTL) * image.get().Size();
		}

		// Flip texture coordinates vertically
		olc::ImageRegion& flipV()
		{
			std::swap(coords[0], coords[3]);
			std::swap(coords[1], coords[2]);
			return *this;
		}

		// Flip texture coordinates horizontally
		olc::ImageRegion& flipH()
		{
			std::swap(coords[0], coords[1]);
			std::swap(coords[2], coords[3]);
			return *this;
		}

		// [UNSAFE] Returns pixel at location according to region mapping
		olc::Pixel& Pixel(const olc::vi2d& pos)
		{
			olc::vi2d imgSize = image.get().Size();
			olc::vf2d uv = olc::vf2d(
				(float(pos.x) + 0.5f) / regionsize.x,
				(float(pos.y) + 0.5f) / regionsize.y
			);
			olc::vf2d texPos = TransformUV(uv);
			olc::vi2d pixelPos =
			{
				int(std::floor(texPos.x * imgSize.x)) % imgSize.x,
        		int(std::floor(texPos.y * imgSize.y)) % imgSize.y
			};
			return image.get().Pixel(pixelPos);
		}

		olc::Pixel Sample(const olc::vf2d& uv)
		{
			olc::vi2d imgSize = image.get().Size();
			olc::vf2d texPos = TransformUV(uv);
			olc::vi2d pixelPos =
			{
				int(std::floor(texPos.x * imgSize.x)) % imgSize.x,
				int(std::floor(texPos.y * imgSize.y)) % imgSize.y
			};
			return image.get().Pixel(pixelPos);
		}

		private:
			olc::vf2d TransformUV(const olc::vf2d& uv)
			{
				return
				{
					uv.x * (coords[1].x - coords[0].x) + uv.y * (coords[3].x - coords[0].x) + coords[0].x,
					uv.x * (coords[1].y - coords[0].y) + uv.y * (coords[3].y - coords[0].y) + coords[0].y
				};
			}
	};

	
}
#define PGE_IMAGE_DECLARED 1
#endif
//! END DECLARATION
