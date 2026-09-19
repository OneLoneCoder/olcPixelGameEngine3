#include "gpu_opengl33.h"

//! START IMPLEMENTATION
namespace olc::gpu
{

	// === PIXEL SHADER PGE DEFAULTS ===
	std::string Shader::static_PS_DefaultHeader =
#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
R"(#version 330 core
)"
#else
R"(#version 300 es
precision mediump float;
)"
#endif
R"(
// Pixel output to framebuffer
layout(location = 0) out vec4 pixel;

// PGE *REQUIRED* Uniforms - You must have these in your shader
uniform vec2 pgeTargetSizeInPixels;			// Size of the target olc::Image in pixels
uniform vec2 pgeInverseTargetSizeInPixels;  // 1.0 / Size of the target olc::Image in pixels
uniform float pgeTotalTimeElapsed;			// Total time elapsed since application started
uniform sampler2D pgeTexture0;				// Current source olc::Image bound as texture0
uniform sampler2D pgeTexture1;				// Current source olc::Image bound as texture1
uniform sampler2D pgeTexture2;				// Current source olc::Image bound as texture2
uniform sampler2D pgeTexture3;				// Current source olc::Image bound as texture3

// Inputs from Vertex Shader
in vec2 oTex;
in vec4 oCol;
)";

	std::string Shader::static_PS_DefaultMain =
R"(
void main()
{
	// We premultiply alpha here
	vec4 texColor = texture(pgeTexture0, oTex) * oCol;
	pixel = vec4(texColor.rgb * texColor.a, texColor.a);
}
)";
	
	
	// === VERTEX SHADER PGE DEFAULTS ===
	std::string Shader::static_VS_DefaultHeader =
#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
R"(#version 330 core
)"
#else
R"(#version 300 es
precision mediump float;
)"
#endif
R"(
// PGE *REQUIRED* Attributes - You must have these in your shader
layout(location = 0) in vec4 aPos; // x, y, z, w
layout(location = 1) in vec4 aCol; // r, g, b, a
layout(location = 2) in vec2 aTex; // u, v

// PGE *REQUIRED* Uniforms - You must have these in your shader
uniform mat4 pgeMVP;						// Model-View-Projection matrix
uniform int pgeDrawType;					// 0 = 2D Polygon, 1 = 2D Line, 2 = 3D
uniform vec4 pgeGlobalTint;					// Global tint to apply to all vertices
uniform vec2 pgeTargetSizeInPixels;			// Size of the target olc::Image in pixels
uniform vec2 pgeInverseTargetSizeInPixels;  // 1.0 / Size of the target olc::Image in pixels
uniform float pgeTotalTimeElapsed;			// Total time elapsed since application started

// Outputs to Pixel Shader
out vec2 oTex;
out vec4 oCol;
)";

	std::string Shader::static_VS_DefaultMain =
