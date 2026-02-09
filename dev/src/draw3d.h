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
#include "vector4d.h"
#include "matrix4d.h"
#include "pixel.h"
#include "image.h"
#include "gputask.h"
#include "font.h"
#include "draw2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_DRAW3D_DECLARED)
namespace olc
{
	namespace gpu
	{
		class Renderer;
		class Shader;
	}

	class Draw3D
	{


	public:
		Draw3D(olc::Draw2D& d2d);

		// Associate this drawing toolbox with a renderer
		void SetGPU(olc::gpu::Renderer* const renderer);
		void ProcessGPUTasks();

	public:
		// Sets the drawing target of this drawing toolbox
		void SetTarget(olc::Image& image);
		// Get the current drawing target
		olc::Image& GetTarget();
		// Get Size of drawing target (aka GetTarget()->Size())
		olc::vi2d GetTargetSize();
		// Set the area in the target to 3d draw to
		void SetViewport(const olc::vi2d& pos, const olc::vi2d& size);

	public: // Applied Matrices
		void MatrixReset();
		void SetModelMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetModelMatrix() const;
		void SetViewMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetViewMatrix() const;
		void SetProjectionMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetProjectionMatrix() const;
		void SetMVPMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetMVPMatrix() const;

	public: // Applied Rendering Modes
		void SetCullMode(const olc::GPUTask::CullMode mode);
		void EnableDepth(const bool bEnable);

	public: // Primitive Drawing Functions
		// Clear entire draw target to specific colour
		void Clear(const olc::Pixel& col);

		GPUTask& Line(
			const olc::vf4d& vStart,
			const olc::vf4d& vEnd,
			const olc::Pixel& col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask& Mesh(
			const olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask& Mesh(
			const olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vUVs,
			olc::Image& texture,
			const olc::Pixel tint = olc::Colour::WHITE);
		


	public: // GPU Task Creator Functions (not normally called by user)
		GPUTask TaskWireMesh(
			olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillMesh(
			olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);
		
		GPUTask TaskTexturedMesh(
			olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vTexCoords,
			olc::Image* const image,
			const olc::Pixel tint = olc::Colour::WHITE);

	public:
		// Change the shader used for subsequent GPU drawing tasks
		bool SetShader(const olc::gpu::Shader& shader);
		// Reset to default shader for subsequent GPU drawing tasks
		bool ResetShader();
		// Set uniform variable for subsequent GPU drawing tasks
		bool SetShaderUniform(const std::string& name, const float value);
		// Set uniform variable for subsequent GPU drawing tasks
		bool SetShaderUniform(const std::string& name, const olc::vf2d& value);
		// Set uniform variable for subsequent GPU drawing tasks
		bool SetShaderUniform(const std::string& name, const olc::Pixel value);
		// Assign an image to a texture slot for subsequent GPU drawing tasks
		bool SetShaderTexture(const uint32_t nSlot, olc::Image& image);

	public:
		struct sDrawMetrics
		{
			uint32_t nGPUTasks = 0;
			uint32_t nGPUtoCPUTransfers = 0;
			uint32_t nCPUtoGPUTransfers = 0;
			uint32_t nShaderChanges = 0;
		};

		void ResetDrawMetrics();
		sDrawMetrics GetDrawMetrics() const;

	private:
		sDrawMetrics drawMetrics;




	protected:
		// Checks residency of image resource, and brings it to cpu RAM for r/w
		void PrepareTargetForSW();
		// Checks residency of image resource, and brings it to gpu VRAM for r/w
		void PrepareTargetForHW();

		// Checks residency of image resource, and brings it to cpu RAM for r/w
		void PrepareImageForSW(olc::Image& image);
		// Checks residency of image resource, and brings it to gpu VRAM for r/w
		void PrepareImageForHW(olc::Image& image);

		olc::Image* pTarget = nullptr;
		olc::gpu::Renderer* pRenderer = nullptr;

		mf4d matModel;
		mf4d matView;
		mf4d matProjection;
		mf4d matVP;
		mf4d matMVP;
		olc::vf2d vViewportPos = { 0, 0 };
		olc::vi2d vViewportSize = { 0, 0 };
		olc::GPUTask::CullMode cullMode = olc::GPUTask::CullMode::NoCull;
		bool bDepth = true;

		olc::Draw2D& draw2d;
	
	private:
		// Simple dynamic buffer that only grows as needed
		template<typename T>
		struct buffer
		{
			std::vector<T> data;

			void reserve(size_t n)
			{
				if (n > data.capacity())
					data.reserve(n);

				// Ensure size matches requested so we
				// can index into it directly
				data.resize(n);
			}
		};

		// Thread local buffers to avoid repeated allocations
		static thread_local buffer<olc::vf4d> buffPoints;
		static thread_local buffer<olc::Pixel> buffColours;
		static thread_local buffer<olc::GPUTask> vecGPUTasks;
	};
}
#define PGE_DRAW3D_DECLARED
#endif
//! END DECLARATION