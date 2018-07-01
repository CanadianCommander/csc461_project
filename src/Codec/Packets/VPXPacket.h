#pragma once
#include <memory>
#include "Packet.h"
#include "codec_api.h"

namespace Codec {

  class VPXPacket: public Packet {
  public:
    VPXPacket(uint8_t * data, uint64_t size);
    virtual ~VPXPacket();
    uint8_t * GetRawData() override;
    uint32_t  GetRawDataLen() override;

  private:
    uint8_t * _data;
    uint64_t  _sz;

  };
}
