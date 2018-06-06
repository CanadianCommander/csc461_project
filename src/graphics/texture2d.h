#pragma once

#include "graphics.h"
#include "graphics_resource.h"

namespace Graphics {

class Texture2D : GraphicsResource
{

public:
	Texture2D(int width, int height, GLint internalFormat, GLenum format, GLenum type, const void* data = nullptr);
	void SetData(void* data, uint8_t unpackAlignment = 4);

private:
	int _width;
	int _height;
	GLint _internalFormat;
	GLenum _format;
	GLenum _type;
};


}
