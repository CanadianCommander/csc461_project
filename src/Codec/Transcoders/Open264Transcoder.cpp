#include "../Codec.h"
#include "Open264Transcoder.h"

#include <memory>
#include <cstring>

namespace Codec{

  Open264Transcoder::Open264Transcoder(){
    _encoder = nullptr;
    _encoderDelayInit = false;
  }

  Open264Transcoder::~Open264Transcoder(){
    if(_encoder){
      _encoder->Uninitialize();
      WelsDestroySVCEncoder(_encoder);
    }
  }


  void Open264Transcoder::InitEncoderEx(bool delayInit, EUsageType euType, float frameRate, uint16_t width, uint16_t height, uint32_t targetBitrate){
    if(_encoder){
      LogCodec("Encoder Intialized Twice", false);
      return;
    }

    if(WelsCreateSVCEncoder(&_encoder)){
      LogCodec("Error creating open H 264 encoder", true);
      throw EncoderException("Error creating h264 encoder");
    }
    LogCodec("new openh264 encoder created ", false)

    if(delayInit){
      LogCodec("openh264 init delayed", false);
      _encoderDelayInit = true;
    }
    else{
      _SetEncoderOptions(euType, frameRate, width, height, targetBitrate);
    }
  }

  void Open264Transcoder::InitEncoder(){
    InitEncoderEx(true,(EUsageType)0,0,0,0,0);
  }

  void Open264Transcoder::FeedFrame(std::shared_ptr<IO::Image> src){
    if(_encoderDelayInit){
      LogCodec("Doing delayed encoder init", false);
      _encoderDelayInit = false;
      _SetEncoderOptions(EUsageType::SCREEN_CONTENT_REAL_TIME, 60.0f,
                          src->GetWidth(), src->GetHeight());
    }

    SFrameBSInfo info;
    auto openh264Img = _ImageToSourcePicture(src);
    if(_encoder->EncodeFrame(openh264Img.get(), &info)){
      LogCodec("Error encoding image!", true);
      throw EncoderException("Error encoding image!");
    }
    LogCodec("Image fed to encoder", false);

    if(info.eFrameType != videoFrameTypeInvalid){
      LogCodec("New packet from encoder\n- Frame: %d\n- Size: %d\n- Time: %u",
                false, info.eFrameType, info.iFrameSizeInBytes, info.uiTimeStamp);
    }

  }

  std::shared_ptr<Packet> Open264Transcoder::NextPacket(){

  }

  void Open264Transcoder::_SetEncoderOptions(EUsageType euType, float frameRate, uint16_t width,
                          uint16_t height, uint32_t targetBitrate){
    SEncParamBase params;
    memset(&params, 0, sizeof(SEncParamBase));
    params.iUsageType = euType;
    params.fMaxFrameRate = frameRate;
    params.iPicWidth = width;
    params.iPicHeight = height;
    params.iTargetBitrate = targetBitrate;
    if(_encoder->Initialize(&params)){
      LogCodec("Error Initializing Encoder", true);
      throw EncoderException("Error Initializing Encoder");
    }

    LogCodec("Open 264 encoder options set\n- usage type: %d \n- frame rate: %f \n- picture width: %d \n- picture height: %d \n- bitrate: %d \n",
            false, euType, frameRate, width, height, targetBitrate);
  }

  std::shared_ptr<SSourcePicture> Open264Transcoder::_ImageToSourcePicture(std::shared_ptr<IO::Image> img){
    LogCodec("Image Conversion on Image of Width: %d Height: %d", false,
              img->GetWidth(), img->GetHeight());

    std::shared_ptr<SSourcePicture> pic = std::make_shared<SSourcePicture>();
    std::shared_ptr<std::vector<uint8_t>> rgbData = img->GetRGBBuffer();
    memset(pic.get(), 0, sizeof(SSourcePicture));


    pic->iPicWidth = img->GetWidth();
    pic->iPicHeight = img->GetHeight();
    pic->iColorFormat = videoFormatI420;
    pic->iStride[0] = pic->iStride[1] = pic->iStride[2] = pic->iPicWidth;
    pic->pData[0] = &(*rgbData.get())[0];

    return pic;
  }
}
