#pragma once
#include <vector>
#include <memory>

namespace Graphics{
  class Texture;
}

namespace IO {
  //list of image formats, add as needed
  enum ImageFormat{
    IMAGE_FORMAT_RGB,
    IMAGE_FORMAT_BGR,
    IMAGE_FORMAT_YUV444
  };

  class Image {
  public:
    virtual ImageFormat GetRawImageFormat() = 0;
    virtual std::shared_ptr<std::vector<ImageFormat>> GetSupportedImageFormats() = 0;
    virtual uint32_t GetWidth() = 0;
    virtual uint32_t GetHeight() = 0;
    virtual uint64_t GetDataByteLen() = 0;

    virtual void * GetRawDataPtr() = 0;
    virtual std::shared_ptr<std::vector<uint8_t>> GetImageBuffer(ImageFormat f) =0;
    virtual std::shared_ptr<std::vector<uint8_t>> GetRGBBuffer() =0;
    virtual void ConstructTexture(Graphics::Texture * t) = 0;//remove?

  };
}
