#pragma once
#include <memory>
#include "Transcoder.h"
#include "codec_api.h"

namespace Codec{

  class Open264Transcoder: public Transcoder {
  public:
    //general methods
    Open264Transcoder();
    virtual ~Open264Transcoder();

    //encoder functions
    //init encoder w/ additonal options
    void InitEncoderEx(bool delayInit, EUsageType euType, float frameRate, uint16_t width,
                      uint16_t height, uint32_t targetBitrate=5000000);
    //---- encoder overrides ----
    void InitEncoder() override;
    void FeedFrame(std::shared_ptr<IO::Image> src) override;
    std::shared_ptr<Packet> NextPacket() override;

  private:
    void _SetEncoderOptions(EUsageType euType, float frameRate, uint16_t width, uint16_t height,
                            uint32_t targetBitrate=5000000);
    std::shared_ptr<SSourcePicture> _ImageToSourcePicture(std::shared_ptr<IO::Image> img);

    ISVCEncoder * _encoder;
    bool          _encoderDelayInit;
  };

}
