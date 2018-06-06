#ifndef TEXTURE_RGB_H_
#define TEXTURE_RGB_H_
#include <vector>
#include <memory>
#include "Texture.h"

namespace nscreen {
  class TextureRGB: public Texture {
  public:
    TextureRGB(uint8_t * rawRGB, uint32_t width, uint32_t height, uint32_t len);
    ~TextureRGB();


    void * getRawDataPtr() override;
    std::shared_ptr<std::vector<uint8_t> > getRGBBuffer() override;
    uint32_t getWidth() override;
    uint32_t getHeight() override;
    uint64_t getDataByteLen() override;

  private:
    uint8_t* mRawRGB;
    uint32_t mRGBLenBytes;
    uint32_t mRGBWidth;
    uint32_t mRGBHeight;
  };
}

#endif /* TEXTURE_RGB_H_ */
