
#include <vector>
#include "../../../3rdParty/libde265/libde265/image.h"
#include "DE265Transcoder.h"
#include "../Packets/DE265Packet.h"
#include "../Codec.h"

namespace Codec {


  DE265Transcoder::DE265Transcoder()
  {
  	_encContext = nullptr;
  }

  DE265Transcoder::~DE265Transcoder()
  {
  	if (_encContext)
  	{
  		try
  		{
  			//flush and free remaing encoded packets
  			en265_push_eof(_encContext);
  			for (;;)
  			{
  				auto foobar = NextPacket();
  			}
  		}
  		catch (EncoderException e)
  		{
  			//nop
  		}
  		en265_free_encoder(_encContext);
  		de265_free();
  	}
  }

  void DE265Transcoder::InitEncoderEx(uint16_t cThreads)
  {
  	if (_encContext)
  	{
  		LogCodec("Request to create encoder but, encorder has already been created!", true);
  		throw EncoderException("encoder context already exists!");
  	}
  	enable_logging(LogModule::LogEncoder);
  	de265_init();
  	_encContext = en265_new_encoder();
  	LogCodec("en265 encoder created", false);

  	if (en265_start_encoder(_encContext, cThreads))
  	{
  		LogCodec("Could not start encoding with %d threads", true, cThreads);
  		throw EncoderException("Encoding Initalization Error");
  	}
  	LogCodec("Encoder Started with %d threads", false, cThreads);
  }

  void DE265Transcoder::InitEncoder()
  {
  	InitEncoderEx(16);
  }

  void DE265Transcoder::FeedFrame(std::shared_ptr<IO::Image> src)
  {
  	de265_image* srcImg = _ImageToDe265Image(src);
  	if (en265_push_image(_encContext, srcImg))
  	{
  		LogCodec("Image Feed ERROR", true);
  		throw EncoderException("Error feeding image to encoder");
  	} else
  	{
  		LogCodec("Image fed to encoder, \nQueue Length %d \nState: %d",
  		         false, en265_current_input_queue_length(_encContext), en265_get_encoder_state(_encContext));
  	}
  }

  std::shared_ptr<Packet> DE265Transcoder::NextPacket()
  {
  	en265_packet* newPack = en265_get_packet(_encContext, 0);

  	if (!newPack)
  	{
  		LogCodec("Encoder queue empty", false)
  		throw EncoderException("No Packet available");
  	} else
  	{
  		LogCodec("fetched img form encoder", false);
  		std::shared_ptr<en265_packet> dePacket(newPack,
  		                                       [this](en265_packet* pack) -> void
  		                                       { en265_free_packet(this->_encContext, pack); });
  		std::shared_ptr<DE265Packet> newPacket = std::make_shared<DE265Packet>(dePacket);
  		return newPacket;
  	}
  }


  de265_image* DE265Transcoder::_ImageToDe265Image(std::shared_ptr<IO::Image> img)
  {

  	std::shared_ptr<std::vector<uint8_t>> rgbData = img->GetRGBBuffer();
  	de265_image* newImg = new de265_image;

  	newImg->alloc_image(img->GetWidth(), img->GetHeight(), de265_chroma_444, NULL, false,
  	                    NULL, /*NULL*/ 0, NULL, false);

  	LogCodec("Image Conversion on Image of Width: %d Height: %d", false, img->GetWidth(), img->GetHeight());
  	/**
  	  de image is layed out in planes. so it wants all red then all green then all blue
  	  instead of rgb rgb rgb ...
  	**/
  	/*
  	uint8_t * dataPtrR = newImg->get_image_plane(2);
  	uint8_t * dataPtrG = newImg->get_image_plane(0);
  	uint8_t * dataPtrB = newImg->get_image_plane(1);
  	uint32_t strideR = newImg->get_image_stride(2);
  	uint32_t strideG = newImg->get_image_stride(0);
  	uint32_t strideB = newImg->get_image_stride(1);


  	for(int y = 0; y < img->GetHeight(); y++){
  	  for(int x = 0; x < img->GetWidth(); x++){
  		*(dataPtrR + strideR*y + x) = (*rgbData.get())[y*3*img->GetHeight() + x*3 + 1];
  		*(dataPtrG + strideG*y + x) = (*rgbData.get())[y*3*img->GetHeight() + x*3 + 1];
  		*(dataPtrB + strideB*y + x) = (*rgbData.get())[y*3*img->GetHeight() + x*3 + 2];
  	  }
  	}*/
  	return newImg;
  }

}
