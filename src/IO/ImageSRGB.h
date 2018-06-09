#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "Image.h"

namespace IO {

class ImageSRGB : public Image
{

public:
	ImageSRGB(size_t width, size_t height, uint8_t* data, uint32_t dataLength);
	GLint GetGLInternalFormat() override;
	GLenum GetGLFormat() override;
};

}
