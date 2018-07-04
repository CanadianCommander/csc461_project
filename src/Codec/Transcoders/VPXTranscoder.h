#pragma once

#include <memory>
#include <queue>
#include <string>
#include "Transcoder.h"
#include "../Packets/VPXPacket.h"
#include "../../IO/Image.h"
#include "vpx/vpx_encoder.h"
#include "vpx/vpx_decoder.h"
#include "VPXHelper.h"

namespace Codec {
#define VPX_VERSION "vp8" // vp8

class VPXTranscoder : public Transcoder
{
public:
	//general methods
	VPXTranscoder();
	virtual ~VPXTranscoder();

	//encoder functions
	//init encoder w/ additonal options
	void InitEncoderEx(std::string codecName, uint32_t width, uint32_t height,
	                   uint16_t threadCount, uint32_t bitrate = 5000000);

	//---- encoder overrides ----
	void InitEncoder() override;
	void FeedFrame(std::shared_ptr<IO::Image> src) override;
	std::shared_ptr<Packet> NextPacket() override;


	//decoder functions
	void InitDecoderEx(std::string codecName);
	//--- decoder overrides ---
	void InitDecoder() override;
	void FeedPacket(Packet* pk) override;
	std::shared_ptr<IO::Image> NextImage() override;

private:
	std::shared_ptr<vpx_image> ImageToVPXImage(std::shared_ptr<IO::Image> img);
	std::shared_ptr<IO::Image> VPXImageToImage(vpx_image_t* img);


	// encoder stuff
	vpx_codec_ctx_t _encoder;
	int64_t _encoder_frame_num;
	std::queue<std::shared_ptr<VPXPacket>> _packetQueue;
	bool _delayEncoderInit;

	// decoder stuff
	vpx_codec_ctx_t _decoder;
	std::queue<std::shared_ptr<IO::Image>> _frameQueue;
};

}
