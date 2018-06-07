
#include "Texture.h"

namespace Graphics
{

Texture::Texture(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type) : GraphicsResource()
{
	if (width <= 0 || height <= 0)
	{
		LogError(LogCategory::GRAPHICS, "Texture width and height must both be greater than zero.");
		exit(EXIT_FAILURE);
	}

	_width = width;
	_height = height;
	_internalFormat = internalFormat;
	_format = format;
	_type = type;

	glGenTextures(1, &_handle);
	LogGL("glGenTextures");
	glBindTexture(GL_TEXTURE_2D, _handle);
	LogGL("glBindTexture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	LogGL("glTexParameter");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	LogGL("glTexParameter");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	LogGL("glTexParameter");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	LogGL("glTexParameter");

	// We don't need mip maps

	// https://www.khronos.org/opengl/wiki/Image_Format
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
	// https://gist.github.com/Kos/4739337
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
	LogGL("glTexImage2D");
}

void Texture::SetData(void* data, uint8_t unpackAlignment)
{
	GLuint previousBoundTextureHandle;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&previousBoundTextureHandle));
	LogGL("glGetInteger");

	if (previousBoundTextureHandle != _handle)
	{
		glBindTexture(GL_TEXTURE_2D, _handle);
		LogGL("glBindTexture");
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
	LogGL("glPixelStore");

	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _format, _type, data);
	LogGL("glTexImage2D");

	if (previousBoundTextureHandle != _handle)
	{
		glBindTexture(GL_TEXTURE_2D, previousBoundTextureHandle);
		LogGL("glBindTexture");
	}
}

void* Texture::GetData(uint64_t* length)
{
	length = _length;
	return _data;
}

}
