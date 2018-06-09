#include "ImageSRGB.h"
#include "../Graphics/Texture.h"

namespace IO {

ImageSRGB::ImageSRGB(size_t width, size_t height, uint8_t* data, uint32_t dataLength) : Image(width, height, data,
                                                                                              dataLength)
{}

GLint ImageSRGB::GetGLInternalFormat()
{
	return GL_SRGB8;
}

GLenum ImageSRGB::GetGLFormat()
{
	return GL_BGRA;
}

}
