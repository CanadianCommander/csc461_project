#include "ImageBGRA.h"
#include "../Graphics/Texture.h"

namespace IO {

  ImageBGRA::ImageBGRA(uint8_t * rawBGRA, uint32_t width, uint32_t height, uint32_t len){
    _rawBGRA = rawBGRA;

    _width = width;
    _height = height;
    _lenBytes = len;
  }

  ImageBGRA::~ImageBGRA(){
    if(_aDestruct){
      _aDestruct();
    }
  }

  void ImageBGRA::SetAppleDestructor(std::function<void()> des){
    _aDestruct = des;
  }

  ImageFormat ImageBGRA::GetRawImageFormat() {
    return IMAGE_FORMAT_BGR;
  }

  std::shared_ptr<std::vector<ImageFormat>> ImageBGRA::GetSupportedImageFormats() {
    ImageFormat sFormats[] = {IMAGE_FORMAT_BGR, IMAGE_FORMAT_RGB};
    auto outV = std::make_shared<std::vector<ImageFormat>>();
    outV->assign(sFormats,sFormats + 2);
    return outV;
  }

  void * ImageBGRA::GetRawDataPtr(){
    return (void*)_rawBGRA;
  }

  std::shared_ptr<std::vector<uint8_t>> ImageBGRA::GetImageBuffer(ImageFormat f) {
    switch(f){
      case IMAGE_FORMAT_RGB:
        return GetRGBBuffer();

      case IMAGE_FORMAT_BGR:
        auto retBRGBuff = std::make_shared<std::vector<uint8_t>>();
        retBRGBuff->assign(_rawBGRA, _rawBGRA + _lenBytes);
        return retBRGBuff;
    }

    throw std::invalid_argument("Image format not supported");
  };

  std::shared_ptr<std::vector<uint8_t>> ImageBGRA::GetRGBBuffer(){
    auto vec = std::make_shared<std::vector<uint8_t> >(_lenBytes);

    uint32_t vecIndex = 0;
    for(uint32_t i =0; i < _lenBytes; i+= 4){
      (*vec)[vecIndex++] = _rawBGRA[i + 2];
      (*vec)[vecIndex++] = _rawBGRA[i + 1];
      (*vec)[vecIndex++] = _rawBGRA[i];
    }

    return vec;
  }

  uint32_t ImageBGRA::GetWidth(){
    return _width;
  }

  uint32_t ImageBGRA::GetHeight(){
    return _height;
  }

  uint64_t ImageBGRA::GetDataByteLen() {
    return _lenBytes;
  }

  void ImageBGRA::ConstructTexture(Graphics::Texture * t){
    *t = Graphics::Texture(*this);
  }
}
