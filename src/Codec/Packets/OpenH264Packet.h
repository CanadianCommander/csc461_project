#pragma once
#include <memory>
#include "Packet.h"
#include "codec_api.h"

namespace Codec {

  class OpenH264Packet: public Packet {
  public:
    OpenH264Packet(SFrameBSInfo pack);
    uint8_t * GetRawData() override;
    uint32_t  GetRawDataLen() override;

  private:
    SFrameBSInfo _openH264Packet;

  };
}
