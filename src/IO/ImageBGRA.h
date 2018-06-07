#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "Image.h"

namespace IO {
  class ImageBGRA: public Image {
  public:
    ImageBGRA(uint8_t * rawBGRA, uint32_t width, uint32_t height, uint32_t len);
    ~ImageBGRA();

    void SetAppleDestructor(std::function<void()> des);

    void * GetRawDataPtr() override;
    std::shared_ptr<std::vector<uint8_t>> GetRGBBuffer() override;
    uint32_t GetWidth() override;
    uint32_t GetHeight() override;
    uint64_t GetDataByteLen() override;

  private:
    uint8_t* _rawBGRA;
    uint32_t _lenBytes;
    uint32_t _width;
    uint32_t _height;
    std::function<void()>  _aDestruct;
  };
}
