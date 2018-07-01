#pragma once
#include <vector>
#include <memory>
#include "Image.h"

namespace IO {
  class ImageYUV420: public Image {
  public:
    ImageYUV420(uint8_t ** planeData, uint32_t width, uint32_t height, uint32_t Ystride, uint32_t UVstride);
    ~ImageYUV420();

    ImageFormat GetRawImageFormat() override;
    std::shared_ptr<std::vector<ImageFormat>> GetSupportedImageFormats() override;
    void * GetRawDataPtr() override;
    std::shared_ptr<std::vector<uint8_t>> GetImageBuffer(ImageFormat f) override;
    std::shared_ptr<std::vector<uint8_t>> GetRGBBuffer() override;
    uint32_t GetWidth() override;
    uint32_t GetHeight() override;
    uint64_t GetDataByteLen() override;
    void ConstructTexture(Graphics::Texture * t) override;

  private:
    uint8_t * _YUVData;
    uint32_t _Ysize;
    uint32_t _Usize;
    uint32_t _Vsize;
    uint32_t _Ystride;
    uint32_t _Ustride;
    uint32_t _Vstride;

    uint32_t _width;
    uint32_t _height;

  };
}
