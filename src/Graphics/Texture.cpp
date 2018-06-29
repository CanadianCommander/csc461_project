#include "Texture.h"

namespace Graphics {

Texture::Texture(uint32_t width, uint32_t height) : GraphicsResource()
{
	if (width <= 0 || height <= 0)
	{
		LogCritical(LogCategory::GRAPHICS, "Texture width and height must both be greater than zero.");
	}

	_width = width;
	_height = height;
}

void Texture::UploadImage(const Image* image)
{
	GLint colorComponentsType;
	GLenum pixelDataFormat;
	GLenum pixelDataType;
	GetFormatFromImageType(image->GetType(), &colorComponentsType, &pixelDataFormat, &pixelDataType);

	if (_handle == 0 || colorComponentsType != _colorComponentsType || pixelDataFormat != _pixelDataFormat || pixelDataType != _pixelDataType)
	{
		_colorComponentsType = colorComponentsType;
		_pixelDataFormat = pixelDataFormat;
		_pixelDataType = pixelDataType;

		if (_handle != 0)
		{
			glDeleteTextures(1, &_handle);
			LogGL("glDeleteTextures");
		}

		glGenTextures(1, &_handle);
		LogGL("glGenTextures");
		glBindTexture(GL_TEXTURE_2D, _handle);
		LogGL("glBindTexture");

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		LogsGL("glTexParameter");
	}

	//TODO: This might need to be considered.
	int unpackAlignment = 4;
	glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
	LogGL("glPixelStore");

	glTexImage2D(GL_TEXTURE_2D, 0, colorComponentsType, _width, _height, 0, pixelDataFormat, pixelDataType, image->GetData());
	LogGL("glTexImage2D");
}

void Texture::GetFormatFromImageType(ImageType imageType, GLint* colorComponentsType, GLenum* pixelDataFormat, GLenum* pixelDataType)
{
	switch (imageType)
	{
		case ImageType::RGB:
			*colorComponentsType = GL_RGB;
			*pixelDataFormat = GL_RGB;
			*pixelDataType = GL_UNSIGNED_BYTE;
			return;
		case ImageType::BGRA:
			*colorComponentsType = GL_RGB;
			*pixelDataFormat = GL_BGRA;
			*pixelDataType = GL_UNSIGNED_BYTE;
			return;
		default:
			LogCritical(LogCategory::GRAPHICS, "OpenGL support for image type is not yet implemented.")
	}
}

}