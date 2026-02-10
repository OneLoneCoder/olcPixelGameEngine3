#include "api_opengl.h"


//! START IMPLEMENTATION
namespace olc::apis::opengl
{
	bool gl::bLoaded = false;

	gl& gl::Get()
	{
		static olc::apis::opengl::gl me;
		if (!me.bLoaded) me.LoadAll();
		return me;
	}

	bool gl::LoadAll()
	{
		// Load all the OpenGL API entry points
		bLoaded = true;
		bLoaded &= (_glCreateShader = OGL_LOAD(glCreateShader)) != nullptr;
		bLoaded &= (_glCreateProgram	= OGL_LOAD(glCreateProgram)) != nullptr;
		bLoaded &= (_glShaderSource = OGL_LOAD(glShaderSource)) != nullptr;
		bLoaded &= (_glDeleteShader = OGL_LOAD(glDeleteShader)) != nullptr;
		bLoaded &= (_glCompileShader = OGL_LOAD(glCompileShader)) != nullptr;
		bLoaded &= (_glLinkProgram = OGL_LOAD(glLinkProgram)) != nullptr;
		bLoaded &= (_glDeleteProgram = OGL_LOAD(glDeleteProgram)) != nullptr;
		bLoaded &= (_glAttachShader = OGL_LOAD(glAttachShader)) != nullptr;
		bLoaded &= (_glBindBuffer = OGL_LOAD(glBindBuffer)) != nullptr;
		bLoaded &= (_glBufferData = OGL_LOAD(glBufferData)) != nullptr;
		bLoaded &= (_glGenBuffers = OGL_LOAD(glGenBuffers)) != nullptr;
		bLoaded &= (_glVertexAttribPointer = OGL_LOAD(glVertexAttribPointer)) != nullptr;
		bLoaded &= (_glEnableVertexAttribArray = OGL_LOAD(glEnableVertexAttribArray)) != nullptr;
		bLoaded &= (_glUseProgram = OGL_LOAD(glUseProgram)) != nullptr;
#if OLC_HOST == OLC_HOST_EMSCRIPTEN
        bLoaded &= (_glBindVertexArray = OGL_LOAD(glBindVertexArrayOES)) != nullptr;
		bLoaded &= (_glGenVertexArrays = OGL_LOAD(glGenVertexArraysOES)) != nullptr;
		bLoaded &= (_glDrawBuffers = OGL_LOAD(glDrawBuffersEXT)) != nullptr;
		// bLoaded &= (_glTexImage2DMultisample = OGL_LOAD(glTexImage2DMultisample)) != nullptr;
		bLoaded &= (_glBlitFramebuffer = OGL_LOAD(glBlitFramebuffer)) != nullptr;
#elif OLC_HOST == OLC_HOST_ANDROID
        bLoaded &= (_glBindVertexArray = OGL_LOAD(glBindVertexArray)) != nullptr;
        bLoaded &= (_glGenVertexArrays = OGL_LOAD(glGenVertexArrays)) != nullptr;
        bLoaded &= (_glDrawBuffers = OGL_LOAD(glDrawBuffers)) != nullptr;
        // bLoaded &= (_glTexImage2DMultisample = OGL_LOAD(glTexImage2DMultisample)) != nullptr;
        bLoaded &= (_glBlitFramebuffer = OGL_LOAD(glBlitFramebuffer)) != nullptr;
#else
        bLoaded &= (_glBindVertexArray = OGL_LOAD(glBindVertexArray)) != nullptr;
		bLoaded &= (_glGenVertexArrays = OGL_LOAD(glGenVertexArrays)) != nullptr;
		bLoaded &= (_glDrawBuffers = OGL_LOAD(glDrawBuffers)) != nullptr;
		bLoaded &= (_glTexImage2DMultisample = OGL_LOAD(glTexImage2DMultisample)) != nullptr;
		bLoaded &= (_glBlitFramebuffer = OGL_LOAD(glBlitFramebuffer)) != nullptr;
#endif
		bLoaded &= (_glGetShaderInfoLog = OGL_LOAD(glGetShaderInfoLog)) != nullptr;
		bLoaded &= (_glGetUniformLocation = OGL_LOAD(glGetUniformLocation)) != nullptr;
		bLoaded &= (_glUniform1f = OGL_LOAD(glUniform1f)) != nullptr;
		bLoaded &= (_glUniform1i = OGL_LOAD(glUniform1i)) != nullptr;
		bLoaded &= (_glUniform2fv = OGL_LOAD(glUniform2fv)) != nullptr;
		bLoaded &= (_glUniform4fv = OGL_LOAD(glUniform4fv)) != nullptr;
		bLoaded &= (_glUniformMatrix4fv = OGL_LOAD(glUniformMatrix4fv)) != nullptr;
		bLoaded &= (_glActiveTexture = OGL_LOAD(glActiveTexture)) != nullptr;
		bLoaded &= (_glGenFramebuffers = OGL_LOAD(glGenFramebuffers)) != nullptr;
		bLoaded &= (_glBindFramebuffer = OGL_LOAD(glBindFramebuffer)) != nullptr;
		bLoaded &= (_glCheckFramebufferStatus = OGL_LOAD(glCheckFramebufferStatus)) != nullptr;
		bLoaded &= (_glDeleteFramebuffers = OGL_LOAD(glDeleteFramebuffers)) != nullptr;
		bLoaded &= (_glFramebufferTexture2D = OGL_LOAD(glFramebufferTexture2D)) != nullptr;
		bLoaded &= (_glBlendFuncSeparate = OGL_LOAD(glBlendFuncSeparate)) != nullptr;
		bLoaded &= (_glGenRenderbuffers = OGL_LOAD(glGenRenderbuffers)) != nullptr;
		bLoaded &= (_glBindRenderbuffer = OGL_LOAD(glBindRenderbuffer)) != nullptr;
		bLoaded &= (_glRenderbufferStorageMultisample = OGL_LOAD(glRenderbufferStorageMultisample)) != nullptr;
		bLoaded &= (_glFramebufferRenderbuffer = OGL_LOAD(glFramebufferRenderbuffer)) != nullptr;
		bLoaded &= (_glDeleteRenderbuffers = OGL_LOAD(glDeleteRenderbuffers)) != nullptr;
		bLoaded &= (_glGetInternalformativ = OGL_LOAD(glGetInternalformativ)) != nullptr;
		bLoaded &= (_glGetShaderiv = OGL_LOAD(glGetShaderiv)) != nullptr;
		bLoaded &= (_glGetRenderbufferParameteriv = OGL_LOAD(glGetRenderbufferParameteriv)) != nullptr;
		bLoaded &= (_glRenderbufferStorage = OGL_LOAD(glRenderbufferStorage)) != nullptr;

		// Do we really need to do this? - jx9
#if OLC_HOST != OLC_HOST_WINDOWS
		bLoaded &= (_glGetIntegerv = OGL_LOAD(glGetIntegerv)) != nullptr;
#else
		_glGetIntegerv = ::glGetIntegerv;
#endif

#if OLC_HOST == OLC_HOST_WINDOWS
		bLoaded &= (_wglSwapIntervalEXT = OGL_LOAD(wglSwapIntervalEXT)) != nullptr;
#endif

		
		return bLoaded;
	}

