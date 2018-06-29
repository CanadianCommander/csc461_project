#include "Shader.h"

namespace Graphics {
namespace Shaders {

Shader::Shader(const char* source, GLenum type) : GraphicsResource()
{
	_handle = glCreateShader(type);
	LogGLWithHandle("glCreateShader", _handle);
	glShaderSource(_handle, 1, &source, nullptr);
	LogGLWithHandle("glCompileSource", _handle);
	glCompileShader(_handle);
	LogGLWithHandle("glCompileShader", _handle);

	_type = type;
	auto result = GL_FALSE;
	glGetShaderiv(_handle, GL_COMPILE_STATUS, &result);

	if (!result) {
		GLint log_length = 0;
		glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &log_length);
		auto log = new char[log_length];
		glGetShaderInfoLog(_handle, log_length, nullptr, log);
		LogCritical(LogCategory::GRAPHICS, "glCompileShader: %s. Handle %d.\n%s", toStatus(false), _handle, log);
	} else {
		LogVerbose(LogCategory::GRAPHICS, "glCompileShader: %s. Handle %d.", toStatus(true), _handle);
	}
}

}
}