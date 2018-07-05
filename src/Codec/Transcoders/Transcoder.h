#pragma once

#include "Encoder.h"
#include "Decoder.h"


namespace Codec {

  class Transcoder : public Encoder, public Decoder
  {

  };

}
