#pragma once

namespace Codec {

class Packet
{
public:
	virtual uint8_t* GetRawData() = 0;
	virtual uint32_t GetRawDataLen() = 0;
};

}
