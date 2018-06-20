#pragma once
#include "Transcoder.h"
#include "../../../3rdParty/libde265/libde265/en265.h"

namespace Codec {
  class DE265Transcoder: public Transcoder {
  public:
    //general methods
    DE265Transcoder();
    virtual ~DE265Transcoder();

    //encoder functions
    /**
      initalize encoder
      @param cThreads the number of system threads to use for encoding
    **/
    void InitEncoderEx(uint16_t cThreads);
    //---- encoder overrides ----
    void InitEncoder() override;
    void FeedFrame(std::shared_ptr<IO::Image> src) override;
    std::shared_ptr<Packet> NextPacket() override;

    //decoder functions

  private:
    de265_image * _ImageToDe265Image(std::shared_ptr<IO::Image> img);

    en265_encoder_context * _encContext;

  };
}
