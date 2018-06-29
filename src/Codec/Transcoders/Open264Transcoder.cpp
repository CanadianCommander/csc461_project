#include "../Codec.h"
#include "Open264Transcoder.h"
#include "../Packets/OpenH264Packet.h"

#include <chrono>

//clamp as that provided by std in C++17
static int inline clamp(int num, int low, int high)
{
	num = num >= low ? num : low;
	num = num <= high ? num : high;
	return num;
}


namespace Codec {

Open264Transcoder::Open264Transcoder()
{
	_encoder = nullptr;
	_encoderDelayInit = false;
	_decoder = nullptr;
}

Open264Transcoder::~Open264Transcoder()
{
	if (_encoder)
	{
		_encoder->Uninitialize();
		WelsDestroySVCEncoder(_encoder);
	}
	if (_decoder)
	{
		_decoder->Uninitialize();
		WelsDestroyDecoder(_decoder);
	}
}


void
Open264Transcoder::InitEncoderEx(bool delayInit, EUsageType euType, float frameRate, uint16_t width, uint16_t height,
                                 uint32_t targetBitrate)
{
	if (_encoder)
	{
		LogCodec("Encoder Intialized Twice", false);
		return;
	}

	if (WelsCreateSVCEncoder(&_encoder))
	{
		LogCodec("Error creating open H 264 encoder", true);
		throw EncoderException("Error creating h264 encoder");
	}
	LogCodec("new openh264 encoder created ", false)

	if (delayInit)
	{
		LogCodec("openh264 init delayed", false);
		_encoderDelayInit = true;
	} else
	{
		_SetEncoderOptions(euType, frameRate, width, height, targetBitrate);
	}
}

void Open264Transcoder::InitEncoder()
{
	InitEncoderEx(true, (EUsageType)0, 0, 0, 0, 0);
}

void Open264Transcoder::FeedFrame(std::shared_ptr<IO::Image> src)
{
	if (_encoderDelayInit)
	{
		LogCodec("Doing delayed encoder init", false);
		_encoderDelayInit = false;
		_SetEncoderOptions(SCREEN_CONTENT_REAL_TIME, 60.0f,
		                   src->GetWidth(), src->GetHeight());
	}

	SFrameBSInfo info;
	memset(&info, 0, sizeof(SFrameBSInfo));
	auto openh264Img = _ImageToSourcePicture(src);
	_encoder->ForceIntraFrame(true);
	if (_encoder->EncodeFrame(openh264Img.get(), &info))
	{
		LogCodec("Error encoding image!", true);
		throw EncoderException("Error encoding image!");
	}
	LogCodec("Image fed to encoder", false);

	if (info.eFrameType != videoFrameTypeInvalid && info.eFrameType != videoFrameTypeSkip)
	{
		_frameQueue.push(info);
		LogCodec("New packet from encoder\n- Type: %d\n- Size: %d\n- Time: %u",
		         false, info.eFrameType, info.iFrameSizeInBytes, info.uiTimeStamp);
	}

}

std::shared_ptr<Packet> Open264Transcoder::NextPacket()
{
	if (_frameQueue.size() > 0)
	{
		auto res = std::make_shared<OpenH264Packet>(_frameQueue.front());
		_frameQueue.pop();
		return res;
	} else
	{
		throw EncoderException("Packet Queue Empty");
	}
}

void Open264Transcoder::_SetEncoderOptions(EUsageType euType, float frameRate, uint16_t width,
                                           uint16_t height, uint32_t targetBitrate)
{
	SEncParamBase params;
	memset(&params, 0, sizeof(SEncParamBase));
	params.iUsageType = euType;
	params.fMaxFrameRate = frameRate;
	params.iPicWidth = width;
	params.iPicHeight = height;
	params.iTargetBitrate = targetBitrate;
	params.iRCMode = RC_QUALITY_MODE;
	if (_encoder->Initialize(&params))
	{
		LogCodec("Error Initializing Encoder", true);
		throw EncoderException("Error Initializing Encoder");
	}

	LogCodec(
			"Open 264 encoder options set\n- usage type: %d \n- frame rate: %f \n- picture width: %d \n- picture height: %d \n- bitrate: %d \n",
			false, euType, frameRate, width, height, targetBitrate);
}

std::shared_ptr<SSourcePicture> Open264Transcoder::_ImageToSourcePicture(std::shared_ptr<IO::Image> image)
{
	LogCodec("Image Conversion on Image of Width: %d Height: %d", false,
	         image->GetWidth(), image->GetHeight());

	std::shared_ptr<SSourcePicture> pic = std::shared_ptr<SSourcePicture>(new SSourcePicture(),
	                                                                      [](SSourcePicture* sp) -> void
	                                                                      {
		                                                                      delete[] sp->pData[0];
		                                                                      delete[] sp->pData[1];
		                                                                      delete[] sp->pData[2];
	                                                                      });
	auto data = (uint8_t*)image->GetData();
	memset(pic.get(), 0, sizeof(SSourcePicture));


	pic->iPicWidth = image->GetWidth();
	pic->iPicHeight = image->GetHeight();
	pic->iColorFormat = videoFormatI420;
	pic->iStride[0] = pic->iPicWidth;
	pic->iStride[1] = pic->iStride[2] = pic->iPicWidth / 2;
	pic->pData[0] = new uint8_t[pic->iPicWidth * pic->iPicHeight];
	pic->pData[1] = new uint8_t[(pic->iPicWidth * pic->iPicHeight) / 4];
	pic->pData[2] = new uint8_t[(pic->iPicWidth * pic->iPicHeight) / 4];

	//RGB -> I420 YUV image
#pragma omp parallel for schedule(dynamic)
	for (int z = 0; z < pic->iPicHeight; z += 1)
	{
		//buffers to take advantage of cpu cache
		uint8_t lineBufferY[pic->iStride[0]];
		uint8_t lineBufferU[pic->iStride[0] / 2];
		uint8_t lineBufferV[pic->iStride[0] / 2];

		for (int i = 0; i < pic->iStride[0] * 3; i += 3)
		{
			//calculate YUV
			auto Y = (0.299 * (data[i + pic->iStride[0] * z * 3]))
			         + (0.587 * (data[i + 1 + pic->iStride[0] * z * 3]))
			         + (0.114 * (data[i + 2 + pic->iStride[0] * z * 3]))
			         + 16;
			auto U = (0.439 * (data[i + pic->iStride[0] * z * 3]))
			         + (-0.368 * (data[i + 1 + pic->iStride[0] * z * 3]))
			         + (-0.071 * (data[i + 2 + pic->iStride[0] * z * 3]))
			         + 128;
			auto V = (-0.148 * (data[i + pic->iStride[0] * z * 3]))
			         + (-0.291 * (data[i + 1 + pic->iStride[0] * z * 3]))
			         + (0.439 * (data[i + 2 + pic->iStride[0] * z * 3]))
			         + 128;

			lineBufferY[i / 3] = (uint8_t)clamp((int)Y, 0, 255);
			lineBufferU[(i / 3) / 2] = (uint8_t)clamp((int)U, 0, 255);
			lineBufferV[(i / 3) / 2] = (uint8_t)clamp((int)V, 0, 255);
		}
		memcpy((*pic->pData + pic->iPicWidth * z), lineBufferY, (size_t)pic->iStride[0]);
		memcpy((*(pic->pData + 1) + (pic->iPicWidth / 2) * (z / 2)), lineBufferU, (size_t)pic->iStride[0] / 2);
		memcpy((*(pic->pData + 2) + (pic->iPicWidth / 2) * (z / 2)), lineBufferV, (size_t)pic->iStride[0] / 2);

	}
	return pic;
}

std::shared_ptr<IO::Image>
Open264Transcoder::_SourcePictureToImage(uint8_t** yuvData, uint32_t w, uint32_t h, uint32_t strideY, uint32_t strideUV,
                                         float colorCRed, float colorCGreen, float colorCBlue)
{
	uint8_t rgbBuffer[w * h * 3];
	LogCodec("YUV to RGB with\n- width: %d\n- height: %d\n- stride Y: %d\n- stride UV: %d", false, w, h, strideY,
	         strideUV);

	// I420 YUV -> RGB Image
#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < h; y++)
	{
		//buffer to take advantage of cpu cache
		uint8_t lineBuffer[w * 3];

		for (int x = 0; x < w; x++)
		{
			auto R = 1.164 * (yuvData[0][strideY * y + x] - 20)
			         + 2.018 * (yuvData[1][(strideY / 2) * (y / 2) + x / 2] - 128);
			auto G = 1.164 * (yuvData[0][strideY * y + x] - 16)
			         - 0.813 * (yuvData[1][(strideY / 2) * (y / 2) + x / 2] - 128)
			         - 0.391 * (yuvData[2][(strideY / 2) * (y / 2) + x / 2] - 128);
			auto B = 1.164 * (yuvData[0][strideY * y + x] - 16)
			         + 1.596 * (yuvData[2][(strideY / 2) * (y / 2) + x / 2] - 128);

			lineBuffer[x * 3] = (uint8_t)clamp((int)(R * colorCRed), 0, 255);
			lineBuffer[x * 3 + 1] = (uint8_t)clamp((int)(G * colorCGreen), 0, 255);
			lineBuffer[x * 3 + 2] = (uint8_t)clamp((int)(B * colorCBlue), 0, 255);
		}

		memcpy((rgbBuffer + y * w * 3), lineBuffer, w * 3);
	}

	//rgbBuffer is "&rgbBuffer[0]" instead of just "rgbBuffer" because template madness
	// throws error in the later case, even though this case does work with the vanilla "new ..."
	auto res = std::make_shared<IO::Image>(IO::ImageType::RGB, w, h, &rgbBuffer[0], w * h * 3);
	return res;
}

void Open264Transcoder::InitDecoder()
{
	if (_decoder)
	{
		LogCodec("Decoder already initialized", false);
		return;
	}

	if (WelsCreateDecoder(&_decoder))
	{
		LogCodec("Error creating decoder", true);
		throw DecoderException("Error allocating h264 decoder!");
	}

	_SetDecoderOptions();
}

void Open264Transcoder::_SetDecoderOptions()
{
	SDecodingParam sdParam;
	memset(&sdParam, 0, sizeof(SDecodingParam));

	sdParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_SVC;

	if (_decoder->Initialize(&sdParam))
	{
		LogCodec("Decoder Set Options failed!", true);
		throw EncoderException("Error setting decoder options");
	} else
	{
		LogCodec("Decoder Options set", false);
	}
}

void Open264Transcoder::FeedPacket(Packet* pk)
{
	uint8_t* yuvData[3];
	SBufferInfo sDstBufferInfo;
	memset(&sDstBufferInfo, 0, sizeof(SBufferInfo));
	//technically a memory over commit
	uint8_t YData[pk->GetRawDataLen()];
	uint8_t UData[pk->GetRawDataLen()];
	uint8_t VData[pk->GetRawDataLen()];

	memset(YData, 0, pk->GetRawDataLen());
	memset(UData, 0, pk->GetRawDataLen());
	memset(VData, 0, pk->GetRawDataLen());

	yuvData[0] = YData;
	yuvData[1] = UData;
	yuvData[2] = VData;

	int res = _decoder->DecodeFrameNoDelay(pk->GetRawData(), pk->GetRawDataLen(), yuvData, &sDstBufferInfo);
	if (res)
	{
		LogCodec("Error Decoding Frame! with error code: %d", true, res);
		throw DecoderException("Could Not Decode Frame!");
	} else
	{
		LogCodec("Frame Decoded", false);
	}

	if (sDstBufferInfo.iBufferStatus == 1)
	{
		_decoderQueue.push(_SourcePictureToImage(yuvData, sDstBufferInfo.UsrData.sSystemBuffer.iWidth,
		                                         sDstBufferInfo.UsrData.sSystemBuffer.iHeight,
		                                         sDstBufferInfo.UsrData.sSystemBuffer.iStride[0],
		                                         sDstBufferInfo.UsrData.sSystemBuffer.iStride[1]));
	} else
	{
		LogCodec("Frame Dropped", false);
	}
}

std::shared_ptr<IO::Image> Open264Transcoder::NextImage()
{
	if (_decoderQueue.size() > 0)
	{
		auto res = _decoderQueue.front();
		_decoderQueue.pop();
		return res;
	} else
	{
		throw DecoderException("Queue Empty");
	}
}

}
