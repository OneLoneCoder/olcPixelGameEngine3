#pragma once


//! START CUSTOMHEADER
#include "config.h"

//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <iostream>
#include <sstream>
#include <source_location>
//! END STDHEADER

// deliberately outside of single header scope
#if OLC_HOST == OLC_HOST_LINUX_X11
namespace X11 {
	#include <GL/glx.h>
}
using namespace X11;
#endif

//! START OPENGL_CONFIG

#if OLC_HOST == OLC_HOST_WINDOWS
	#include <Windows.h>
	#pragma comment(lib, "gdi32.lib")
	#pragma comment(lib, "opengl32.lib")
	#include <gl/GL.h>
	#define CALLSTYLE __stdcall
	// ooof... was getting a bunch of spurious C4191 from MSVC 17.14.9, so round trip via void-town
	#define OGL_LOAD(t) reinterpret_cast<t##_t*>(reinterpret_cast<void*>(wglGetProcAddress(#t)))
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
	#include <GL/gl.h>
	#define OGL_LOAD(t) reinterpret_cast<t##_t*>(X11::glXGetProcAddress(reinterpret_cast<const GLubyte*>(#t)))
#endif

#if OLC_HOST == OLC_HOST_LINUX_WAYLAND
	#include <EGL/egl.h>
	#include <GL/gl.h>

	#define OGL_LOAD(t) reinterpret_cast<t##_t*>(eglGetProcAddress(#t))
#endif

#if OLC_HOST == OLC_HOST_MACOS
		#define GL_SILENCE_DEPRECATION // Stops MacOS & iOS whining about OpenGL is deprecated and we should use Metal
        #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED // Stops MacOS & iOS whining about OpenGL even more
        #define CALLSTYLE
        #define OGL_LOAD(t) &::t
        #define GL_GLEXT_PROTOTYPES
        #undef GL_CLAMP
		#define GL_CLAMP GL_CLAMP_TO_EDGE
        #include <stddef.h>                         // Correct issue with Unknown type name 'ptrdiff_t'
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>
        #include <OpenGL/glu.h>
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
	#include <GL/gl.h>
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
	#define GL_GLEXT_PROTOTYPES
	#include <GLES3/gl2ext.h>
	#include <emscripten/emscripten.h>
	#define CALLSTYLE
	#undef GL_CLAMP
	#define GL_CLAMP GL_CLAMP_TO_EDGE

	#define OGL_LOAD(t) ::t
#endif

#if OLC_HOST == OLC_HOST_ANDROID
    #include <EGL/egl.h>
    #include <GLES3/gl3.h>
    #define GL_GLEXT_PROTOTYPES
    #include <GLES3/gl3ext.h>
    #define CALLSTYLE
    #undef GL_CLAMP
    #define GL_CLAMP GL_CLAMP_TO_EDGE
    #define GL_LINE 0
    #define GL_FILL 0
    #define OGL_LOAD(t) reinterpret_cast<t##_t*>(eglGetProcAddress(#t))
#endif

#if !defined(CALLSTYLE)
	#define CALLSTYLE
#endif


//! END OPENGL_CONFIG

//! START DECLARATION
namespace olc
{
	namespace apis::opengl
	{

