#pragma once

#include <memory>
#include <vector>
#include "GraphicsDebug.h"
#include "GraphicsResource.h"
#include "../IO/Image.h"

using IO::ImageType;
using IO::Image;

namespace Graphics {

class Texture : public GraphicsResource
{

public:
	explicit Texture(uint32_t width, uint32_t height);

	void UploadImage(const Image* image);

	uint32_t GetWidth()
	{ return _width; }

	uint32_t GetHeight()
	{ return _height; }

private:
	uint32_t _width;
	uint32_t _height;
	GLint _colorComponentsType;
	GLenum _pixelDataFormat;
	GLenum _pixelDataType;

	void GetFormatFromImageType(ImageType imageType, GLint* colorComponentsCount, GLenum* pixelDataFormat, GLenum* pixelDataType);

};
}