	bool gl::CheckError([[maybe_unused]] const std::source_location  loc)
	{
#if OLC_GPU_ERRORCHECK == 1
		GLenum err;
		bool bWasError = false;		
		std::stringstream sLocation;
		sLocation
			<< loc.file_name() << '('
			<< loc.line() << ':'
			<< loc.column() << ") `"
			<< loc.function_name() << "`: ";
		while ((err = ::glGetError()) != GL_NO_ERROR)
		{
			switch (err)
			{
			case GL_INVALID_ENUM:
				sLocation << "OGL33 Error: GL_INVALID_ENUM\n"; break;
			case GL_INVALID_VALUE:
				sLocation << "OGL33 Error: GL_INVALID_VALUE\n"; break;
			case GL_INVALID_OPERATION:
				sLocation << "OGL33 Error: GL_INVALID_OPERATION\n"; break;
			case 0x0506: //GL_INVALID_FRAMEBUFFER_OPERATION:
				sLocation << "OGL33 Error: GL_INVALID_FRAMEBUFFER_OPERATION\n"; break;
			case GL_OUT_OF_MEMORY:
				sLocation << "OGL33 Error: GL_OUT_OF_MEMORY\n"; break;
#if OLC_HOST != OLC_HOST_ANDROID
			case GL_STACK_UNDERFLOW:
				sLocation << "OGL33 Error: GL_STACK_UNDERFLOW\n"; break;
			case GL_STACK_OVERFLOW:
				sLocation << "OGL33 Error: GL_STACK_OVERFLOW\n"; break;
#endif
			}
			bWasError = true;

			std::cout << sLocation.str() << "\n";
			sLocation.clear();
		}
		return bWasError;
#else
		return false;
#endif
	}