		typedef char GLchar;
    	typedef ptrdiff_t GLsizeiptr;
    
#if OLC_HOST == OLC_HOST_WINDOWS
        // "Target Host Window Handle"
        typedef HDC glDeviceContext_t;
        // "State of OpenGL Machinary"
        typedef HGLRC glRenderContext_t;
        // Johnnyg63: moved glShaderSource_t definition for Windows, as it does not match macOS and android signatures
        typedef void CALLSTYLE glShaderSource_t(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
#endif

#if OLC_HOST == OLC_HOST_MACOS
        typedef void* glDeviceContext_t;
        typedef CGLContextObj glRenderContext_t;
        typedef void CALLSTYLE glShaderSource_t(GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length);
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
        typedef void CALLSTYLE glShaderSource_t(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
		typedef X11::GLXContext glDeviceContext_t;
		typedef X11::GLXContext glRenderContext_t;
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN || OLC_HOST == OLC_HOST_LINUX_WAYLAND || OLC_HOST == OLC_HOST_ANDROID
	typedef void CALLSTYLE glShaderSource_t(GLuint shader, GLsizei size, const GLchar *const * string, const GLint * length);
	typedef void glDeviceContext_t;
	typedef struct
	{
		EGLDisplay display;
		EGLContext context;
		EGLSurface surface;
		EGLConfig config;
	} glRenderContext_t;
#endif

		typedef GLuint CALLSTYLE glCreateShader_t(GLenum type);
		typedef GLuint CALLSTYLE glCreateProgram_t(void);
		
		typedef void CALLSTYLE glDeleteShader_t(GLuint shader);
		typedef void CALLSTYLE glCompileShader_t(GLuint shader);
		typedef void CALLSTYLE glLinkProgram_t(GLuint program);
		typedef void CALLSTYLE glDeleteProgram_t(GLuint program);
		typedef void CALLSTYLE glAttachShader_t(GLuint program, GLuint shader);
		typedef void CALLSTYLE glBindBuffer_t(GLenum target, GLuint buffer);
		typedef void CALLSTYLE glBufferData_t(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
		typedef void CALLSTYLE glGenBuffers_t(GLsizei n, GLuint* buffers);
		typedef void CALLSTYLE glVertexAttribPointer_t(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
		typedef void CALLSTYLE glEnableVertexAttribArray_t(GLuint index);
		typedef void CALLSTYLE glUseProgram_t(GLuint program);
		typedef void CALLSTYLE glBindVertexArray_t(GLuint array);
		typedef void CALLSTYLE glGenVertexArrays_t(GLsizei n, GLuint* arrays);
		typedef void CALLSTYLE glGetShaderInfoLog_t(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
		typedef GLint CALLSTYLE glGetUniformLocation_t(GLuint program, const GLchar* name);
		typedef void CALLSTYLE glUniform1f_t(GLint location, GLfloat v0);
		typedef void CALLSTYLE glUniform1i_t(GLint location, GLint v0);
		typedef void CALLSTYLE glUniform2fv_t(GLint location, GLsizei count, const GLfloat* value);
		typedef void CALLSTYLE glUniform4fv_t(GLint location, GLsizei count, const GLfloat* value);
		typedef void CALLSTYLE glUniformMatrix4fv_t(GLint location, GLsizei count, GLboolean trasnpose, const GLfloat* value);
		typedef void CALLSTYLE glActiveTexture_t(GLenum texture);
		typedef void CALLSTYLE glGenFramebuffers_t(GLsizei n, GLuint* ids);
		typedef void CALLSTYLE glBindFramebuffer_t(GLenum target, GLuint fb);
		typedef GLenum CALLSTYLE glCheckFramebufferStatus_t(GLenum target);
		typedef void CALLSTYLE glDeleteFramebuffers_t(GLsizei n, const GLuint* fbs);
		typedef void CALLSTYLE glFramebufferTexture2D_t(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		typedef void CALLSTYLE glDrawBuffers_t(GLsizei n, const GLenum* bufs);
		typedef void CALLSTYLE glBlendFuncSeparate_t(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
		typedef void CALLSTYLE glTexImage2DMultisample_t(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
		typedef void CALLSTYLE glBlitFramebuffer_t(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
		typedef void CALLSTYLE glGenRenderbuffers_t(GLsizei n, GLuint* renderbuffers);
		typedef void CALLSTYLE glBindRenderbuffer_t(GLenum target, GLuint renderbuffer);
		typedef void CALLSTYLE glRenderbufferStorageMultisample_t(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
		typedef void CALLSTYLE glFramebufferRenderbuffer_t(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
		typedef void CALLSTYLE glDeleteRenderbuffers_t(GLsizei n, const GLuint* renderbuffers);
		typedef void CALLSTYLE glGetInternalformativ_t(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params);
		typedef void CALLSTYLE glGetShaderiv_t(GLuint shader, GLenum pname, GLint* params);
		typedef void CALLSTYLE glGetIntegerv_t(GLenum pname, GLint *data);
		typedef void CALLSTYLE glGetRenderbufferParameteriv_t(GLenum target, GLenum pname, GLint* params);
		typedef void CALLSTYLE glRenderbufferStorage_t(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

#if OLC_HOST == OLC_HOST_WINDOWS
		typedef void CALLSTYLE wglSwapIntervalEXT_t(GLsizei n);
#endif

		// A little GL class (singleton)
		class gl
		{
		public:
			static gl& Get();
			inline bool HasLoaded() const { return bLoaded; }

		private:
			gl() = default;
			static bool bLoaded;

		private:
			bool LoadAll();

		protected:
			glCreateShader_t* _glCreateShader = nullptr;
			glCreateProgram_t* _glCreateProgram = nullptr;
			glShaderSource_t* _glShaderSource = nullptr;
			glDeleteShader_t* _glDeleteShader = nullptr;
			glCompileShader_t* _glCompileShader = nullptr;
			glLinkProgram_t* _glLinkProgram = nullptr;
			glDeleteProgram_t* _glDeleteProgram = nullptr;
			glAttachShader_t* _glAttachShader = nullptr;
			glBindBuffer_t* _glBindBuffer = nullptr;
			glBufferData_t* _glBufferData = nullptr;
			glGenBuffers_t* _glGenBuffers = nullptr;
			glVertexAttribPointer_t* _glVertexAttribPointer = nullptr;
			glEnableVertexAttribArray_t* _glEnableVertexAttribArray = nullptr;
			glUseProgram_t* _glUseProgram = nullptr;
			glBindVertexArray_t* _glBindVertexArray = nullptr;
			glGenVertexArrays_t* _glGenVertexArrays = nullptr;
			glGetShaderInfoLog_t* _glGetShaderInfoLog = nullptr;
			glGetUniformLocation_t* _glGetUniformLocation = nullptr;
			glUniform1f_t* _glUniform1f = nullptr;
			glUniform1i_t* _glUniform1i = nullptr;
			glUniform2fv_t* _glUniform2fv = nullptr;
			glUniform4fv_t* _glUniform4fv = nullptr;
			glUniformMatrix4fv_t* _glUniformMatrix4fv = nullptr;
			glActiveTexture_t* _glActiveTexture = nullptr;
			glGenFramebuffers_t* _glGenFramebuffers = nullptr;
			glBindFramebuffer_t* _glBindFramebuffer = nullptr;
			glCheckFramebufferStatus_t* _glCheckFramebufferStatus = nullptr;
			glDeleteFramebuffers_t* _glDeleteFramebuffers = nullptr;
			glFramebufferTexture2D_t* _glFramebufferTexture2D = nullptr;
			glDrawBuffers_t* _glDrawBuffers = nullptr;
			glBlendFuncSeparate_t* _glBlendFuncSeparate = nullptr;
			glTexImage2DMultisample_t* _glTexImage2DMultisample = nullptr;
			glBlitFramebuffer_t* _glBlitFramebuffer = nullptr;
			glGenRenderbuffers_t* _glGenRenderbuffers = nullptr;
			glBindRenderbuffer_t* _glBindRenderbuffer = nullptr;
			glRenderbufferStorageMultisample_t* _glRenderbufferStorageMultisample = nullptr;
			glFramebufferRenderbuffer_t* _glFramebufferRenderbuffer = nullptr;
			glDeleteRenderbuffers_t* _glDeleteRenderbuffers = nullptr;
			glGetInternalformativ_t* _glGetInternalformativ = nullptr;
			glGetShaderiv_t* _glGetShaderiv = nullptr;
			glGetIntegerv_t *_glGetIntegerv = nullptr;
			glGetRenderbufferParameteriv_t* _glGetRenderbufferParameteriv = nullptr;
			glRenderbufferStorage_t* _glRenderbufferStorage = nullptr;
#if OLC_HOST == OLC_HOST_WINDOWS
			wglSwapIntervalEXT_t* _wglSwapIntervalEXT = nullptr;
#endif


		public:
			// Proxies allow switchable, clutter-free error checking

			// OpenGL3.3 Proxies

			GLuint glCreateShader(GLenum type);
			GLuint glCreateProgram(void);
			void glShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
			void glDeleteShader(GLuint shader);
			void glCompileShader(GLuint shader);
			void glLinkProgram(GLuint program);
			void glDeleteProgram(GLuint program);
			void glAttachShader(GLuint program, GLuint shader);
			void glBindBuffer(GLenum target, GLuint buffer);
			void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
			void glGenBuffers(GLsizei n, GLuint* buffers);
			void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
			void glEnableVertexAttribArray(GLuint index);
			void glUseProgram(GLuint program);
			void glBindVertexArray(GLuint array);
			void glGenVertexArrays(GLsizei n, GLuint* arrays);
			void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
			GLint glGetUniformLocation(GLuint program, const GLchar* name);
			void glUniform1f(GLint location, GLfloat v0);
			void glUniform1i(GLint location, GLint v0);
			void glUniform2fv(GLint location, GLsizei count, const GLfloat* value);
			void glUniform4fv(GLint location, GLsizei count, const GLfloat* value);
			void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean trasnpose, const GLfloat* value);
			void glActiveTexture(GLenum texture);
			void glGenFramebuffers(GLsizei n, GLuint* ids);
			void glBindFramebuffer(GLenum target, GLuint fb);
			GLenum glCheckFramebufferStatus(GLenum target);
			void glDeleteFramebuffers(GLsizei n, const GLuint* fbs);
			void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
			void glDrawBuffers(GLsizei n, const GLenum* bufs);
			void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
			void glTexImage2DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
			void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
			void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);
			void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
			void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
			void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
			void glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
			void glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params);
			void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
			void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);
			void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

			// OpenGL1.2 Proxies (just keeps things tidy imo)
			void glGenTextures(GLsizei n, GLuint* textures);
			void glBindTexture(GLenum target, GLuint texture);
			void glTexParameteri(GLenum target, GLenum pname, GLint param);
			void glTexEnvf(GLenum target, GLenum pname, GLfloat param);
			void glDeleteTextures(GLsizei n, const GLuint* textures); 
			void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
			void glClear(GLbitfield mask);
			void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
			void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
			void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* data);
			void glCullFace(GLenum mode);
			void glEnable(GLenum cap);
		    void glDisable(GLenum cap);
			void glDrawArrays(GLenum mode, GLint first,	GLsizei count);
			void glBlendFunc(GLenum sfactor, GLenum dfactor);
			void glDepthFunc(GLenum func);
			void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);
			void glHint(GLenum target, GLenum mode);
			void glPolygonMode(GLenum face, GLenum mode);
			void glFrontFace(GLenum mode);
			void glGetIntegerv(GLenum pname, GLint *data);


			void glSwapInterval(GLsizei n);

			// Constants
			static constexpr GLenum GL_FRAMEBUFFER_COMPLETE_X = 0x8CD5;
			static constexpr GLenum GL_TEXTURE_2D_MULTISAMPLE_X = 0x9100;
			static constexpr GLenum GL_COLOR_ATTACHMENT0_X = 0x8CE0;
			static constexpr GLenum GL_TEXTURE0_X = 0x84C0;
			static constexpr GLenum GL_READ_FRAMEBUFFER_X = 0x8CA8;
			static constexpr GLenum GL_DRAW_FRAMEBUFFER_X = 0x8ca9;
			static constexpr GLenum GL_FRAMEBUFFER_X = 0x8D40;
			static constexpr GLenum GL_ARRAY_BUFFER_X = 0x8892;
			static constexpr GLenum GL_STREAM_DRAW_X = 0x88E0;
			static constexpr GLenum	GL_DRAW_FRAMEBUFFER_BINDING_X = 0x8CA6;
			static constexpr GLenum GL_FRAGMENT_SHADER_X = 0x8B30;
			static constexpr GLenum GL_VERTEX_SHADER_X = 0x8B31;
			static constexpr GLenum GL_GEOMETRY_SHADER_X = 0x8DD9;
			static constexpr GLenum GL_MULTISAMPLE_X = 0x809D;
			static constexpr GLenum GL_RENDERBUFFER_X = 0x8D41;
			static constexpr GLenum GL_SAMPLES_X = 0x80A9;
			static constexpr GLenum GL_COMPILE_STATUS_X = 0x8B81;
			static constexpr GLenum GL_INFO_LOG_LENGTH_X = 0x8B84;
			static constexpr GLenum GL_DEPTH_COMPONENT24_X = 0x81A6;
			static constexpr GLenum GL_DEPTH_ATTACHMENT_X = 0x8D00;
			static constexpr GLenum GL_RENDERBUFFER_SAMPLES_X = 0x8CAB;
		private:
			bool CheckError(const std::source_location loc = std::source_location::current());

		};
	}
	
} // olc namespace
//! END DECLARATION
