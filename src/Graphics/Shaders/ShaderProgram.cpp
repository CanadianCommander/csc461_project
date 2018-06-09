#include "ShaderProgram.h"

namespace Graphics {
namespace Shaders {

ShaderProgram::ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) : GraphicsResource()
{
	_vertexShader = new Shader(vertexShaderSource, GL_VERTEX_SHADER);
	_fragmentShader = new Shader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	Create();
	Link();
	Use();
	FindAttributes();
	FindUniforms();
}

void ShaderProgram::Create()
{
	_handle = glCreateProgram();
	LogGLWithHandle("glCompileProgram", _handle);
}

void ShaderProgram::Link()
{
	glAttachShader(_handle, _vertexShader->_handle);
	LogGLWithHandle("glAttachShader", _handle);
	glAttachShader(_handle, _fragmentShader->_handle);
	LogGLWithHandle("glAttachShader", _handle);
	glLinkProgram(_handle);
	GLint result = GL_FALSE;
	glGetProgramiv(_handle, GL_LINK_STATUS, &result);
	if (!result) {
		GLint log_length = 0;
		glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &log_length);
		auto log = new char[log_length];
		glGetProgramInfoLog(_handle, log_length, nullptr, log);
		LogCritical(LogCategory::GRAPHICS, "glLinkProgram: %s. Handle %d.\n%s", toStatus(false), _handle, log);
	} else {
		LogVerbose(LogCategory::GRAPHICS, "glLinkProgram: %s. Handle %d.", toStatus(true), _handle);
	}
}

void ShaderProgram::FindAttributes()
{
	GLint attributes_count;
	glGetProgramiv(_handle, GL_ACTIVE_ATTRIBUTES, &attributes_count);
	LogGLWithHandle("glGetProgramiv", _handle);

	GLuint index;
	GLint size;
	GLint length;
	GLenum type;
	const GLsizei buffer_size = 50; // maximum name length
	GLchar name[buffer_size];

	for (index = 0; index < attributes_count; index++)
	{
		glGetActiveAttrib(_handle, (GLuint)index, buffer_size, &length, &size,
		                  &type, name);
		LogGLWithHandle("glGetActiveAttrib", _handle);
		_attributes[name] = index;
	}
}

void ShaderProgram::FindUniforms()
{
	GLint uniforms_count;
	glGetProgramiv(_handle, GL_ACTIVE_UNIFORMS, &uniforms_count);
	LogGLWithHandle("glGetProgramiv", _handle);

	GLuint index;
	GLint size;
	GLint length;
	GLenum type;
	const GLsizei buffer_size = 50; // maximum name length
	GLchar name[buffer_size];

	for (index = 0; index < uniforms_count; index++)
	{
		glGetActiveUniform(_handle, (GLuint)index, buffer_size, &length, &size,
		                   &type, name);
		LogGLWithHandle("glGetActiveUniform", _handle);
		_uniforms[name] = index;
	}
}

void ShaderProgram::Use()
{
	glUseProgram(_handle);
	LogGLWithHandle("glUseProgram", _handle);
}

}
}
