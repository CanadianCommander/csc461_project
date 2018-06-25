#pragma once
#include <memory>
#include <queue>
#include "Transcoder.h"
#include "codec_api.h"
#include "../../IO/Image.h"

namespace Codec{

  class Open264Transcoder: public Transcoder {
  public:
    //general methods
    Open264Transcoder();
    virtual ~Open264Transcoder();

    //encoder functions
    //init encoder w/ additonal options
    void InitEncoderEx(bool delayInit, EUsageType euType, float frameRate, uint16_t width,
                      uint16_t height, uint32_t targetBitrate=10000000);
    //---- encoder overrides ----
    void InitEncoder() override;
    void FeedFrame(std::shared_ptr<IO::Image> src) override;
    std::shared_ptr<Packet> NextPacket() override;


    //decoder functions

    //--- decoder overrides ---
    void InitDecoder() override;
    void FeedPacket(Packet * pk) override;
    std::shared_ptr<IO::Image> NextImage() override;

  private:
    void _SetEncoderOptions(EUsageType euType, float frameRate, uint16_t width, uint16_t height,
                            uint32_t targetBitrate=10000000);
    void _SetDecoderOptions();

    std::shared_ptr<SSourcePicture> _ImageToSourcePicture(std::shared_ptr<IO::Image> img);
    std::shared_ptr<IO::Image>          _SourcePictureToImage(uint8_t ** yuvData, uint32_t w, uint32_t h);

    // encoder stuff
    ISVCEncoder * _encoder;
    bool          _encoderDelayInit;
    std::queue<SFrameBSInfo> _frameQueue;

    // decoder stuff
    ISVCDecoder * _decoder;
    std::queue<std::shared_ptr<IO::Image>> _decoderQueue;
  };

}
