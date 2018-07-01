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
    ImageFormat sFormats[] = {IMAGE_FORMAT_RGB, IMAGE_FORMAT_YUV444, IMAGE_FORMAT_YUV420};
    auto outV = std::make_shared<std::vector<ImageFormat>>();
    outV->assign(sFormats, sFormats + 3);
    return outV;
  }

  void * ImageRGB::GetRawDataPtr(){
    return (void*)_rawRGB;
  }

  std::shared_ptr<std::vector<uint8_t>> ImageRGB::GetImageBuffer(ImageFormat f) {
    switch(f){
      case IMAGE_FORMAT_RGB:
        return GetRGBBuffer();
      case IMAGE_FORMAT_YUV444:
        return GetYUV444Buffer();
      case IMAGE_FORMAT_YUV420:
        return GetYUV420Buffer();
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

  std::shared_ptr<std::vector<uint8_t>> ImageRGB::GetYUV444Buffer(){
    std::shared_ptr<std::vector<uint8_t>> buff = std::make_shared<std::vector<uint8_t>>(_width*_height*3,0);

    //RGB -> I420 YUV image
    #pragma omp parallel for schedule(dynamic)
    for(int z = 0; z < _height; z +=1){
      //buffers to take advantage of cpu cache
      uint8_t lineBufferY[_width];
      uint8_t lineBufferU[_width];
      uint8_t lineBufferV[_width];

      for(int i =0; i < _width*3; i +=3){
        //calculate YUV
        int Y = (0.299*(_rawRGB[i + _width*z*3]))
                  + (0.587*(_rawRGB[i+1 + _width*z*3]))
                  + (0.114*(_rawRGB[i+2 + _width*z*3]))
                  + 16;
        int U = (0.439*(_rawRGB[i + _width*z*3]))
                  + (-0.368*(_rawRGB[i+1 + _width*z*3]))
                  + (-0.071*(_rawRGB[i+2 + _width*z*3]))
                  + 128;
        int V = (-0.148*(_rawRGB[i + _width*z*3]))
                  + (-0.291*(_rawRGB[i+1 + _width*z*3]))
                  + (0.439*(_rawRGB[i+2 + _width*z*3]))
                  + 128;

        lineBufferY[i/3] = clamp(Y,0,255);
        lineBufferU[i/3] = clamp(U,0,255);
        lineBufferV[i/3] = clamp(V,0,255);
      }
      memcpy((&(buff->at(0)) + _width*z), lineBufferY, _width);
      memcpy((&(buff->at(_width*_height)) + _width*z), lineBufferU, _width);
      memcpy((&(buff->at(_width*_height*2)) + _width*z), lineBufferV, _width);

    }
    return buff;
  }

  std::shared_ptr<std::vector<uint8_t>> ImageRGB::GetYUV420Buffer(){
    std::shared_ptr<std::vector<uint8_t>> buff = std::make_shared<std::vector<uint8_t>>(_width*_height + (_width*_height)/2,0);

    //RGB -> I420 YUV image
    #pragma omp parallel for schedule(dynamic)
    for(int z = 0; z < _height; z +=1){
      //buffers to take advantage of cpu cache
      uint8_t lineBufferY[_width];
      uint8_t lineBufferU[_width/2];
      uint8_t lineBufferV[_width/2];

      for(int i =0; i < _width*3; i +=3){
        //calculate YUV
        int Y = (0.299*(_rawRGB[i + _width*z*3]))
                  + (0.587*(_rawRGB[i+1 + _width*z*3]))
                  + (0.114*(_rawRGB[i+2 + _width*z*3]))
                  + 16;
        int U = (0.439*(_rawRGB[i + _width*z*3]))
                  + (-0.368*(_rawRGB[i+1 + _width*z*3]))
                  + (-0.071*(_rawRGB[i+2 + _width*z*3]))
                  + 128;
        int V = (-0.148*(_rawRGB[i + _width*z*3]))
                  + (-0.291*(_rawRGB[i+1 + _width*z*3]))
                  + (0.439*(_rawRGB[i+2 + _width*z*3]))
                  + 128;

        lineBufferY[i/3] = clamp(Y,0,255);
        lineBufferU[(i/3)/2] = clamp(U,0,255);
        lineBufferV[(i/3)/2] = clamp(V,0,255);
      }
      memcpy((&(buff->at(0)) + _width*z), lineBufferY, _width);
      memcpy((&(buff->at(_width*_height)) + (_width/2)*(z/2)), lineBufferU, _width/2);
      memcpy((&(buff->at((5*_width*_height)/4)) + (_width/2)*(z/2)), lineBufferV, _width/2);

    }
    return buff;
  }

}
