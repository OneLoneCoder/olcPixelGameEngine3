#pragma once


//! START CUSTOMHEADER
#include "config.h"

//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <iostream>
#include <sstream>
#include <source_location>
//! END STDHEADER


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

#if OLC_HOST == OLC_HOST_LINUX_X11 || OLC_HOST == OLC_HOST_LINUX_WAYLAND
	#include <GL/gl.h>
	#if OLC_HOST == OLC_HOST_LINUX_X11
		namespace X11
		{
			#include <GL/glx.h>
		}
	#endif
#endif

#if OLC_HOST == OLC_HOST_MACOS
		#define GL_SILENCE_DEPRECATION // Stops MacOS & iOS whining about OpenGL is deprecated and we should use Metal
        #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED // Stops MacOS & iOS whining about OpenGL even more
        #define CALLSTYLE
        #define OGL_LOAD(t) &::t
        #define GL_GLEXT_PROTOTYPES
        #define GL_CLAMP GL_CLAMP_TO_EDGE
        #include <stddef.h>                         // Correct issue with Unknown type name 'ptrdiff_t'
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>
        #include <OpenGL/glu.h>
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
	#define GL_GLEXT_PROTOTYPES
	#include <GLES2/gl2ext.h>
	#include <emscripten/emscripten.h>
	#define GL_CLAMP GL_CLAMP_TO_EDGE
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

		

#if OLC_HOST == OLC_HOST_WINDOWS
		typedef void CALLSTYLE glSwapInterval_t(GLsizei n);
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

		private:
			bool CheckError(const std::source_location loc = std::source_location::current());

		};
	}
	


//#if defined(OLC_PLATFORM_X11)
//	typedef int(locSwapInterval_t)(X11::Display* dpy, X11::GLXDrawable drawable, int interval);
//#endif
//
//#if defined(OLC_PLATFORM_EMSCRIPTEN)
//	typedef void CALLSTYLE locShaderSource_t(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
//	typedef EGLBoolean(locSwapInterval_t)(EGLDisplay display, EGLint interval);
//#else
//	typedef void CALLSTYLE locShaderSource_t(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
//#endif

} // olc namespace
//! END DECLARATION