	void gl::glGenTextures(GLsizei n, GLuint* textures)
	{
		::glGenTextures(n, textures);
		CheckError();
	}

	void gl::glBindTexture(GLenum target, GLuint texture)
	{
		::glBindTexture(target, texture);
		CheckError();
	}

	void gl::glTexParameteri(GLenum target, GLenum pname, GLint param)
	{
		::glTexParameteri(target, pname, param);
		CheckError();
	}

	void gl::glTexEnvf(GLenum target, GLenum pname, GLfloat param)
	{
#if OLC_HOST != OLC_HOST_ANDROID
		::glTexEnvf(target, pname, param);
		CheckError();
#endif
	}

	void gl::glDeleteTextures(GLsizei n, const GLuint* textures)
	{
		::glDeleteTextures(n, textures);
		CheckError();
	}

	void gl::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
	{
		::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
		CheckError();
	}

	void gl::glClear(GLbitfield mask)
	{
		::glClear(mask);
		CheckError();
	}

	void gl::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		::glViewport(x, y, width, height);
		CheckError();
	}

	void gl::glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		::glClearColor(red, green, blue, alpha);
		CheckError();
	}

	void gl::glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* data)
	{
		::glReadPixels(x, y, width, height, format, type, data);
		CheckError();
	}

	void gl::glCullFace(GLenum mode)
	{
		::glCullFace(mode);
		CheckError();
	}

	void gl::glEnable(GLenum cap)
	{
		::glEnable(cap);
		CheckError();
	}

	void gl::glDisable(GLenum cap)
	{
		::glDisable(cap);
		CheckError();
	}

	void gl::glDrawArrays(GLenum mode, GLint first, GLsizei count)
	{
		::glDrawArrays(mode, first, count);
		CheckError();
	}

	void gl::glBlendFunc(GLenum sfactor, GLenum dfactor)
	{
		::glBlendFunc(sfactor, dfactor);
		CheckError();
	}

	void gl::glDepthFunc(GLenum func)
	{
		::glDepthFunc(func);
		CheckError();
	}

	void gl::glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels)
	{
#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
		::glGetTexImage(target, level, format, type, pixels);
		CheckError();
#endif
	}

	void gl::glHint(GLenum target, GLenum mode)
	{
		::glHint(target, mode);
		CheckError();
	}

	void gl::glPolygonMode(GLenum face, GLenum mode)
	{
#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
		::glPolygonMode(face, mode);
		CheckError();
#endif
	}

	void gl::glFrontFace(GLenum mode)
	{
		::glFrontFace(mode);
		CheckError();
	}

	void gl::glSwapInterval(GLsizei n)
	{
#if OLC_HOST == OLC_HOST_WINDOWS
		_wglSwapIntervalEXT(n);
#endif
	}

	GLuint gl::glCreateShader(GLenum type)
	{
		return _glCreateShader(type);
	}

	GLuint gl::glCreateProgram(void)
	{
		return _glCreateProgram();
	}

	void gl::glShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
	{
		_glShaderSource(shader, count, string, length);
		CheckError();
	}

	void gl::glDeleteShader(GLuint shader)
	{
		_glDeleteShader(shader);
		CheckError();
	}

	void gl::glCompileShader(GLuint shader)
	{
		_glCompileShader(shader);
		CheckError();
	}

	void gl::glLinkProgram(GLuint program)
	{
		_glLinkProgram(program);
		CheckError();
	}

	void gl::glDeleteProgram(GLuint program)
	{
		_glDeleteProgram(program);
		CheckError();
	}

	void gl::glAttachShader(GLuint program, GLuint shader)
	{
		_glAttachShader(program, shader);
		CheckError();
	}

	void gl::glBindBuffer(GLenum target, GLuint buffer)
	{
		_glBindBuffer(target, buffer);
		CheckError();
	}

	void gl::glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
	{
		_glBufferData(target, size, data, usage);
		CheckError();
	}

	void gl::glGenBuffers(GLsizei n, GLuint* buffers)
	{
		_glGenBuffers(n, buffers);
		CheckError();
	}

	void gl::glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
	{
		_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
		CheckError();
	}

	void gl::glEnableVertexAttribArray(GLuint index)
	{
		_glEnableVertexAttribArray(index);
		CheckError();
	}

	void gl::glUseProgram(GLuint program)
	{
		_glUseProgram(program);
		CheckError();
	}

	void gl::glBindVertexArray(GLuint array)
	{
		_glBindVertexArray(array);
		CheckError();
	}

	void gl::glGenVertexArrays(GLsizei n, GLuint* arrays)
	{
		_glGenVertexArrays(n, arrays);
		CheckError();
	}

	void gl::glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
	{
		_glGetShaderInfoLog(shader, bufSize, length, infoLog);
		CheckError();
	}

	GLint gl::glGetUniformLocation(GLuint program, const GLchar* name)
	{
		return _glGetUniformLocation(program, name);
	}

	void gl::glUniform1f(GLint location, GLfloat v0)
	{
		_glUniform1f(location, v0);
		CheckError();
	}

	void gl::glUniform1i(GLint location, GLint v0)
	{
		_glUniform1i(location, v0);
		CheckError();
	}

	void gl::glUniform2fv(GLint location, GLsizei count, const GLfloat* value)
	{
		_glUniform2fv(location, count, value);
		CheckError();
	}

	void gl::glUniform4fv(GLint location, GLsizei count, const GLfloat* value)
	{
		_glUniform4fv(location, count, value);
		CheckError();
	}

	void gl::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean trasnpose, const GLfloat* value)
	{
		_glUniformMatrix4fv(location, count, trasnpose, value);
		CheckError();
	}

	void gl::glActiveTexture(GLenum texture)
	{
		_glActiveTexture(texture);
		CheckError();
	}

	void gl::glGenFramebuffers(GLsizei n, GLuint* ids)
	{
		_glGenFramebuffers(n, ids);
		CheckError();
	}

	void gl::glBindFramebuffer(GLenum target, GLuint fb)
	{
		_glBindFramebuffer(target, fb);
		CheckError();
	}

	GLenum gl::glCheckFramebufferStatus(GLenum target)
	{
		return _glCheckFramebufferStatus(target);
	}

	void gl::glDeleteFramebuffers(GLsizei n, const GLuint* fbs)
	{
		_glDeleteFramebuffers(n, fbs);
		CheckError();
	}

	void gl::glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{
		_glFramebufferTexture2D(target, attachment, textarget, texture, level);
		CheckError();
	}

	void gl::glDrawBuffers(GLsizei n, const GLenum* bufs)
	{
		_glDrawBuffers(n, bufs);
		CheckError();
	}

	void gl::glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		_glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
		CheckError();
	}

	void gl::glTexImage2DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
	{
#if OLC_HOST != OLC_HOST_EMSCRIPTEN && OLC_HOST != OLC_HOST_ANDROID
		_glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
		CheckError();
#endif
	}

	void gl::glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
	{
		_glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
		CheckError();
	}

	void gl::glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
	{
		_glGenRenderbuffers(n, renderbuffers);
		CheckError();
	}

	void gl::glBindRenderbuffer(GLenum target, GLuint renderbuffer)
	{
		_glBindRenderbuffer(target, renderbuffer);
		CheckError();
	}

	void gl::glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
	{
		_glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
		CheckError();
	}

	void gl::glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
	{
		_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
		CheckError();
	}

	void gl::glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
	{
		_glDeleteRenderbuffers(n, renderbuffers);
		CheckError();
	}

	void gl::glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params)
	{
		_glGetInternalformativ(target, internalformat, pname, bufSize, params);
		CheckError();
	}

	void gl::glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
	{
		_glGetShaderiv(shader, pname, params);
		CheckError();
	}

	void gl::glGetIntegerv(GLenum pname, GLint *data)
	{
		_glGetIntegerv(pname, data);
		CheckError();
	}

	void gl::glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
	{
		_glGetRenderbufferParameteriv(target, pname, params);
		CheckError();
	}

	void gl::glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
	{
		_glRenderbufferStorage(target, internalformat, width, height);
		CheckError();
	}
}
//! END IMPLEMENTATION