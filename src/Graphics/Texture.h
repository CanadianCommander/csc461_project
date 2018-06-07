#pragma once

#include <memory>
#include <vector>
#include "Graphics.h"
#include "GraphicsResource.h"

namespace Graphics {

  class Texture : GraphicsResource
  {
  public:
  	Texture(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type);
    Texture(const Texture &other);

  	void UploadData(void* data, uint8_t unpackAlignment = 4);
    void UploadData(std::shared_ptr<std::vector<uint8_t>> data, uint8_t unpackAlignment = 4);
    void BindTexture();

  	uint32_t GetWidth() { return _width; }
  	uint32_t GetHeight() { return _height; }

  private:
  	uint32_t _width;
  	uint32_t _height;
  	GLint _internalFormat;
  	GLenum _format;
  	GLenum _type;
  };
}