R"(
void main()
{
	if (pgeDrawType == 2) // 3D																																  
	{
		gl_Position = pgeMVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
		oTex = aTex;
	}

	else if (pgeDrawType == 1) // 2D Line																																		  
	{
		float p = 1.0 / aPos.z;
		gl_Position = p * vec4(vec2(2.0 * (floor(aPos.xy) + 0.5) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = aTex;
	}

	else if (pgeDrawType == 3) // Unconstrained 2D Line																																		  
	{
		float p = 1.0 / aPos.z;
		gl_Position = p * vec4(vec2(2.0 * (aPos.xy) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = aTex;
	}

	else if (pgeDrawType == 4) // Unconstrained 2D Polygon																																		  
	{
		float p = 1.0 / aPos.z;
		gl_Position = p * vec4(vec2(2.0 * (aPos.xy) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = p * vec2(aTex.x, aTex.y);
	}

	else if (pgeDrawType == 0) // 2D Polygon																																		  
	{
		float p = 1.0 / aPos.z; 
		gl_Position = p * vec4(vec2(2.0 * (aPos.xy + 0.25) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = p * vec2(aTex.x, aTex.y);
	}

	else  // Balanced default
	{
		gl_Position = aPos;
		oTex = aTex;
	}

	oCol = aCol * pgeGlobalTint;
}
)";

	
	// === GEOMETRY SHADER PGE DEFAULTS ===
	std::string Shader::static_GS_DefaultHeader = "";
	std::string Shader::static_GS_DefaultMain = "";



	std::string Shader_GLSL33::Compile()
	{
		auto& gl = olc::apis::opengl::gl::Get();

		nCompiledShaderID = gl.glCreateProgram();

		// Fragment Shader
		if (!srcPixelShader.empty())
		{
			nPixelShaderID = gl.glCreateShader(gl.GL_FRAGMENT_SHADER_X);
			const char* s = srcPixelShader.c_str();
			gl.glShaderSource(nPixelShaderID, 1, &s, nullptr);
			gl.glCompileShader(nPixelShaderID);
			
			// Display Fragment Shader Compile Errors
			int32_t nCompileStatus = 0;
			gl.glGetShaderiv(nPixelShaderID, gl.GL_COMPILE_STATUS_X, &nCompileStatus);
			if (nCompileStatus == 0)
			{
				int32_t nInfoLogLength = 0;
				gl.glGetShaderiv(nPixelShaderID, gl.GL_INFO_LOG_LENGTH_X, &nInfoLogLength);
				std::vector<char> vInfoLog(nInfoLogLength);
				gl.glGetShaderInfoLog(nPixelShaderID, nInfoLogLength, nullptr, vInfoLog.data());
				return std::string("Fragment Shader Compile Error:\n") + std::string(vInfoLog.data());				
			}

			gl.glAttachShader(nCompiledShaderID, nPixelShaderID);
		}

		// Vertex Shader
		if (!srcVertexShader.empty())
		{
			nVertexShaderID = gl.glCreateShader(gl.GL_VERTEX_SHADER_X);
			const char* s = srcVertexShader.c_str();
			gl.glShaderSource(nVertexShaderID, 1, &s, nullptr);
			gl.glCompileShader(nVertexShaderID);
			
			// Display Vertex Shader Compile Errors
			int32_t nCompileStatus = 0;
			gl.glGetShaderiv(nVertexShaderID, gl.GL_COMPILE_STATUS_X, &nCompileStatus);
			if (nCompileStatus == 0)
			{
				int32_t nInfoLogLength = 0;
				gl.glGetShaderiv(nVertexShaderID, gl.GL_INFO_LOG_LENGTH_X, &nInfoLogLength);
				std::vector<char> vInfoLog(nInfoLogLength);
				gl.glGetShaderInfoLog(nVertexShaderID, nInfoLogLength, nullptr, vInfoLog.data());
				return std::string("Vertex Shader Compile Error:\n") + std::string(vInfoLog.data());				
			}

			gl.glAttachShader(nCompiledShaderID, nVertexShaderID);
		}


		// Geometry Shader
		if (!srcGeometryShader.empty())
		{
			nGeometryShaderID = gl.glCreateShader(gl.GL_GEOMETRY_SHADER_X);
			const char* s = srcGeometryShader.c_str();
			gl.glShaderSource(nGeometryShaderID, 1, &s, nullptr);
			gl.glCompileShader(nGeometryShaderID);
			
			// Display Vertex Shader Compile Errors
			int32_t nCompileStatus = 0;
			gl.glGetShaderiv(nGeometryShaderID, gl.GL_COMPILE_STATUS_X, &nCompileStatus);
			if (nCompileStatus == 0)
			{
				int32_t nInfoLogLength = 0;
				gl.glGetShaderiv(nGeometryShaderID, gl.GL_INFO_LOG_LENGTH_X, &nInfoLogLength);
				std::vector<char> vInfoLog(nInfoLogLength);
				gl.glGetShaderInfoLog(nGeometryShaderID, nInfoLogLength, nullptr, vInfoLog.data());
				return std::string("Geometry Shader Compile Error:\n") + std::string(vInfoLog.data());
				
			}


			gl.glAttachShader(nCompiledShaderID, nGeometryShaderID);
		}

		gl.glLinkProgram(nCompiledShaderID);

		// Required PGE3 Uniforms
		CreateUniform("pgeMVP");
		CreateUniform("pgeDrawType");
		CreateUniform("pgeGlobalTint");
		CreateUniform("pgeTargetSizeInPixels");
		CreateUniform("pgeInverseTargetSizeInPixels");
		CreateUniform("pgeTotalTimeElapsed");

		CreateUniform("pgeTexture0");
		CreateUniform("pgeTexture1");
		CreateUniform("pgeTexture2");
		CreateUniform("pgeTexture3");

		return "OK";
	}

	int32_t Shader_GLSL33::CreateUniform(const std::string& name)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		const char* s = name.c_str();
		int32_t nID = gl.glGetUniformLocation(nCompiledShaderID, s);
		if (nID != -1)
		{
			mapUniforms.insert({ name, nID });
			return GetUniform(name);
		}
		else
			return -1;
	}


	bool Renderer_OGL33::CreateDevice(std::vector<void*> os_win_id, const RendererConfig& cfg)
	{
		config = cfg;


#if !defined(OLC_USE_WXWIDGETS)

#if OLC_HOST == OLC_HOST_WINDOWS
		// Create OpenGL Device Context
		if (!PrepareWindowTarget(os_win_id))
		{
			return false;
		}

		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));
		HGLRC tempContext = wglCreateContext(glDeviceContext);
		if (!tempContext)
		{
			lastError = RendererError::FailedToCreateRenderContext;
			return false;
		}
		wglMakeCurrent(glDeviceContext, tempContext);

		typedef HGLRC(WINAPI* PFN_wglCreateContextAttribsARB)(HDC, HGLRC, const int*);
		auto pfnCreateContextAttribs = (PFN_wglCreateContextAttribsARB)wglGetProcAddress("wglCreateContextAttribsARB");

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(tempContext);

		if (pfnCreateContextAttribs)
		{
			int gl33_attribs[] = {
				0x2091, 3,			// WGL_CONTEXT_MAJOR_VERSION_ARB = 3
				0x2092, 3,			// WGL_CONTEXT_MINOR_VERSION_ARB = 3	
				0x2094, 0,			// WGL_CONTEXT_FLAGS_ARB = 0 (no flags)
				0x9126, 0x00000002, // WGL_CONTEXT_PROFILE_MASK_ARB = COMPATIBILITY
				0 };
			glRenderContext = pfnCreateContextAttribs(glDeviceContext, nullptr, gl33_attribs);
		}
		else
			glRenderContext = wglCreateContext(glDeviceContext);

		// Create OpenGL Render Context
		if (!glRenderContext)
		{
			lastError = RendererError::FailedToCreateRenderContext;
			return false;
		}

		if (!wglMakeCurrent(glDeviceContext, glRenderContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}

#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
		const auto window_handle = reinterpret_cast<X11::Window>(os_win_id[0]);
		auto* display = reinterpret_cast<X11::Display*>(os_win_id[1]);
        GLint olc_GLAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, X11::None };

		X11::XVisualInfo* olc_VisualInfo = X11::glXChooseVisual(display, 0, olc_GLAttribs);
		glRenderContext = X11::glXCreateContext(display, olc_VisualInfo, nullptr, GL_TRUE);
		glXMakeCurrent(display, window_handle, glRenderContext);

		X11::XWindowAttributes gwa;
		X11::XGetWindowAttributes(display, window_handle, &gwa);
		glViewport(0, 0, gwa.width, gwa.height);
#endif

#if OLC_HOST == OLC_HOST_MACOS
        
		// os_win_id[0] is the OLC OpenGL Device Context      
        glRenderContext = (olc::apis::opengl::glRenderContext_t)os_win_id[0];
        if (CGLSetCurrentContext((CGLContextObj)glRenderContext) != kCGLNoError) {
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}

#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND || OLC_HOST == OLC_HOST_ANDROID
#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_ANDROID
    #if OLC_HOST == OLC_HOST_ANDROID
        EGLNativeWindowType window_handle = reinterpret_cast<ANativeWindow*>(os_win_id[0]);
    #else
        EGLNativeWindowType window_handle = NULL;
    #endif
	EGLNativeDisplayType display = EGL_DEFAULT_DISPLAY;
#else
	const auto wayland_window = reinterpret_cast<olc::host::WaylandWindow*>(os_win_id[0]);
	EGLNativeWindowType window_handle = reinterpret_cast<EGLNativeWindowType>(wayland_window->window);
	EGLNativeDisplayType display = reinterpret_cast<EGLNativeDisplayType>(os_win_id[1]);
#endif

	glRenderContext.display = eglGetDisplay(display);
	if(glRenderContext.display == EGL_NO_DISPLAY) {
		std::cout << "Could not create EGL Display" << std::endl;
	}

	eglInitialize(glRenderContext.display, nullptr, nullptr);

#if OLC_HOST == OLC_HOST_ANDROID
	glRenderContext.config = FindBestConfig(glRenderContext.display, OLC_MSAA_SAMPLES);
#else
	EGLint num_config;
	EGLint const attribute_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_SAMPLE_BUFFERS, 1,
        EGL_SAMPLES, OLC_MSAA_SAMPLES,
        EGL_NONE
    };
	eglChooseConfig(glRenderContext.display, attribute_list, &glRenderContext.config, 1, &num_config);
#endif

	EGLint const context_config[] = {EGL_CONTEXT_MAJOR_VERSION, 3, EGL_NONE};

	/* create an EGL rendering context */
#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
	eglBindAPI(EGL_OPENGL_API);
#endif
	glRenderContext.context = eglCreateContext(glRenderContext.display, glRenderContext.config, EGL_NO_CONTEXT, context_config);
	glRenderContext.surface = eglCreateWindowSurface(glRenderContext.display, glRenderContext.config, window_handle, nullptr);
	if(glRenderContext.surface == EGL_NO_SURFACE) {
		std::cout << "Could not create EGL Surface" << std::endl;
	}

	if(!eglMakeCurrent(glRenderContext.display, glRenderContext.surface, glRenderContext.surface, glRenderContext.context))
	{
		lastError = RendererError::FailedToCreateRenderContext;
		return false;
	}
#endif

#else // wxWidgets specific

#endif // wxWidgets

		// Can't load OpenGL API until context is loaded
		auto& gl = olc::apis::opengl::gl::Get();
		if (!gl.HasLoaded())
		{
			std::cout << "Error: Could not Load OpenGL!\n";
			lastError = RendererError::NoError;
			return false;
		}

		// Store the initial (screen) framebuffer binding
		// On most platforms the system provides a default framebuffer of 0
		// However on iOS there is no system buffer and instead a GLKit creates an FBO to use
		gl.glGetIntegerv(gl.GL_DRAW_FRAMEBUFFER_BINDING_X, (GLint *)&nScreenFBO);

		// Configure Swap Interval (VSync)
		if (config.VerticalSync)
		{
			// Enable VSync - lock to display refresh
			// May also be governed by OS / driver settings
			// and desktop compositor settings
#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND
			eglSwapInterval(glRenderContext.display, 1);
#elif OLC_HOST == OLC_HOST_LINUX_X11
			gl.XSwapIntervalEXT(display, window_handle, 1);
#else
			gl.glSwapInterval(1);
#endif

		}
		else
		{
			// Disable VSync - run like the clappers!
#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND
			eglSwapInterval(glRenderContext.display, 0);
#elif OLC_HOST == OLC_HOST_LINUX_X11
			gl.XSwapIntervalEXT(display, window_handle, 0);
#else
			gl.glSwapInterval(0);
#endif
		}
		

		// Create "Default" Shader
		shaderDefault.SetPixelShaderSource(
			Shader_GLSL33::PS_DefaultHeader() + Shader_GLSL33::PS_DefaultMain());

		shaderDefault.SetVertexShaderSource(
			Shader_GLSL33::VS_DefaultHeader() + Shader_GLSL33::VS_DefaultMain());

		shaderDefault.SetGeometryShaderSource(
			Shader_GLSL33::GS_DefaultHeader() + Shader_GLSL33::GS_DefaultMain());

		std::string sResult = shaderDefault.Compile();
		if (sResult != "OK")
		{
			std::cout << "Error compiling default shader: " << sResult << std::endl;
			lastError = RendererError::FailedToCompileShader;
			return false;
		}

		// Create "Default" Vertex Buffer / Vertex Attributes. This buffer is reused
		// for all drawing operations. It's possible future versions may allow the
		// creation of additional named buffers for repeated drawing operations with
		// minimal overhead.
		gl.glGenBuffers(1, &nDefaultVB);
		gl.glGenVertexArrays(1, &nDefaultVA);
		gl.glBindVertexArray(nDefaultVA);
		gl.glBindBuffer(gl.GL_ARRAY_BUFFER_X, nDefaultVB);

		// A big one is allocated to reduce shuffles in GPU memory
		GPUTask::Vertex verts[OLC_GPU_MAX_VERTICES];
		gl.glBufferData(gl.GL_ARRAY_BUFFER_X, sizeof(GPUTask::Vertex) * OLC_GPU_MAX_VERTICES, verts, gl.GL_STREAM_DRAW_X);
		
		// Float Index 0 = x, 1 = y, 2 = z, 3 = w
		gl.glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex),        (void*)(0 * sizeof(float)));
		gl.glEnableVertexAttribArray(0);		
		// Float Index 4 = (RGBA 8-bit x4)
		gl.glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GPUTask::Vertex), (void*)(4 * sizeof(float)));	
		gl.glEnableVertexAttribArray(1);
		// Float Index 5 = u0, 6 = v0
		gl.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex),        (void*)(5 * sizeof(float)));
		gl.glEnableVertexAttribArray(2);
		// Float Index 7 = u1, 8 = v1
		gl.glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex), (void*)(7 * sizeof(float)));
		gl.glEnableVertexAttribArray(3);
		// Float Index 9 = u2, 10 = v2
		gl.glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex), (void*)(9 * sizeof(float)));
		gl.glEnableVertexAttribArray(4);
		// Float Index 11 = u3, 12 = v4
		gl.glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex), (void*)(11 * sizeof(float)));
		gl.glEnableVertexAttribArray(5);

		// Buffers are configured, unbind for now
		gl.glBindBuffer(gl.GL_ARRAY_BUFFER_X, 0);
		gl.glBindVertexArray(0);


		


		// Create a null-texture so sampler doesnt fail. We don't have some of the core's helper
		// functions here, so we construct it manually
		imgBlank.CreateNoGPU({ 1,1 });
		imgBlank.SetGPUID(CreateTexture(imgBlank.Size()));
		imgBlank.BindCPU();
		imgBlank.Pixel({ 0,0 }) = olc::Colour::WHITE;
		imgBlank.BindGPU();
		WriteTexture(imgBlank.GetGPUID(), imgBlank);

		// Create a Frame Buffer Object for off-screen rendering things
		gl.glGenFramebuffers(1, (GLuint*)&nDefaultFBO);
		gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nDefaultFBO);

		// Create a shared depth renderbuffer (will be resized dynamically)
		gl.glGenRenderbuffers(1, &nDepthRBO);
		gl.glBindRenderbuffer(gl.GL_RENDERBUFFER_X, nDepthRBO);
		// Allocate with a default size (will be resized when needed)
		gl.glRenderbufferStorage(gl.GL_RENDERBUFFER_X, gl.GL_DEPTH_COMPONENT24_X, 1024, 1024);
		gl.glFramebufferRenderbuffer(gl.GL_FRAMEBUFFER_X, gl.GL_DEPTH_ATTACHMENT_X, gl.GL_RENDERBUFFER_X, nDepthRBO);
		vCurrentDepthSize = {1024, 1024};
		nCurrentDepthSamples = 0;

		// Attach 4 colour buffers
		std::array<GLenum, 4> attachments = 
		{ {
			gl.GL_COLOR_ATTACHMENT0_X,
			gl.GL_COLOR_ATTACHMENT0_X + 1,
			gl.GL_COLOR_ATTACHMENT0_X + 2,
			gl.GL_COLOR_ATTACHMENT0_X + 3
		} };

		gl.glDrawBuffers(4, attachments.data());
		// Unlink them from any existing image textures
		//gl.glFramebufferTexture2D(gl.GL_FRAMEBUFFER_X, attachments[0], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(gl.GL_FRAMEBUFFER_X, attachments[1], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(gl.GL_FRAMEBUFFER_X, attachments[2], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(gl.GL_FRAMEBUFFER_X, attachments[3], GL_TEXTURE_2D, 0, 0);

		// Unbind the FBO
		gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nScreenFBO);

		// Create FBOs for MSAA resolve operations
		gl.glGenFramebuffers(1, &nResolveFBO_Draw);
		gl.glGenFramebuffers(1, &nResolveFBO_Read);

		// PGE Specific requirements

		// Texturing Enabled
