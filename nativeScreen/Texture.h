#ifndef FRAME_BUFFER_H_
#define FRAME_BUFFER_H_
#include <vector>
#include <memory>


namespace nscreen {

  class Texture {
  public:
    virtual void * getRawDataPtr() = 0;
    virtual std::shared_ptr<std::vector<uint8_t> > getRGBBuffer() =0;
    virtual uint32_t getWidth() = 0;
    virtual uint32_t getHeight() = 0;
    virtual uint64_t getDataByteLen() = 0;

  };
}

#endif /* FRAME_BUFFER_H_ */
