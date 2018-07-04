#include "VPXTranscoder.h"
#include "../Codec.h"
#include "../../IO/ImageYUV420.h"
#include <algorithm>
#include <cstring>
#include <chrono>

namespace Codec {
  //general methods
  VPXTranscoder::VPXTranscoder(){
    _delayEncoderInit = false;
    memset(&_encoder, 0, sizeof(vpx_codec_ctx_t));
    memset(&_decoder, 0, sizeof(vpx_codec_ctx_t));
  }

  VPXTranscoder::~VPXTranscoder(){
    vpx_codec_destroy(&_encoder);
    vpx_codec_destroy(&_decoder);
  }

  void VPXTranscoder::InitEncoderEx(std::string codecName, uint32_t width, uint32_t height,
                                      uint16_t threadCount, uint32_t bitrate){
    const VpxInterface * encoderInterface = get_vpx_encoder_by_name(codecName.c_str());
    if(!encoderInterface){
      LogCodec("Could Not find codec: %s", true, codecName.c_str());
      throw EncoderException("Error could not locate codec");
    }

    LogCodec("Using encoder w/ name: %s", false, vpx_codec_iface_name(encoderInterface->codec_interface()))

    vpx_codec_enc_cfg cfg;
    memset(&cfg, 0, sizeof(vpx_codec_enc_cfg));
    if(vpx_codec_enc_config_default(encoderInterface->codec_interface(), &cfg, 0)){
      throw EncoderException("Error could not get default encoder config");
    }

    cfg.g_w = width;
    cfg.g_h = height;
    cfg.g_threads = threadCount;
    cfg.rc_target_bitrate = bitrate;
    cfg.g_error_resilient = false;
    if(vpx_codec_enc_init(&_encoder, encoderInterface->codec_interface(), &cfg, 0)){
      throw EncoderException("Error Initalizing encoder ");
    }

    LogCodec("Encoder Created with\n- width: %d\n- height: %d\n- threads: %d\n- bitrate: %d", false,
                cfg.g_w, cfg.g_h, cfg.g_threads, cfg.rc_target_bitrate);

    _encoder_frame_num = 0;
  }


  void VPXTranscoder::InitEncoder(){
    _delayEncoderInit = true;
  }

  void VPXTranscoder::FeedFrame(std::shared_ptr<IO::Image> src){
    if(_delayEncoderInit){
      _delayEncoderInit = false;
      InitEncoderEx(VPX_VERSION, src->GetWidth(), src->GetHeight(), 4);
    }

    auto vpxImg = ImageToVPXImage(src);
    auto startTime = std::chrono::high_resolution_clock::now();
    auto res = vpx_codec_encode(&_encoder, vpxImg.get(),_encoder_frame_num++, 30000,  0, VPX_DL_REALTIME);
    LogCodec("---PERFORMANCE---\nEncoder Time %dms\n---PERFORMANCE---",
              false, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count());
    if(res){
      LogCodec("Error when encoding frame! code: %d", true, res);
      throw EncoderException("Error! Codec Error while encoding frame");
    }

    vpx_codec_iter_t it = NULL;
    const vpx_codec_cx_pkt_t * packet = nullptr;
    while((packet = vpx_codec_get_cx_data(&_encoder, &it)) != NULL){
      if(packet->kind == VPX_CODEC_CX_FRAME_PKT){
        LogCodec("New Packet From Encoder", false);
        _packetQueue.push(std::make_shared<VPXPacket>((uint8_t*)packet->data.frame.buf,packet->data.frame.sz));
      }
    }

  }

  std::shared_ptr<Packet> VPXTranscoder::NextPacket(){
    if(_packetQueue.size() > 0){
      auto pk = _packetQueue.front();
      _packetQueue.pop();
      return pk;
    }
    else{
      throw EncoderException("Packet Queue Empty");
    }
  }

  std::shared_ptr<vpx_image> VPXTranscoder::ImageToVPXImage(std::shared_ptr<IO::Image> img){
    std::shared_ptr<std::vector<IO::ImageFormat>> formats = img->GetSupportedImageFormats();

    auto found = std::find(formats->begin(), formats->end(), IO::IMAGE_FORMAT_YUV420);
    if(found == formats->end()){
      throw EncoderException("Error passed image does not support YUV420 format");
    }

    std::shared_ptr<vpx_image> vpxImg =
      std::shared_ptr<vpx_image>( vpx_img_alloc(NULL,VPX_IMG_FMT_I420, img->GetWidth(), img->GetHeight(), 0),
                                  [] (vpx_image* vp_i) -> void {vpx_img_free(vp_i);});

    std::shared_ptr<std::vector<uint8_t>> YUVData = img->GetImageBuffer(IO::IMAGE_FORMAT_YUV420);
    memcpy(vpxImg->planes[0], &YUVData->at(0), vpxImg->w*vpxImg->h);
    memcpy(vpxImg->planes[1], &YUVData->at(vpxImg->w*vpxImg->h), (vpxImg->w*vpxImg->h)/4);
    memcpy(vpxImg->planes[2], &YUVData->at((vpxImg->w*vpxImg->h*5)/4), (vpxImg->w*vpxImg->h)/4);

    return vpxImg;
  }

  std::shared_ptr<IO::Image> VPXTranscoder::VPXImageToImage(vpx_image_t * img){
    return std::make_shared<IO::ImageYUV420>(img->planes, img->d_w, img->d_h, img->stride[0], img->stride[1]);
  }

  void VPXTranscoder::InitDecoderEx(std::string codecName){
    const VpxInterface *decInterface = get_vpx_decoder_by_name(codecName.c_str());
    if(!decInterface){
      LogCodec("Error, unknown codec: %s", true, codecName.c_str());
      throw DecoderException("Error, uknown decoder codec!");
    }
    LogCodec("Using decoder w/ name: %s", false, vpx_codec_iface_name(decInterface->codec_interface()))

    auto res = vpx_codec_dec_init(&_decoder, decInterface->codec_interface(), NULL, 0);
    if(res){
      LogCodec("Error creating decoder, code: %d", true, res);
      throw DecoderException("Error, could not create decoder");
    }
  }

  void VPXTranscoder::InitDecoder(){
    InitDecoderEx(VPX_VERSION);
  }

  void VPXTranscoder::FeedPacket(Packet * pk){
    if(vpx_codec_decode(&_decoder, pk->GetRawData(), pk->GetRawDataLen(), NULL, 0)){
      throw DecoderException("Error decoding packet.");
    }

    vpx_codec_iter_t it = NULL;
    vpx_image_t * outFrame = nullptr;

    while((outFrame = vpx_codec_get_frame(&_decoder, &it)) != NULL){
      LogCodec("Got new frame from decoder W: %d H: %d", false, outFrame->d_w, outFrame->d_h);
      _frameQueue.push(VPXImageToImage(outFrame));
    }

  }

  std::shared_ptr<IO::Image> VPXTranscoder::NextImage(){
    if(_frameQueue.size() > 0){
      auto img = _frameQueue.front();
      _frameQueue.pop();
      return img;
    }
    else{
      throw DecoderException("Frame queue empty");
    }
  }

}