#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
		gl.glEnable(GL_TEXTURE_2D); // Turn on texturing
		gl.glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
		// Alpha Blending Enabled
		gl.glEnable(GL_BLEND);

		// Front Face is Counter-Clockwise
		gl.glFrontFace(GL_CCW);

		lastError = RendererError::NoError;
		return true;
	}

	bool Renderer_OGL33::DestroyDevice()
	{
		auto& gl = olc::apis::opengl::gl::Get();
		
		// Delete depth renderbuffer
		if (nDepthRBO != 0)
		{
			gl.glDeleteRenderbuffers(1, &nDepthRBO);
			nDepthRBO = 0;
		}
	
#if OLC_HOST == OLC_HOST_WINDOWS
		wglDeleteContext(glRenderContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS
		CGLSetCurrentContext(NULL);
		CGLDestroyContext((CGLContextObj)glRenderContext);
#endif
#if OLC_HOST == OLC_HOST_LINUX_X11
		auto* display = X11::XOpenDisplay(nullptr);
		X11::glXMakeCurrent(display, 0, NULL);
		X11::glXDestroyContext(display, glRenderContext);
#endif
#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND || OLC_HOST == OLC_HOST_ANDROID
		eglMakeCurrent(glRenderContext.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(glRenderContext.display, glRenderContext.context);
		eglDestroySurface(glRenderContext.display, glRenderContext.surface);
		eglTerminate(glRenderContext.display);
		glRenderContext.display = EGL_NO_DISPLAY;
		glRenderContext.surface = EGL_NO_SURFACE;
		glRenderContext.context = EGL_NO_CONTEXT;
#endif
		return false;
	}

	bool Renderer_OGL33::RetargetDevice(std::vector<void*> os_win_id)
	{
#if OLC_HOST == OLC_HOST_WINDOWS
		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));

		if (!wglMakeCurrent(glDeviceContext, glRenderContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;			
			return false;
		}
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS
    
        auto glDeviceContext = (olc::apis::opengl::glRenderContext_t)os_win_id[0];
		if (CGLSetCurrentContext((CGLContextObj)glDeviceContext) != kCGLNoError) {
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}
#endif
#if OLC_HOST == OLC_HOST_LINUX_X11
		const auto window = reinterpret_cast<X11::Window>(os_win_id[0]);
		auto* display = reinterpret_cast<X11::Display*>(os_win_id[1]);
		if(!X11::glXMakeCurrent(display, window, glRenderContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}
#endif
#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND || OLC_HOST == OLC_HOST_ANDROID
	if(!eglMakeCurrent(glRenderContext.display, glRenderContext.surface, glRenderContext.surface, glRenderContext.context))
	{
		lastError = RendererError::FailedToSwitchRenderContext;
		return false;
	}
#endif
		return true;
	}

	bool Renderer_OGL33::PrepareWindowTarget(std::vector<void*> os_win_id)
	{
#if OLC_HOST == OLC_HOST_WINDOWS
		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		int pf = 0;
		if (!(pf = ChoosePixelFormat(glDeviceContext, &pfd)))
		{
			lastError = RendererError::InvalidDCPixelFormat;
			return false;
		}

		if (!SetPixelFormat(glDeviceContext, pf, &pfd))
		{
			lastError = RendererError::FailedToSetDCPixelFormat;
			return false;
		}
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS
             
        // params[0] is the OLC OpenGL Device Context      
        glRenderContext = (olc::apis::opengl::glRenderContext_t)os_win_id[0];
		if (CGLSetCurrentContext((CGLContextObj)glRenderContext) != kCGLNoError) {
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}

#endif
		return true;
	}

	uint32_t Renderer_OGL33::CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// Curiously OpenGL doesnt actually care about the size of the texture
		// as part of its creation.	This matters later when we Write to texture
		// resources on GPU
		
		uint32_t id = 0;

		// Helper function to create regular (non-MSAA) texture
		auto CreateRegularTexture = [&]()
		{
			uint32_t new_id = 0;
			gl.glGenTextures(1, &new_id);
			glBindTexture(GL_TEXTURE_2D, new_id);

			if (cfg.Filtered)
			{
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

			if (cfg.Clamp)
			{
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
			else
			{
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			return new_id;
		};

		// Create the regular texture (used for sampling)
		id = CreateRegularTexture();

		// If MSAA is requested, also create a renderbuffer for MSAA rendering
		if (cfg.MSAA)
		{
			uint32_t rboId = 0;
			gl.glGenRenderbuffers(1, &rboId);
			gl.glBindRenderbuffer(gl.GL_RENDERBUFFER_X, rboId);
			
			// Map texture to its MSAA renderbuffer
			mapTextureToRenderbuffer[id] = rboId;
		}

#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_MACOS && OLC_HOST != OLC_HOST_ANDROID
		gl.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif

		mapTextureSizes[id] = vSize;
		return id;
	}

	bool Renderer_OGL33::WriteTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// Always write to the regular texture (for sampling)
		gl.glBindTexture(GL_TEXTURE_2D, texid);
		gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Size().x, image.Size().y, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, image.Data());

		// If this texture has MSAA, allocate storage for the renderbuffer
		if (mapTextureToRenderbuffer.contains(texid))
		{
			uint32_t rboId = mapTextureToRenderbuffer[texid];
			gl.glBindRenderbuffer(gl.GL_RENDERBUFFER_X, rboId);

			int32_t maxSamples = 0;
			gl.glGetInternalformativ(gl.GL_RENDERBUFFER_X, GL_RGBA8, gl.GL_SAMPLES_X, 1, &maxSamples);

			// Allocate MSAA renderbuffer storage
			gl.glRenderbufferStorageMultisample(
				gl.GL_RENDERBUFFER_X,
				std::min<int32_t>(image.GetConfig().MSAASamples, maxSamples),
				GL_RGBA8,
				image.Size().x,
				image.Size().y
			);

			// Unbind renderbuffer
			gl.glBindRenderbuffer(gl.GL_RENDERBUFFER_X, 0);
		}

		// Update size tracking
		mapTextureSizes[texid] = image.Size();

		return true;
	}

	bool Renderer_OGL33::ReadTexture(const uint32_t texid, olc::Image& image)
	{
		olc_IgnoreUnused(texid);

		auto& gl = olc::apis::opengl::gl::Get();

		// Read the teture data back into the image
		// With renderbuffer approach, we always read from the regular texture
		// which has been blitted to via ResolveMSAA if its an MSAA texture
		gl.glBindTexture(GL_TEXTURE_2D, image.GetGPUID());

#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
		gl.glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
#else		
		gl.glReadPixels(0, 0, image.Size().x, image.Size().y, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
#endif
		return true;
	}

	bool Renderer_OGL33::DeleteTexture(const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		
		// Delete the texture
		gl.glDeleteTextures(1, &texid);
		
		// Delete associated renderbuffer if it exists
		if (mapTextureToRenderbuffer.contains(texid))
		{
			uint32_t rboId = mapTextureToRenderbuffer[texid];
			gl.glDeleteRenderbuffers(1, &rboId);
			mapTextureToRenderbuffer.erase(texid);
		}

		mapTextureSizes.erase(texid);

		if (nCurrentTextureSource == texid)
			nCurrentTextureSource = 0;

		if (nCurrentTextureTarget == texid)
			nCurrentTextureTarget = 0;
		
		return true;
	}

	bool Renderer_OGL33::AssignTextureSource(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// This function binds a texture to a texture slot for sampling. If the
		// texture is an MSAA texture, we need to use the resolved version for sampling

		// Check if this is an MSAA texture...
		uint32_t actualTexId = texid;
		//if (mapMSAAToResolved.contains(texid))
		//{
			// ...yes it is, so use the resolved texture for sampling
		//	actualTexId = mapMSAAToResolved[texid];
		//}

		// If the requested source texture is currently attached as the render target,
		// unbind the framebuffer to avoid sampling from a texture that's being written to.
		if (actualTexId == nCurrentTextureTarget && actualTexId != 0)
		{
#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning ATS: Requested source is currently attached as target (" << actualTexId << ") - unbinding FBO\n";
#endif
			gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nScreenFBO);
			nCurrentTextureTarget = 0;
		}

		//if (nCurrentTextureSource == texid)
		//	return true;

		// Bind texture to specified texture slot
		gl.glActiveTexture(gl.GL_TEXTURE0_X + slot);
		gl.glBindTexture(GL_TEXTURE_2D, actualTexId);

		// Record currently bound source texture
		nCurrentTextureSource = actualTexId;
		return true;
	}

	// Assign a texture as the current render target. 
	// If texid is 0, bind the default framebuffer (screen)
	bool Renderer_OGL33::AssignTextureTarget(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// This function attaches a texture to the FBO for rendering.

		// If the requested target texture is currently bound as a source, unbind it
		// from all texture units to ensure we do not sample from a texture that's
		// attached to the FBO (undefined behavior).
		if (texid != 0 && texid == nCurrentTextureSource)
		{
#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning ATT: Requested target is currently bound as source (" << texid << ") - unbinding texture units\n";
#endif
			// Unbind from a reasonable number of texture units (0..7) used by this renderer
			for (int i = 0; i < 8; ++i)
			{
				gl.glActiveTexture(gl.GL_TEXTURE0_X + i);
				gl.glBindTexture(GL_TEXTURE_2D, 0);
			}

			// Reset to texture unit 0
			gl.glActiveTexture(gl.GL_TEXTURE0_X);
			nCurrentTextureSource = 0;
		}
		
		if (texid == 0)
		{
			// Unbind the FBO (bind default framebuffer)
			nActiveAttachmentMask = 0x01;
			gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nScreenFBO);
			return true;
		}	
	
		// Bind FBO
		gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nDefaultFBO);

		// Resize depth buffer to match target texture dimensions
		olc::vi2d targetSize = mapTextureSizes[texid];
		int32_t targetSamples = 0;

		// Check if this is an MSAA texture
		bool bIsMSAA = mapTextureToRenderbuffer.contains(texid);
		if (bIsMSAA)
		{
			// Get the MSAA sample count from the color renderbuffer
			uint32_t rboId = mapTextureToRenderbuffer[texid];
			gl.glBindRenderbuffer(gl.GL_RENDERBUFFER_X, rboId);
			gl.glGetRenderbufferParameteriv(gl.GL_RENDERBUFFER_X, gl.GL_RENDERBUFFER_SAMPLES_X, &targetSamples);
		}

		// Only resize if dimensions or sample count changed
		if (targetSize != vCurrentDepthSize || targetSamples != nCurrentDepthSamples)
		{
			gl.glBindRenderbuffer(gl.GL_RENDERBUFFER_X, nDepthRBO);
			
			if (bIsMSAA && targetSamples > 0)
			{
				// Allocate MSAA depth buffer
				gl.glRenderbufferStorageMultisample(
					gl.GL_RENDERBUFFER_X,
					targetSamples,
					gl.GL_DEPTH_COMPONENT24_X,
					targetSize.x,
					targetSize.y
				);
			}
			else
			{
				// Allocate regular depth buffer
				gl.glRenderbufferStorage(
					gl.GL_RENDERBUFFER_X,
					gl.GL_DEPTH_COMPONENT24_X,
					targetSize.x,
					targetSize.y
				);
			}
			
			// Update tracked size and samples
			vCurrentDepthSize = targetSize;
			nCurrentDepthSamples = targetSamples;
			
			// Re-attach depth buffer to FBO
			gl.glFramebufferRenderbuffer(
				gl.GL_FRAMEBUFFER_X, 
				gl.GL_DEPTH_ATTACHMENT_X, 
				gl.GL_RENDERBUFFER_X, 
				nDepthRBO
			);
		}

		// Update active attachment bitmask and rebuild draw buffers
		nActiveAttachmentMask |= (1 << slot);
		RebuildDrawBuffers();

		// If target texture is MSAA, enable multisampling
		if (mapTextureToRenderbuffer.contains(texid))
		{
			// MSAA texture - attach renderbuffer
			uint32_t rboId = mapTextureToRenderbuffer[texid];
			gl.glFramebufferRenderbuffer(
				gl.GL_FRAMEBUFFER_X, 
				gl.GL_COLOR_ATTACHMENT0_X + slot, 
				gl.GL_RENDERBUFFER_X, 
				rboId
			);
		}
		else
		{
			// Regular texture - attach directly
			gl.glFramebufferTexture2D(
				gl.GL_FRAMEBUFFER_X, 
				gl.GL_COLOR_ATTACHMENT0_X + slot, 
				GL_TEXTURE_2D, 
				texid, 
				0
			);
		}

#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
		// Check FBO complete state
		GLenum status = gl.glCheckFramebufferStatus(gl.GL_FRAMEBUFFER_X);
		if (status != gl.GL_FRAMEBUFFER_COMPLETE_X)
		{
			std::cout << "ERROR: FBO incomplete\n";
		}
#endif

	nCurrentTextureTarget = texid;		
	return true;
	}

	bool Renderer_OGL33::DetachTextureTarget(const uint32_t slot)
	{
		// No-op if slot is not active
		if (!(nActiveAttachmentMask & (1 << slot)))
			return true;

		auto &gl = olc::apis::opengl::gl::Get();

		// Detach texture from this attachment slot
		gl.glFramebufferTexture2D(
			gl.GL_FRAMEBUFFER_X,
			gl.GL_COLOR_ATTACHMENT0_X + slot,
			gl.GL_TEXTURE_2D_X,
			0,
			0);

		// Also detach any renderbuffer (MSAA case)
		gl.glFramebufferRenderbuffer(
			gl.GL_FRAMEBUFFER_X,
			gl.GL_COLOR_ATTACHMENT0_X + slot,
			gl.GL_RENDERBUFFER_X,
			0);

		// Clear the bit and rebuild draw buffers
		nActiveAttachmentMask &= ~(1 << slot);
		RebuildDrawBuffers();

		return true;
	}

	int Renderer_OGL33::RebuildDrawBuffers()
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// GL_NONE is required for inactive slots so that the fragment shader
		// `layout(location = N)` outputs are mapped correctly
		std::array<GLenum, 8> drawBuffers;
		int maxSlot = 0;
		for (int i = 0; i < 8; i++)
		{
			if (nActiveAttachmentMask & (1 << i))
			{
				drawBuffers[i] = gl.GL_COLOR_ATTACHMENT0_X + i;
				maxSlot = i + 1;
			}
			else
			{
				drawBuffers[i] = gl.GL_NONE_X;
			}
		}
		if (maxSlot > 0)
			gl.glDrawBuffers(maxSlot, drawBuffers.data());
		return maxSlot;
	}

	bool Renderer_OGL33::ResolveMSAA(const uint32_t texid)
	{
		// Check if this texture has an MSAA renderbuffer
		if (!mapTextureToRenderbuffer.contains(texid))
			return true;  // Not MSAA, nothing to do

		auto& gl = olc::apis::opengl::gl::Get();

		// It did! Get the renderbuffer ID
		uint32_t rboId = mapTextureToRenderbuffer[texid];

		// Ensure all rendering to MSAA texture is finished
		glFinish();

		// Bind renderbuffer to read FBO
		gl.glBindFramebuffer(gl.GL_READ_FRAMEBUFFER_X, nResolveFBO_Read);
		gl.glFramebufferRenderbuffer(
			gl.GL_READ_FRAMEBUFFER_X, 
			gl.GL_COLOR_ATTACHMENT0_X, 
			gl.GL_RENDERBUFFER_X, 
			rboId
		);

		// Bind regular texture to draw FBO
		gl.glBindFramebuffer(gl.GL_DRAW_FRAMEBUFFER_X, nResolveFBO_Draw);
		gl.glFramebufferTexture2D(
			gl.GL_DRAW_FRAMEBUFFER_X, 
			gl.GL_COLOR_ATTACHMENT0_X, 
			GL_TEXTURE_2D, 
			texid, 
			0
		);

#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
		// Check FBO complete state
		GLenum readStatus = gl.glCheckFramebufferStatus(gl.GL_READ_FRAMEBUFFER_X);
		GLenum drawStatus = gl.glCheckFramebufferStatus(gl.GL_DRAW_FRAMEBUFFER_X);
		if (readStatus != gl.GL_FRAMEBUFFER_COMPLETE_X || drawStatus != gl.GL_FRAMEBUFFER_COMPLETE_X)
		{
			std::cout << "ResolveMSAA ERROR: FBO incomplete!\n";
			return false;
		}
#endif

		// Blit from MSAA renderbuffer to resolved
		olc::vi2d size = mapTextureSizes[texid];
		gl.glBlitFramebuffer(
			0, 0, size.x, size.y,
			0, 0, size.x, size.y,
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST
		);

		// Restore to default framebuffer (screen)
		gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nDefaultFBO);
		return true;
	}

	bool Renderer_OGL33::ApplyShader(const Shader& shader)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		pCurrentShader = &shader;
		gl.glUseProgram(pCurrentShader->GetShaderID());

		// Set default texture slots if they exist in the shader
		int32_t loc0 = pCurrentShader->GetUniform("pgeTexture0");
		if (loc0 != -1) { gl.glUniform1i(loc0, 0); } // Texture slot 0
		int32_t loc1 = pCurrentShader->GetUniform("pgeTexture1");
		if (loc1 != -1) { gl.glUniform1i(loc1, 1); } // Texture slot 1
		int32_t loc2 = pCurrentShader->GetUniform("pgeTexture2");
		if (loc2 != -1) { gl.glUniform1i(loc2, 2); } // Texture slot 2
		int32_t loc3 = pCurrentShader->GetUniform("pgeTexture3");
		if (loc3 != -1) { gl.glUniform1i(loc3, 3); } // Texture slot 3
		
		return true;
	}

	bool Renderer_OGL33::ApplyDefaultShader()
	{	
		return ApplyShader(shaderDefault);
	}

	bool Renderer_OGL33::SetUniform(const std::string& name, const float value)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		int loc = pCurrentShader->GetUniform(name);
		if (loc == -1)
		{
			#if OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning: Uniform '" << name << "' not found in current shader\n";
			#endif
			return false;
		}
		gl.glUniform1f(loc, value);
		return true;
	}

	bool Renderer_OGL33::SetUniform(const std::string& name, const olc::vf2d& value)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		int loc = pCurrentShader->GetUniform(name);
		if (loc == -1)
		{
			#if OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning: Uniform '" << name << "' not found in current shader\n";
			#endif
			return false;
		}
		gl.glUniform2fv(loc, 1, value.a().data());
		return true;
	}

	bool Renderer_OGL33::SetUniform(const std::string& name, const olc::Pixel value)
	{
		float f[4] = {
			float(value.r) / 255.0f,
			float(value.g) / 255.0f,
			float(value.b) / 255.0f,
			float(value.a) / 255.0f
		};

		auto& gl = olc::apis::opengl::gl::Get();
		int loc = pCurrentShader->GetUniform(name);
		if (loc == -1)
		{
			#if OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning: Uniform '" << name << "' not found in current shader\n";
			#endif
			return false;
		}
		gl.glUniform4fv(loc, 1, f);
		return true;
	}

	bool Renderer_OGL33::DoGPUTask(const olc::GPUTask& task)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		switch (task.task)
		{
			case GPUTask::Task::NullTask:
			break;
			case GPUTask::Task::DrawPolygon:
			{
				
				if (task.pImage == nullptr)
					AssignTextureSource(0, imgBlank.GetGPUID());
				else
				{
					if (nCurrentTextureSource != task.pImage->GetGPUID())
						AssignTextureSource(0, task.pImage->GetGPUID());
				}

				// Bind generic vertex buffer
				gl.glBindVertexArray(nDefaultVA);
				gl.glBindBuffer(gl.GL_ARRAY_BUFFER_X, nDefaultVB);
				
				// Copy data from CPU to GPU
				gl.glBufferData(gl.GL_ARRAY_BUFFER_X, sizeof(GPUTask::Vertex) * task.vertexBuffer.size(), task.vertexBuffer.data(), gl.GL_STREAM_DRAW_X);
								
				// Configure shader with expected values
				SetUniform("pgeGlobalTint", task.tint);
				SetUniform("pgeTargetSizeInPixels", vTargetSize);
				SetUniform("pgeInverseTargetSizeInPixels", (1.0f / vTargetSize));
				SetUniform("pgeTotalTimeElapsed", fTotalTime);

				

				// Apply Culling modes
				if (task.cullmode == olc::CullMode::None)
				{
					gl.glDisable(GL_CULL_FACE);
				}
				else if (task.cullmode == olc::CullMode::ClockWise)
				{
					gl.glCullFace(GL_FRONT);
					gl.glEnable(GL_CULL_FACE);
				}
				else if (task.cullmode == olc::CullMode::CounterClockWise)
				{
					gl.glCullFace(GL_BACK);
					gl.glEnable(GL_CULL_FACE);
				}

				// Apply Depth Testing (if required)
				if (task.bDepth)
				{
					gl.glEnable(GL_DEPTH_TEST);
					gl.glDepthFunc(GL_LESS);
				}


				// Apply Blending Mode
				if (task.blendmode == olc::BlendMode::Alpha)
				{
					gl.glEnable(GL_BLEND);
					//gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					gl.glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				}
				else if(task.blendmode == olc::BlendMode::Additive)
				{
					gl.glEnable(GL_BLEND);
					gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				}
				else if(task.blendmode == olc::BlendMode::Multiplicative)
				{
					gl.glEnable(GL_BLEND);
					gl.glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
				}
				else if (task.blendmode == olc::BlendMode::None)
				{
					gl.glDisable(GL_BLEND);
				}


				//gl.glEnable(GL_BLEND);
				//gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//gl.glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

				//// Apply Rendering Mode
				//if (task.bWireframe)
				//	gl.glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				if (task.bIs3D)
				{
					gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 2);
					gl.glUniformMatrix4fv(pCurrentShader->GetUniform("pgeMVP"), 1, true, task.mvpMatrix.data());
				}
				else
				{
					if (task.bPixelConstrained)
					{
						if (task.structure == olc::Structure::Point)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 1);
						else if (task.structure == olc::Structure::Line)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 1);
						else if (task.structure == olc::Structure::LineLoop)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 1);
						else if (task.structure == olc::Structure::LineList)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 1);
						else
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 0);
					}
					else
					{
						if (task.structure == olc::Structure::Point)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 3);
						else if (task.structure == olc::Structure::Line)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 3);
						else if (task.structure == olc::Structure::LineLoop)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 3);
						else if (task.structure == olc::Structure::LineList)
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 3);
						else
							gl.glUniform1i(pCurrentShader->GetUniform("pgeDrawType"), 4);
					}
				}

				if (task.structure == olc::Structure::Fan)
					gl.glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::Strip)
					gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::List)
					gl.glDrawArrays(GL_TRIANGLES, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::Line)
					gl.glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::LineList)
					gl.glDrawArrays(GL_LINES, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::LineLoop)
					gl.glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::Point)
					gl.glDrawArrays(GL_POINTS, 0, (GLsizei)task.vertexBuffer.size());


				//if (task.bWireframe)
				//	gl.glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				if (task.bDepth)
					gl.glDisable(GL_DEPTH_TEST);

			}
			break;
		}

		return true;
	}

	bool Renderer_OGL33::ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glClearColor(float(col.r) / 255.0f, float(col.g) / 255.0f, float(col.b) / 255.0f, float(col.a) / 255.0f);		
		gl.glClear(GL_COLOR_BUFFER_BIT | (bDepth ? GL_DEPTH_BUFFER_BIT : 0) | (bStencil ? GL_STENCIL_BUFFER_BIT : 0));		
		return true;
	}

	bool Renderer_OGL33::SetViewport(const olc::vf2d& pos, const olc::vf2d& size)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glViewport(int(pos.x), int(pos.y), int(size.x), int(size.y));
		vTargetSize = size;
		return true;
	}

	bool Renderer_OGL33::DisplayPrepare(const float fFrameElapsedTime, const float fTotalElapsedTime)
	{
		fFrameTime = fFrameElapsedTime;
		fTotalTime = fTotalElapsedTime;

		auto& gl = olc::apis::opengl::gl::Get();

		gl.glEnable(GL_BLEND);
		gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl.glDepthFunc(GL_LESS);
		return false;
	}

	bool Renderer_OGL33::DisplayDraw(std::vector<void*> os_win_id, bool bVerticalSyncNow)
	{
		//auto& gl = olc::apis::opengl::gl::Get();

#if OLC_HOST == OLC_HOST_WINDOWS
		olc_IgnoreUnused(bVerticalSyncNow);
		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));
		SwapBuffers(glDeviceContext);
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif	

