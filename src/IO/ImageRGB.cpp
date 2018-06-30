#include "ImageRGB.h"
#include "../Graphics/Texture.h"

namespace IO {

  ImageRGB::ImageRGB(uint8_t * rawRGB, uint32_t width, uint32_t height, uint32_t len){
    _rawRGB = new uint8_t[len];
    memcpy(_rawRGB, rawRGB, len);

    _width = width;
    _height = height;
    _lenBytes = len;
  }

  ImageRGB::~ImageRGB(){
    delete [] _rawRGB;
  }

  ImageFormat ImageRGB::GetRawImageFormat() {
    return IMAGE_FORMAT_RGB;
  }

  std::shared_ptr<std::vector<ImageFormat>>ImageRGB::GetSupportedImageFormats() {
    ImageFormat sFormats[] = {IMAGE_FORMAT_RGB};
    auto outV = std::make_shared<std::vector<ImageFormat>>();
    outV->assign(sFormats, sFormats + 1);
    return outV;
  }

  void * ImageRGB::GetRawDataPtr(){
    return (void*)_rawRGB;
  }

  std::shared_ptr<std::vector<uint8_t>> ImageRGB::GetImageBuffer(ImageFormat f) {
    switch(f){
      case IMAGE_FORMAT_RGB:
        return GetRGBBuffer();
    }

    throw std::invalid_argument("Image format not supported");
  };

  std::shared_ptr<std::vector<uint8_t> > ImageRGB::GetRGBBuffer(){
    auto vec = std::make_shared<std::vector<uint8_t> >();
    vec->assign(_rawRGB, _rawRGB + _lenBytes);
    return vec;
  }

  uint32_t ImageRGB::GetWidth(){
    return _width;
  }

  uint32_t ImageRGB::GetHeight(){
    return _height;
  }

  uint64_t ImageRGB::GetDataByteLen() {
    return _lenBytes;
  }

  void ImageRGB::ConstructTexture(Graphics::Texture * t){
    *t = Graphics::Texture(*this);
  }
}
