#include "gpu_opengl33.h"

//! START IMPLEMENTATION
namespace olc::gpu
{
	std::string Shader_GLSL33::Compile()
	{
		auto& gl = olc::apis::opengl::gl::Get();

		nCompiledShaderID = gl.glCreateProgram();

		// Fragment Shader
		if (!srcPixelShader.empty())
		{
			nPixelShaderID = gl.glCreateShader(0x8B30);			
			const char* s = srcPixelShader.c_str();
			gl.glShaderSource(nPixelShaderID, 1, &s, nullptr);
			gl.glCompileShader(nPixelShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nPixelShaderID);
		}

		// Vertex Shader
		if (!srcVertexShader.empty())
		{
			nVertexShaderID = gl.glCreateShader(0x8B31);
			const char* s = srcVertexShader.c_str();
			gl.glShaderSource(nVertexShaderID, 1, &s, nullptr);
			gl.glCompileShader(nVertexShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nVertexShaderID);
		}


		// Geometry Shader
		if (!srcGeometryShader.empty())
		{
			nGeometryShaderID = gl.glCreateShader(0x8DD9);
			const char* s = srcGeometryShader.c_str();
			gl.glShaderSource(nGeometryShaderID, 1, &s, nullptr);
			gl.glCompileShader(nGeometryShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nGeometryShaderID);
		}

		gl.glLinkProgram(nCompiledShaderID);

		return "OK";
	}

