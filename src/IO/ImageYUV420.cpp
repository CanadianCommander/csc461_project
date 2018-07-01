#include "ImageYUV420.h"
#include <cstring>

namespace IO{
  ImageYUV420::ImageYUV420(uint8_t ** planeData, uint32_t width, uint32_t height, uint32_t Ystride, uint32_t UVstride){
    _Ysize = Ystride*height;
    _Usize = _Vsize = UVstride*(height/2);
    _Ystride = Ystride ;
    _Ustride = _Vstride = UVstride;
    _width = width;
    _height = height;

    _YUVData = new uint8_t[height*Ystride + (height/2)*_Ustride*2];

    memcpy(_YUVData, planeData[0], height*Ystride);
    memcpy(_YUVData + height*Ystride, planeData[1],  (height/2)*_Ustride);
    memcpy(_YUVData + height*Ystride + ((height/2)*_Vstride), planeData[2], (height/2)*_Vstride);

  }

  ImageYUV420::~ImageYUV420(){
    if(_YUVData){
      delete [] _YUVData;
    }
  }

  ImageFormat ImageYUV420::GetRawImageFormat(){
    return IMAGE_FORMAT_YUV420;
  }


  std::shared_ptr<std::vector<ImageFormat>> ImageYUV420::GetSupportedImageFormats() {
    ImageFormat sFormats[] = {IMAGE_FORMAT_RGB, IMAGE_FORMAT_YUV420};
    auto outV = std::make_shared<std::vector<ImageFormat>>();
    outV->assign(sFormats, sFormats + 2);
    return outV;
  }

  void * ImageYUV420::GetRawDataPtr(){
    return (void *) _YUVData;
  }

  std::shared_ptr<std::vector<uint8_t>> ImageYUV420::GetImageBuffer(ImageFormat f){
    switch(f){
      case IMAGE_FORMAT_RGB:
        return GetRGBBuffer();
      case IMAGE_FORMAT_YUV420:
        auto retYUVBuff = std::make_shared<std::vector<uint8_t>>();
        retYUVBuff->assign(_YUVData, _YUVData + _height*_Ystride + _height*_Ustride + _height*_Vstride);
        return retYUVBuff;
    }

    throw std::invalid_argument("Image format not supported");
  }

  std::shared_ptr<std::vector<uint8_t>> ImageYUV420::GetRGBBuffer(){
    std::shared_ptr<std::vector<uint8_t>> rgbBuff =
          std::make_shared<std::vector<uint8_t>>((_Ystride*_height + (_Ustride*(_height/2))*2)*3,0);


    #pragma omp parallel for schedule(dynamic)
    for(int y =0; y < _height; y ++){
      //buffer to take advantage of cpu cache
      uint8_t lineBuffer[_width*3];

      for(int x = 0; x < _width; x ++){
        int R = 1.164*(*(_YUVData + _Ystride*y + x) - 20)
                + 2.018*(*(_YUVData + _Ustride*(y/2) + x/2 + _Ystride*_height) - 128);
        int G = 1.164*(*(_YUVData + _Ystride*y + x) - 16)
                - 0.813*(*(_YUVData + _Ustride*(y/2) + x/2 + _Ystride*_height) - 128)
                - 0.391*(*(_YUVData + _Vstride*(y/2) + x/2 + _Ystride*_height + (_Ustride*(_height/2))) - 128);
        int B = 1.164*(*(_YUVData + _Ystride*y + x) - 16)
                + 1.596*(*(_YUVData + _Vstride*(y/2) + x/2 + _Ystride*_height + (_Ustride*(_height/2))) - 128);

        lineBuffer[x*3] = clamp(R,0,255);
        lineBuffer[x*3 + 1] = clamp(G,0,255);
        lineBuffer[x*3 + 2] = clamp(B,0,255);
      }

      memcpy((&rgbBuff->at(0) + y*_width*3), lineBuffer, _width*3);
    }

    return rgbBuff;
  }

  uint32_t ImageYUV420::GetWidth(){
    return _width;
  }

  uint32_t ImageYUV420::GetHeight(){
    return _height;
  }

  uint64_t ImageYUV420::GetDataByteLen(){
    return _height*_Ystride + (_height/2)*_Ustride + (_height/2)*_Vstride;
  }

  void ImageYUV420::ConstructTexture(Graphics::Texture * t){
    throw std::runtime_error("depreciated");
  }
}
