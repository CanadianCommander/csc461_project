#pragma once

#include <memory>

#include "../../IO/Image.h"
#include "../Packets/Packet.h"

namespace Codec {

class Encoder
{
public:
	virtual void InitEncoder() = 0;

	/**
	  feed frame, src in to the encoder.
	  @param src the image (or frame) you whish to encode
	**/
	virtual void FeedFrame(std::shared_ptr<IO::Image> src) = 0;

	/**
	  return the next encoding Packet.
	  Note. if no such packet is available (because processing is still under way) Encoder Exception should be thrown
	**/
	virtual std::shared_ptr<Packet> NextPacket() = 0;

};


class EncoderException : public std::runtime_error
{
public:
	EncoderException(std::string msg) : std::runtime_error(msg)
	{};
};

}
