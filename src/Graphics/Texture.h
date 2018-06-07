#pragma once

#include "Graphics.h"
#include "GraphicsResource.h"

namespace Graphics {

class Texture : GraphicsResource
{

public:
	Texture(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type);
	void* GetData(uint64_t* length);
	void SetData(void* data, uint8_t unpackAlignment = 4);
	uint32_t GetWidth() { return _width; }
	uint32_t GetHeight() { return _height; }

private:
	uint32_t _width;
	uint32_t _height;
	GLint _internalFormat;
	GLenum _format;
	GLenum _type;
	uint64_t* _length;
	void* _data;
};


}