#if OLC_HOST == OLC_HOST_MACOS
		// The pointer value in os_win_id[1] will be set to true, when the OS requests to skip the frame swap
		olc_IgnoreUnused(bVerticalSyncNow);
        const bool* bSkipFrame = static_cast<const bool*>(os_win_id[1]);
		if (*bSkipFrame) return true;
		CGLContextObj cglContext = static_cast<CGLContextObj>(os_win_id[0]);
		CGLFlushDrawable(cglContext);
       
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
		const auto window_handle = reinterpret_cast<X11::Window>(os_win_id[0]);
		auto* display = reinterpret_cast<X11::Display*>(os_win_id[1]);
		X11::glXSwapBuffers(display, window_handle);
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND
	eglSwapBuffers(glRenderContext.display, glRenderContext.surface);
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_ANDROID
	eglSwapBuffers(glRenderContext.display, glRenderContext.surface);
#endif

		return true;
	}

#if OLC_HOST == OLC_HOST_ANDROID
    EGLConfig Renderer_OGL33::FindBestConfig(EGLDisplay display, int desiredMultisamples)
    {
		EGLint numConfigs;
		EGLConfig bestConfig = nullptr;
		EGLConfig fallbackConfig = nullptr;

		// Define attribute list for desired configuration
    	EGLint const attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 16,
        	EGL_NONE
    	};

		// Get all matching configurations
		eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

		std::vector<EGLConfig> configs(numConfigs);
		eglChooseConfig(display, attribs, configs.data(), numConfigs, &numConfigs);

		// Evaluate configurations to find the best that matches desired multisampling
		for (const auto& config : configs) {
			EGLint sampleBuffers = 0;
			EGLint samples = 0;

			eglGetConfigAttrib(display, config, EGL_SAMPLE_BUFFERS, &sampleBuffers);
			eglGetConfigAttrib(display, config, EGL_SAMPLES, &samples);

			if (sampleBuffers > 0 && samples == desiredMultisamples) {
				bestConfig = config;
				break; // Found the best match
			}

			// Keep track of a fallback configuration
			if (fallbackConfig == nullptr) {
				fallbackConfig = config;
			}
		}

		return bestConfig ? bestConfig : fallbackConfig;
    }
#endif

}
//! END IMPLEMENTATION
