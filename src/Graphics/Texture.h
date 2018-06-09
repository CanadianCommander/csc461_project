#pragma once

#include <memory>
#include <vector>
#include "GraphicsDebug.h"
#include "GraphicsResource.h"
#include "../IO/Image.h"

using IO::Image;

namespace Graphics {

class Texture : GraphicsResource
{

public:
	Texture(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type);
	explicit Texture(IO::Image &image);
	Texture(const Texture &other);

	Texture &operator=(const Texture &other);

	void UploadData(void* data, uint8_t unpackAlignment = 4);
	void Bind();

	uint32_t GetWidth()
	{ return _width; }

	uint32_t GetHeight()
	{ return _height; }

private:
	uint32_t _width;
	uint32_t _height;
	GLint _internalFormat;
	GLenum _format;
	GLenum _type;

};
}
