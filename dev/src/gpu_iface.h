#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "gputask.h"
#include "image.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_RENDERER_IFACE_DECLARED)
namespace olc
{
	namespace gpu
	{
		struct RendererConfig
		{
			bool FullScreen = false;
			bool VerticalSync = false;
		};

		enum class RendererError
		{
			NoError, //X11 defines a None macro, so this can't be named None
			InvalidDCPixelFormat,
			FailedToSetDCPixelFormat,
			FailedToCreateRenderContext,
			FailedToSwitchRenderContext,
			FailedToCompileShader,
		};

		class Shader
		{
		public:
			virtual ~Shader();

		public:
			void SetPixelShaderSource(const std::string& src);
			void SetVertexShaderSource(const std::string& src);
			void SetGeometryShaderSource(const std::string& src);

			virtual std::string Compile() = 0;
			virtual int32_t CreateUniform(const std::string& name) = 0;

			int32_t GetUniform(const std::string& name)	const;
			uint32_t GetShaderID() const;


		public:
			static std::string PS_DefaultHeader();
			static std::string PS_DefaultMain();
			
			static std::string VS_DefaultHeader();
			static std::string VS_DefaultMain();
			
			static std::string GS_DefaultHeader();
			static std::string GS_DefaultMain();


		protected:
			std::string srcPixelShader;
			std::string srcVertexShader;
			std::string srcGeometryShader;
			uint32_t nPixelShaderID = 0;
			uint32_t nVertexShaderID = 0;
			uint32_t nGeometryShaderID = 0;
			uint32_t nCompiledShaderID = 0 ;
			std::unordered_map<std::string, uint32_t> mapUniforms;

			static std::string static_PS_DefaultHeader;
			static std::string static_PS_DefaultMain;
			static std::string static_VS_DefaultHeader;
			static std::string static_VS_DefaultMain;
			static std::string static_GS_DefaultHeader;
			static std::string static_GS_DefaultMain;			
		};

		class Renderer
		{
		public:
			Renderer() = default;
			virtual ~Renderer() {};

		public:
			// Check/Get last error
			inline RendererError GetLastError() const { return lastError; }
			inline const RendererConfig& GetConfig() const { return config; }

			

		public: // Device Stuff
			// Constructs a GPU Device interface
			virtual bool CreateDevice(std::vector<void*> os_win_id, const RendererConfig& cfg) = 0;
			// Destroys a GPU device interface
			virtual bool DestroyDevice() = 0;
			// If applicable, retarget the rendering context
			virtual bool RetargetDevice(std::vector<void*> os_win_id) = 0;
			// Prepare an OS rendering target
			virtual bool PrepareWindowTarget(std::vector<void*> os_win_id) = 0;

		public: // Texture Resource Stuff
			// Allocates a new texture resource in VRAM, returns handle
			virtual uint32_t CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg = olc::ImageConfig()) = 0;
			// Writes to / updates an existing texture resource in VRAM, using existing Image in SRAM
			virtual bool WriteTexture(const uint32_t texid, olc::Image& image) = 0;
			// Writes to / updates an existing Image in SRAM, from existing texture resource in VRAM
			virtual bool ReadTexture(const uint32_t texid, olc::Image& image) = 0;
			// Destroys and releases texture resource for given handle
			virtual bool DeleteTexture(const uint32_t texid) = 0;
			// Makes active the given texture resource (for subsequent sampling operations)
			virtual bool AssignTextureSource(const uint32_t slot, const uint32_t texid) = 0;
			// Makes active the given texture resource (for subsequent rendering operations)
			virtual bool AssignTextureTarget(const uint32_t slot, const uint32_t texid) = 0;
			// Detaches any texture from the specified render target slot
			virtual bool DetachTextureTarget(const uint32_t slot) = 0;
			// Resolves an MSAA texture into a normal texture
			virtual bool ResolveMSAA(const uint32_t msaaTexId) = 0;

		public: // Shader Construction Stuff

			// Change the shader used for subsequent GPU drawing tasks
			virtual bool ApplyShader(const Shader& shader) = 0;
			// Reset to default shader for subsequent GPU drawing tasks
			virtual bool ApplyDefaultShader() = 0;
			// Set uniform variable for subsequent GPU drawing tasks
			virtual bool SetUniform(const std::string& name, const float value) = 0;
			// Set uniform variable for subsequent GPU drawing tasks
			virtual bool SetUniform(const std::string& name, const olc::vf2d& value) = 0;
			// Set uniform variable for subsequent GPU drawing tasks
			virtual bool SetUniform(const std::string& name, const olc::Pixel value) = 0;

		public: // GPU Task Processing Stuff
			virtual bool DoGPUTask(const olc::GPUTask& task) = 0;

		public: // Swap Chain Stuff
			// Clears the viewport to a specific colour and depth
			virtual bool ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil) = 0;
			// Sets the viewport area of the drawing space
			virtual bool SetViewport(const olc::vf2d& pos, const olc::vf2d& size) = 0;
			// Configures defaults prior to drawing
			virtual bool DisplayPrepare(const float fFrameElapsedTime, const float fTotalElapsedTime) = 0;
			// Displays the final output
			virtual bool DisplayDraw(std::vector<void*> os_win_id, bool bVerticalSyncNow = false) = 0;


		protected:
			RendererConfig config;
			RendererError lastError = RendererError::NoError;
			float fFrameTime = 0;
			float fTotalTime = 0;
		};
	}
}
#define PGE_RENDERER_IFACE_DECLARED 1
#endif
//! END DECLARATION