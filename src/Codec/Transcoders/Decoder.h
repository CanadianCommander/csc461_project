#pragma once
#include <memory>
#include "../Packets/Packet.h"
#include "../../IO/Image.h"

namespace Codec {

  class Decoder {
  public:
    virtual void InitDecoder() = 0;
    virtual void FeedPacket(Packet * pk) = 0;
    virtual std::shared_ptr<IO::Image> NextImage() = 0;
  };

  class DecoderException : public std::runtime_error {
  public:
    DecoderException(std::string msg): std::runtime_error(msg) {};
  };

}
