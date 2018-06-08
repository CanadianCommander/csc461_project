#pragma once
#include <vector>
#include <memory>

namespace Graphics{
  class Texture;
}

namespace IO {

  class Image {
  public:
    virtual void * GetRawDataPtr() = 0;
    virtual std::shared_ptr<std::vector<uint8_t>> GetRGBBuffer() =0;
    virtual uint32_t GetWidth() = 0;
    virtual uint32_t GetHeight() = 0;
    virtual uint64_t GetDataByteLen() = 0;
    virtual void ConstructTexture(Graphics::Texture * t) = 0;

  };
}
