#include "TextureRGB.h"


namespace nscreen {

  TextureRGB::TextureRGB(uint8_t * rawRGB, uint32_t width, uint32_t height, uint32_t len){
    mRawRGB = new uint8_t[len];
    memcpy(mRawRGB, rawRGB, len);

    mRGBWidth = width;
    mRGBHeight = height;
    mRGBLenBytes = len;
  }

  TextureRGB::~TextureRGB(){
    delete [] mRawRGB;
  }

  void * TextureRGB::getRawDataPtr(){
    return (void*)mRawRGB;
  }

  std::shared_ptr<std::vector<uint8_t> > TextureRGB::getRGBBuffer(){
    auto vec = std::make_shared<std::vector<uint8_t> >();
    vec->assign(mRawRGB, mRawRGB + mRGBLenBytes);
    return vec;
  }

  uint32_t TextureRGB::getWidth(){
    return mRGBWidth;
  }

  uint32_t TextureRGB::getHeight(){
    return mRGBHeight;
  }

  uint64_t TextureRGB::getDataByteLen() {
    return mRGBLenBytes;
  }
}