	int32_t Shader_GLSL33::CreateUniform(const std::string& name)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		const char* s = name.c_str();
		mapUniforms.insert({ name, gl.glGetUniformLocation(nCompiledShaderID, s) });
		return GetUniform(name);
	}



	bool Renderer_OGL33::CreateDevice(std::vector<void*> os_win_id, const RendererConfig& cfg)
	{
		config = cfg;


		
#if OLC_HOST == OLC_HOST_WINDOWS
		// Create OpenGL Device Context
		if (!PrepareWindowTarget(os_win_id))
		{
			return false;
		}

		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));

		// Create OpenGL Render Context
		if (!(glRenderContext = wglCreateContext(glDeviceContext))) 
		{
			lastError = RendererError::FailedToCreateRenderContext;
			return false;
		}

		if (!wglMakeCurrent(glDeviceContext, glRenderContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}

		//// Set Vertical Sync
		//glSwapInterval = OGL_LOAD(glSwapInterval);
		//if (locSwapInterval && !bVSYNC) locSwapInterval(0);
		//bSync = bVSYNC;

#endif

#if OLC_HOST == OLC_HOST_MACOS
        
		// os_win_id[0] is the OLC OpenGL Device Context      
        glRenderContext = (olc::apis::opengl::glRenderContext_t)os_win_id[0];
        if (!glRenderContext) {
            lastError = RendererError::FailedToCreateRenderContext;
            return false;
        }

#endif

		// Can't load OpenGL API until context is loaded
		auto& gl = olc::apis::opengl::gl::Get();
		if (!gl.HasLoaded())
		{
			std::cout << "Error: Could not Load OpenGL!\n";
			lastError = RendererError::None;
			return false;
		}
		

		// Create "Default" Shader
		shaderDefault.SetPixelShaderSource(R"(
			#version 330 core
			layout(location = 0) out vec4 pixel;
			in vec2 oTex;
			in vec4 oCol;
			uniform sampler2D sprTex;

			void main()
			{
				pixel = texture(sprTex, oTex) * oCol;
			}
		)");

		shaderDefault.SetVertexShaderSource(R"(
			#version 330 core
			layout(location = 0) in vec4 aPos;
			layout(location = 1) in vec4 aCol;
			layout(location = 2) in vec2 aTex;
			uniform mat4 mvp;
			uniform int drawtype;
			uniform vec4 tint;
			uniform vec2 target;
			uniform vec2 invtarget;
			out vec2 oTex;
			out vec4 oCol;

			void main()
			{ 																																				  
				if(drawtype == 2) // 3D																																  
				{																																			  
					gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0); 																					  
					oTex = aTex;																															  
				} 				 
			
				else if(drawtype == 1) // 2D Line																																		  
				{																																			  
					float p = 1.0 / aPos.z; 																												  
					gl_Position = p * vec4(vec2(2.0 * ((floor(aPos.xy) + 0.5) * invtarget) - 1.0), 0.0, 1.0);	  
					oTex = aTex;																										  
				} 			  
			
				else if(drawtype == 0) // 2D Polygon																																		  
				{																																			  
					float p = 1.0 / aPos.z; 																												  
					gl_Position = p * vec4(vec2(2.0 * ((floor(aPos.xy)) * invtarget) - 1.0), 0.0, 1.0);	  
					oTex = p * vec2(aTex.x, aTex.y);																										  
				} 
				
				else  // Balanced default
				{
					gl_Position = aPos;
					oTex = aTex;
				} 																																			  
																																			  
				oCol = aCol * tint;																															  
			}
		)");

		//shaderDefault.SetVertexShaderSource(
		//	"#version 330 core\n"
		//	"layout(location = 0) in vec4 aPos;\n"
		//	"layout(location = 1) in vec4 aCol;\n"
		//	"layout(location = 2) in vec2 aTex;\n"
		//	"uniform mat4 mvp;\n"
		//	"uniform int drawtype;\n"
		//	"uniform vec4 tint;\n"
		//	"uniform vec2 target;\n"
		//	"uniform vec2 invtarget;\n"
		//	"out vec2 oTex;\n"
		//	"out vec4 oCol;\n"
		//	"void main()\n"
		//	"{\n 																																				  "
		//	"	if(drawtype == 2)\n // 3D\n																																  "
		//	"	{\n																																			  "
		//	"		gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n 																					  "
		//	"		oTex = aTex;\n																															  "
		//	"	}\n 				 "
		//	"\n"
		//	"	else if(drawtype == 1)\n // Line\n																																		  "
		//	"	{\n																																			  "
		//	"		float p = 1.0 / aPos.z;\n 																												  "
		//	"		gl_Position = p * vec4(2.0 * ((floor(aPos.x) + 0.5) * invtarget.x) - 1.0,2.0 * ((floor(aPos.y)+0.5) * invtarget.y) - 1.0, 0.0, 1.0);\n 	  "
		//	"		oTex = p * vec2(aTex.x, aTex.y);\n																										  "
		//	"	}\n 			  "
		//	""
		//	"	else if(drawtype == 0)\n // Quad\n																																		  "
		//	"	{\n																																			  "
		//	"		float p = 1.0 / aPos.z;\n 																												  "
		//	"		gl_Position = p * vec4(2.0 * ((floor(aPos.x)) * invtarget.x) - 1.0,2.0 * ((floor(aPos.y)) * invtarget.y) - 1.0, 0.0, 1.0);\n 	  "
		//	"		oTex = p * vec2(aTex.x, aTex.y);\n																										  "
		//	"	} else {gl_Position = aPos;}\n 																																			  "
		//	"	\n	 "
		//	"	\n																																			  "
		//	"	oCol = aCol * tint;\n																															  "
		//	"}\n"
		//	//"void main(){ if(is3d!=0) {gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0); oTex = aTex;} else {float p = 1.0 / aPos.z; gl_Position = mvp * (p * vec4(aPos.x, aPos.y, 0.0, 1.0)); oTex = p * aTex;} oCol = aCol * tint;}"

		//);

		shaderDefault.Compile();
		shaderDefault.CreateUniform("mvp");
		shaderDefault.CreateUniform("drawtype");
		shaderDefault.CreateUniform("tint");
		shaderDefault.CreateUniform("target");
		shaderDefault.CreateUniform("invtarget");

		// Create "Default" Vertex Buffer / Vertex Attributes. This buffer is reused
		// for all drawing operations. It's possible future versions may allow the
		// creation of additional named buffers for repeated drawing operations with
		// minimal overhead.
		gl.glGenBuffers(1, &nDefaultVB);
		gl.glGenVertexArrays(1, &nDefaultVA);
		gl.glBindVertexArray(nDefaultVA);
		gl.glBindBuffer(0x8892, nDefaultVB);

		// A big one is allocated to reduce shuffles in GPU memory
		GPUTask::Vertex verts[OLC_GPU_MAX_VERTICES];
		gl.glBufferData(0x8892, sizeof(GPUTask::Vertex) * OLC_GPU_MAX_VERTICES, verts, 0x88E0);
		
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
		gl.glBindBuffer(0x8892, 0);
		gl.glBindVertexArray(0);


		


		// Create a null-texture so sampler doesnt fail. We don't have some of the core's helper
		// functions here, so we construct it manually
		imgBlank.Create({ 1,1 });
		imgBlank.SetGPUID(CreateTexture(imgBlank.Size()));
		imgBlank.BindCPU();
		imgBlank.Pixel({ 0,0 }) = olc::Colour::WHITE;
		imgBlank.BindGPU();
		WriteTexture(imgBlank.GetGPUID(), imgBlank);

		// Create a Frame Buffer Object for off-screen rendering things
		gl.glGenFramebuffers(1, (GLuint*)&nDefaultFBO);
		gl.glBindFramebuffer(36160U, nDefaultFBO); // GL_FRAMEBUFFER
		// Attach 4 colour buffers
		std::array<GLenum, 4> attachments = { {36064U, 36065U, 36066U, 36067U} };
		gl.glDrawBuffers(4, attachments.data());
		// Unlink them from any existing image textures
		//gl.glFramebufferTexture2D(36160U, attachments[0], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(36160U, attachments[1], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(36160U, attachments[2], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(36160U, attachments[3], GL_TEXTURE_2D, 0, 0);
		// Unbind the FBO
		gl.glBindFramebuffer(36160U, 0);

		gl.glEnable(GL_TEXTURE_2D); // Turn on texturing
		gl.glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		gl.glEnable(GL_BLEND);

		lastError = RendererError::None;
		return true;
	}

	bool Renderer_OGL33::DestroyDevice()
	{
		auto& gl = olc::apis::opengl::gl::Get();

#if OLC_HOST == OLC_HOST_WINDOWS
		wglDeleteContext(glRenderContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS
        //TODO: Add MacOS destroy context code
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
			auto err = ::GetLastError();
			return false;
		}
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS

		CGLContextObj cglContext = (CGLContextObj)glRenderContext;
		if (!CGLSetCurrentContext(cglContext))
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
        if (!glRenderContext) {
            lastError = RendererError::FailedToCreateRenderContext;
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
		gl.glGenTextures(1, &id);
		gl.glBindTexture(GL_TEXTURE_2D, id);

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

#if OLC_HOST != OLC_HOST_EMSCRIPTEN
#if OLC_HOST != OLC_HOST_MACOS
		gl.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif
#endif

		std::cout << "Created Texture ID: " << id << " Size: " << vSize.x << "x" << vSize.y << "\n";
		return id;
	}

	bool Renderer_OGL33::WriteTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glBindTexture(GL_TEXTURE_2D, image.GetGPUID());
		gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Size().x, image.Size().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
		return true;
	}

	bool Renderer_OGL33::ReadTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glBindTexture(GL_TEXTURE_2D, image.GetGPUID());
		//gl.glReadPixels(0, 0, image.Size().x, image.Size().y, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
		gl.glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
		return true;
	}

	bool Renderer_OGL33::DeleteTexture(const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glDeleteTextures(1, &texid);
		return true;
	}

	bool Renderer_OGL33::AssignTextureSource(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// If the requested source texture is currently attached as the render target,
		// unbind the framebuffer to avoid sampling from a texture that's being written to.
		if (texid == nCurrentTextureTarget && texid != 0)
		{
#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning ATS: Requested source is currently attached as target (" << texid << ") - unbinding FBO\n";
#endif
			gl.glBindFramebuffer(36160U, 0);
			nCurrentTextureTarget = 0;
		}

		//if (nCurrentTextureSource == texid)
		//	return true;

		gl.glActiveTexture(0x84C0 + slot); // GL_TEXTURE0
		gl.glBindTexture(GL_TEXTURE_2D, texid);
		nCurrentTextureSource = texid;
		return true;
	}

	bool Renderer_OGL33::AssignTextureTarget(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();


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
				gl.glActiveTexture(0x84C0 + i);
				gl.glBindTexture(GL_TEXTURE_2D, 0);
			}
			// Reset to texture unit 0
			gl.glActiveTexture(0x84C0);
			nCurrentTextureSource = 0;
		}

		if (texid == 0)
		{
			// Unbind the FBO (bind default framebuffer)
			gl.glBindFramebuffer(36160U, 0);
			return true;
		}	
		
		// Bind FBO
		gl.glBindFramebuffer(36160U, nDefaultFBO);

		//gl.glEnable(GL_BLEND);
		//gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Allocate target buffers - pick the single attachment corresponding to 'slot'
		std::array<GLenum, 8> attachments =
		{ { 36064U, 36065U, 36066U, 36067U, 36068U, 36069U, 36070U, 36071U } };
		GLenum draw = attachments[slot];
		gl.glDrawBuffers(1, &draw);
		// Bind buffers to texture
		gl.glFramebufferTexture2D(36160U, 36064U + slot, GL_TEXTURE_2D, texid, 0);

		//glReadBuffer(36064U + slot);  // GL_COLOR_ATTACHMENT0 + slot
		

		nCurrentTextureTarget = texid;
		
		return true;
	}

	bool Renderer_OGL33::ApplyShader(const Shader& shader)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glUseProgram(shader.GetShaderID());
		return true;
	}

	bool Renderer_OGL33::ApplyDefaultShader()
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glUseProgram(shaderDefault.GetShaderID());


		return true;
	}

	bool Renderer_OGL33::DoGPUTask(const olc::GPUTask& task)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		switch (task.task)
		{
			case GPUTask::Task::DrawPolygon:
			{
				
				gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//gl.glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

				if (task.pImage == nullptr)
					AssignTextureSource(0, imgBlank.GetGPUID());
				else
					AssignTextureSource(0, task.pImage->GetGPUID());

				// Bind generic vertex buffer
				gl.glBindVertexArray(nDefaultVA);
				gl.glBindBuffer(0x8892, nDefaultVB);
				
				// Copy data from CPU to GPU
				gl.glBufferData(0x8892, sizeof(GPUTask::Vertex) * task.vertexBuffer.size(), task.vertexBuffer.data(), 0x88E0);
				
				

				// Shader: Apply MVP Matrix
				//gl.glUniformMatrix4fv(shaderDefault.GetUniform("mvp"), 1, true, task.mvpMatrix.data());

				// Shader: Apply Global Tint
				float f[4] = { 
					float(task.tint.r) / 255.0f, 
					float(task.tint.g) / 255.0f, 
					float(task.tint.b) / 255.0f, 
					float(task.tint.a) / 255.0f 
				};
				gl.glUniform4fv(shaderDefault.GetUniform("tint"), 1, f);

				f[0] = 64.0f;
				f[1] = 64.0f;
				gl.glUniform2fv(shaderDefault.GetUniform("target"), 1, vTargetSize.a().data());
				gl.glUniform2fv(shaderDefault.GetUniform("invtarget"), 1, ((1.0f / vTargetSize)).a().data());

				// Apply Culling modes
				//if (task.cullmode == GPUTask::CullMode::None)
				//{
				//	gl.glCullFace(GL_FRONT);
				//	gl.glDisable(GL_CULL_FACE);
				//}
				//else if (task.cullmode == GPUTask::CullMode::ClockWise)
				//{
				//	gl.glCullFace(GL_FRONT);
				//	gl.glEnable(GL_CULL_FACE);
				//}
				//else if (task.cullmode == GPUTask::CullMode::CounterClockWise)
				//{
				//	gl.glCullFace(GL_BACK);
				//	gl.glEnable(GL_CULL_FACE);
				//}

				//// Apply Depth Testing (if required)
				//if (task.bDepth)
				//	gl.glEnable(GL_DEPTH_TEST);

				gl.glEnable(GL_BLEND);
				//gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				gl.glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				// Draw the thing!
				if (task.bWireframe)
				{
					// Shader: Configure Rendering Mode
					gl.glUniform1i(shaderDefault.GetUniform("drawtype"), 1);
					gl.glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)task.vertexBuffer.size());
				}
				else
				{
					// Shader: Configure Rendering Mode
					gl.glUniform1i(shaderDefault.GetUniform("drawtype"), 0);

					if (task.structure == GPUTask::Structure::Fan)
						gl.glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)task.vertexBuffer.size());
					else if (task.structure == GPUTask::Structure::Strip)
						gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)task.vertexBuffer.size());
					else if (task.structure == GPUTask::Structure::List)
						gl.glDrawArrays(GL_TRIANGLES, 0, (GLsizei)task.vertexBuffer.size());
					else if (task.structure == GPUTask::Structure::Line)
						gl.glDrawArrays(GL_LINES, 0, (GLsizei)task.vertexBuffer.size());
					else if (task.structure == GPUTask::Structure::Point)
						gl.glDrawArrays(GL_POINTS, 0, (GLsizei)task.vertexBuffer.size());
				}

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

	bool Renderer_OGL33::DisplayPrepare()
	{
		auto& gl = olc::apis::opengl::gl::Get();

		//gl.glUseProgram(shaderDefault.GetShaderID());
		gl.glEnable(GL_BLEND);
		gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl.glDepthFunc(GL_LESS);
		//gl.glBindTexture(GL_TEXTURE_2D, imgBlank.GetGPUID());

		return false;
	}

	bool Renderer_OGL33::DisplayDraw(std::vector<void*> os_win_id, bool bVerticalSyncNow)
	{
		auto& gl = olc::apis::opengl::gl::Get();


#if OLC_HOST == OLC_HOST_WINDOWS
		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));
		SwapBuffers(glDeviceContext);
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif	

#if OLC_HOST == OLC_HOST_MACOS
        glFlushRenderAPPLE();
        glSwapAPPLE();
       
#endif

		return true;
	}


}
//! END IMPLEMENTATION