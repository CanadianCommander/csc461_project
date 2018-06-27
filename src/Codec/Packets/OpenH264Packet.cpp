#include "OpenH264Packet.h"

namespace Codec {
  OpenH264Packet::OpenH264Packet(SFrameBSInfo pack){
    _openH264Packet = pack;
  };

  uint8_t * OpenH264Packet::GetRawData(){
    return  (uint8_t*)_openH264Packet.sLayerInfo[0].pBsBuf;
  }

  uint32_t  OpenH264Packet::GetRawDataLen(){
    return _openH264Packet.iFrameSizeInBytes;
  }
}
