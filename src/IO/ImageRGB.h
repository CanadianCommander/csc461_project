#pragma once
#include <vector>
#include <memory>
#include "Image.h"

namespace IO {
  class ImageRGB: public Image {
  public:
    ImageRGB(uint8_t * rawRGB, uint32_t width, uint32_t height, uint32_t len);
    ~ImageRGB();

    void * GetRawDataPtr() override;
    std::shared_ptr<std::vector<uint8_t>> GetRGBBuffer() override;
    uint32_t GetWidth() override;
    uint32_t GetHeight() override;
    uint64_t GetDataByteLen() override;

  private:
    uint8_t* _rawRGB;
    uint32_t _lenBytes;
    uint32_t _width;
    uint32_t _height;
  };
}
