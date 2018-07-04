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
    ImageFormat sFormats[] = {IMAGE_FORMAT_BGR, IMAGE_FORMAT_RGB, IMAGE_FORMAT_YUV420};
    auto outV = std::make_shared<std::vector<ImageFormat>>();
    outV->assign(sFormats,sFormats + 3);
    return outV;
  }

  void * ImageBGRA::GetRawDataPtr(){
    return (void*)_rawBGRA;
  }

  std::shared_ptr<std::vector<uint8_t>> ImageBGRA::GetImageBuffer(ImageFormat f) {
    switch(f){
      case IMAGE_FORMAT_RGB:
        return GetRGBBuffer();

      case IMAGE_FORMAT_YUV420:
        return GetYUV420Buffer();

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

  std::shared_ptr<std::vector<uint8_t>> ImageBGRA::GetYUV420Buffer(){
    std::shared_ptr<std::vector<uint8_t>> buff = std::make_shared<std::vector<uint8_t>>(_width*_height + (_width*_height)/2,0);

    //BGRA -> I420 YUV image
    #pragma omp parallel for schedule(dynamic)
    for(int z = 0; z < _height; z +=1){
      //buffers to take advantage of cpu cache
      uint8_t lineBufferY[_width];
      uint8_t lineBufferU[_width/2];
      uint8_t lineBufferV[_width/2];

      for(int i =0; i < _width*4; i +=4){
        //calculate YUV
        int Y = (0.299*(_rawBGRA[i + 2 + _width*z*4]))
                  + (0.587*(_rawBGRA[i+1 + _width*z*4]))
                  + (0.114*(_rawBGRA[i + _width*z*4]))
                  + 16;
        int U = (0.439*(_rawBGRA[i + 2 + _width*z*4]))
                  + (-0.368*(_rawBGRA[i+1 + _width*z*4]))
                  + (-0.071*(_rawBGRA[i + _width*z*4]))
                  + 128;
        int V = (-0.148*(_rawBGRA[i + 2 + _width*z*4]))
                  + (-0.291*(_rawBGRA[i+1 + _width*z*4]))
                  + (0.439*(_rawBGRA[i + _width*z*4]))
                  + 128;

        lineBufferY[i/4] = clamp(Y,0,255);
        lineBufferU[(i/4)/2] = clamp(U,0,255);
        lineBufferV[(i/4)/2] = clamp(V,0,255);
      }
      memcpy((&(buff->at(0)) + _width*z), lineBufferY, _width);
      memcpy((&(buff->at(_width*_height)) + (_width/2)*(z/2)), lineBufferU, _width/2);
      memcpy((&(buff->at((5*_width*_height)/4)) + (_width/2)*(z/2)), lineBufferV, _width/2);

    }
    return buff;
  }
}
