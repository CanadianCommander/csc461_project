
#include "Texture.h"

namespace Graphics{

  Texture::Texture(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type) : GraphicsResource(){
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
  	glBindTexture(GL_TEXTURE_2D, _handle);
  	LogGL("gl texture handle generated");

  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  	LogGL("gl texture parameters set");

  	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
  	LogGL("gl GPU texture allocated");
  }

  Texture::Texture(const Texture &other): GraphicsResource(other){
    _width          = other._width;
    _height         = other._height;
    _internalFormat = other._internalFormat;
    _format         = other._format;
    _type           = other._type;
  }

  void Texture::UploadData(void* data, uint8_t unpackAlignment){
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

  void Texture::UploadData(std::shared_ptr<std::vector<uint8_t>> data, uint8_t unpackAlignment){
    UploadData((void *)(data->data()), unpackAlignment);
  }

  void Texture::BindTexture(){
		glBindTexture(GL_TEXTURE_2D, _handle);
    LogGL("gl texture bound");
  }
}
