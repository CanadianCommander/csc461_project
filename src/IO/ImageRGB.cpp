#include "ImageRGB.h"


namespace IO {

  ImageRGB::ImageRGB(uint8_t * rawRGB, uint32_t width, uint32_t height, uint32_t len){
    mRawRGB = new uint8_t[len];
    memcpy(mRawRGB, rawRGB, len);

    mRGBWidth = width;
    mRGBHeight = height;
    mRGBLenBytes = len;
  }

  ImageRGB::~ImageRGB(){
    delete [] mRawRGB;
  }

  void * ImageRGB::GetRawDataPtr(){
    return (void*)mRawRGB;
  }

  std::shared_ptr<std::vector<uint8_t> > ImageRGB::GetRGBBuffer(){
    auto vec = std::make_shared<std::vector<uint8_t> >();
    vec->assign(mRawRGB, mRawRGB + mRGBLenBytes);
    return vec;
  }

  uint32_t ImageRGB::GetWidth(){
    return mRGBWidth;
  }

  uint32_t ImageRGB::GetHeight(){
    return mRGBHeight;
  }

  uint64_t ImageRGB::GetDataByteLen() {
    return mRGBLenBytes;
  }
}
