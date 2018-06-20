#pragma once
#include <memory>
#include "Packet.h"
#include "../../../3rdParty/libde265/libde265/en265.h"

namespace Codec {

  class DE265Packet: public Packet {
  public:
    DE265Packet(std::shared_ptr<en265_packet> pack){dePacket = pack;};
    uint8_t * GetRawData() override {return (uint8_t *)dePacket->data;}

  private:
    std::shared_ptr<en265_packet> dePacket;

  };
}
