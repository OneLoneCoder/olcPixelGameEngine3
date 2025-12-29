#include "image.h"

//! START IMPLEMENTATION
namespace olc
{
	/*Image::Image(const olc::vi2d& size, const ImageConfig& cfg)
	{
	}

	Image::~Image()
	{
	}*/

	bool Image::Create(const olc::vi2d& size, const ImageConfig& cfg)
	{
		dimensions = size;
		config = cfg;
		pixels.resize(dimensions.area(), olc::Colour::TANGERINE);
		return true;
	}

	const olc::vi2d& Image::Size() const
	{
		return dimensions;
	}

	olc::Pixel* Image::Data()
	{
		return pixels.data();
	}

	olc::Pixel& Image::Pixel(const olc::vi2d& pos)
	{
		return pixels[int(pos.y) * dimensions.x + int(pos.x)];
	}

	const ImageConfig& Image::GetConfig() const
	{
		return config;
	}

	int32_t Image::GetGPUID() const
	{
		return gpuResourceID;
	}

	void Image::SetGPUID(const int32_t id)
	{
		gpuResourceID = id;
	}

	std::vector<olc::Pixel>& Image::GetPixels()
	{
		return pixels;
	}

	olc::Pixel Image::Sample(const olc::vf2d& uv)
	{
		return Pixel({ 
			int(std::round(uv.x * dimensions.x)) % dimensions.x, 
			int(std::round(uv.y * dimensions.y)) % dimensions.y });
	}

	void Image::Resize(const olc::vi2d& size)
	{
		dimensions = size;
		pixels.resize(dimensions.area(), olc::Colour::TANGERINE);
		BindCPU();
	}

	bool Image::BoundToGPU() const
	{
		return onGPU;
	}

	bool Image::BoundToCPU() const
	{
		return onCPU;
	}

	olc::ImageRegion Image::region(const olc::vf2d pos, const olc::vf2d& size)
	{
		return region(pos, { pos.x + size.x, pos.y }, { pos.x, pos.y + size.y }, pos + size);
	}

	olc::ImageRegion Image::region(const olc::vf2d& vTL, const olc::vf2d& vTR, const olc::vf2d& vBL, const olc::vf2d& vBR) 
	{
		auto i = 1.0f / this->Size();
		//return olc::ImageRegion(*this, vTL * i, vTR * i, vBL * i, vBR * i );
		return olc::ImageRegion(*this, (vTL + olc::vf2d{0.005f, 0.005f}) * i, (vTR + olc::vf2d{ -0.005f, 0.005f })* i, (vBL + olc::vf2d{ 0.005f, -0.005f })* i, (vBR + olc::vf2d{ -0.005f, -0.005f })* i);
	}

	void Image::BindGPU()
	{
		onGPU = true;
		onCPU = false;
	}

	void Image::BindCPU()
	{
		onCPU = true;
		onGPU = false;
	}

};
//! END IMPLEMENTATION