#pragma once

#include "gpu_iface.h"
#include "api_opengl.h"

//! START DECLARATION
#if !defined(PGE_RENDERER_OPENGL33_DECLARED)
namespace olc
{
	namespace gpu
	{
		class Shader_GLSL33 : public olc::gpu::Shader
		{
		public:
			std::string Compile() override;
			int32_t CreateUniform(const std::string& name) override;
		};

		class Renderer_OGL33 : public olc::gpu::Renderer
		{
		public: // Device Stuff
			// Constructs a GPU Device interface
			bool CreateDevice(std::vector<void*> os_win_id, const RendererConfig& cfg) override;
			// Destroys a GPU device interface
			bool DestroyDevice() override;
			// If applicable, relocate the rendering context
			bool RetargetDevice(std::vector<void*> os_win_id) override;
			// Prepare an OS rendering target
			bool PrepareWindowTarget(std::vector<void*> os_win_id) override;


		public: // Texture Resource Stuff
			// Allocates a new texture resource in VRAM, returns handle
			uint32_t CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg = olc::ImageConfig()) override;
			// Writes to / updates an existing texture resource in VRAM, using existing Image in SRAM
			bool WriteTexture(const uint32_t texid, olc::Image& image) override;
			// Writes to / updates an existing Image in SRAM, from existing texture resource in VRAM
			bool ReadTexture(const uint32_t texid, olc::Image& image) override;
			// Destroys and releases texture resource for given handle
			bool DeleteTexture(const uint32_t texid) override;
			// Makes active the given texture resource (for subsequent sampling operations)
			bool AssignTextureSource(const uint32_t slot, const uint32_t texid) override;
			// Makes active the given texture resource (for subsequent rendering operations)
			bool AssignTextureTarget(const uint32_t slot, const uint32_t texid) override;
			// Resolves an MSAA texture into a normal texture
			virtual bool ResolveMSAA(const uint32_t msaaTexId) override;

		public: // Shader Construction Stuff
			// Change the shader used for subsequent GPU drawing tasks
			bool ApplyShader(const Shader& shader) override;
			// Reset to default shader for subsequent GPU drawing tasks
			bool ApplyDefaultShader() override;
			// Set uniform variable for subsequent GPU drawing tasks
			bool SetUniform(const std::string& name, const float value) override;
			// Set uniform variable for subsequent GPU drawing tasks
			bool SetUniform(const std::string& name, const olc::vf2d& value) override;
			// Set uniform variable for subsequent GPU drawing tasks
			bool SetUniform(const std::string& name, const olc::Pixel value) override;

		public: // GPU Task Stuff
			virtual bool DoGPUTask(const olc::GPUTask& task) override;

		public: // Swap Chain Stuff
			// Clears the viewport to a specific colour and depth
			virtual bool ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil) override;
			// Sets the viewport area of the drawing space
			virtual bool SetViewport(const olc::vf2d& pos, const olc::vf2d& size) override;
			// Configures defaults prior to drawing
			virtual bool DisplayPrepare(const float fFrameElapsedTime, const float fTotalElapsedTime) override;
			// Displays the final output
			virtual bool DisplayDraw(std::vector<void*> os_win_id, bool bVerticalSyncNow) override;

		
		protected: // These may need some thinking about re multiple window
			//olc::apis::opengl::glDeviceContext_t glDeviceContext = 0;
#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND || OLC_HOST == OLC_HOST_ANDROID || OLC_HOST == OLC_HOST_LINUX_DRM
	olc::apis::opengl::glRenderContext_t glRenderContext;
#else
	olc::apis::opengl::glRenderContext_t glRenderContext = 0;
#endif

			Shader_GLSL33 shaderDefault;
			uint32_t nDefaultVB = 0;
			uint32_t nDefaultVA = 0;
			uint32_t nDefaultFBO = 0;
			uint32_t nScreenFBO = 0;
			olc::Image imgBlank;
			olc::vf2d vTargetSize;

			uint32_t nCurrentTextureTarget = 0;
			uint32_t nCurrentTextureSource = 0;

			//std::unordered_map<uint32_t, uint32_t> mapMSAAToResolved;
			uint32_t nResolveFBO_Read = 0;
			uint32_t nResolveFBO_Draw = 0;

			std::unordered_map<uint32_t, olc::vi2d> mapTextureSizes;

			std::unordered_map<uint32_t, uint32_t> mapTextureToRenderbuffer;

			const Shader* pCurrentShader = nullptr;

			uint32_t nDepthRBO = 0;              // Shared depth renderbuffer
			olc::vi2d vCurrentDepthSize = {0, 0}; // Track current depth buffer size
			int32_t nCurrentDepthSamples = 0;     // Track current MSAA sample count

#if OLC_HOST == OLC_HOST_ANDROID || OLC_HOST == OLC_HOST_LINUX_DRM
			EGLConfig FindBestConfig(EGLDisplay display, int desiredMultisamples = OLC_MSAA_SAMPLES);
#endif

		};
	}
}
#define PGE_RENDERER_OPENGL33_DECLARED 1
#endif
//! END DECLARATION

