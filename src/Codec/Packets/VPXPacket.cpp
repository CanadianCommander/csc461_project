#include "VPXPacket.h"
#include <memory.h>

namespace Codec {
  VPXPacket::VPXPacket(uint8_t * data, uint64_t size){
    _data = new uint8_t[size];
    memcpy(_data, data, size);
    _sz = size;
  };

  VPXPacket::~VPXPacket(){
    if(_data){
      delete [] _data;
    }
  }

  uint8_t * VPXPacket::GetRawData(){
    return  _data;
  }

  uint32_t  VPXPacket::GetRawDataLen(){
    return _sz;
  }
}
