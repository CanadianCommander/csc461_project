#include "ImageYUV444.h"
#include <cstring>

namespace IO{
  ImageYUV444::ImageYUV444(uint8_t ** planeData, uint32_t width, uint32_t height, uint32_t stride){
    _YUVData = new uint8_t[height*stride*3];

    memcpy(_YUVData, planeData[0], height*stride);
    memcpy(_YUVData + height*stride, planeData[1], height*stride);
    memcpy(_YUVData + height*stride*2, planeData[2], height*stride);

    _Ysize = _Usize = _Vsize = stride*height;
    _Ystride = _Ustride = _Vstride = stride;
    _width = width;
    _height = height;
  }

  ImageYUV444::~ImageYUV444(){
    if(_YUVData){
      delete [] _YUVData;
    }
  }

  ImageFormat ImageYUV444::GetRawImageFormat(){
    return IMAGE_FORMAT_YUV444;
  }


  std::shared_ptr<std::vector<ImageFormat>> ImageYUV444::GetSupportedImageFormats() {
    ImageFormat sFormats[] = {IMAGE_FORMAT_RGB, IMAGE_FORMAT_YUV444};
    auto outV = std::make_shared<std::vector<ImageFormat>>();
    outV->assign(sFormats, sFormats + 2);
    return outV;
  }

  void * ImageYUV444::GetRawDataPtr(){
    return (void *) _YUVData;
  }

  std::shared_ptr<std::vector<uint8_t>> ImageYUV444::GetImageBuffer(ImageFormat f){
    switch(f){
      case IMAGE_FORMAT_RGB:
        return GetRGBBuffer();
      case IMAGE_FORMAT_YUV444:
        auto retYUVBuff = std::make_shared<std::vector<uint8_t>>();
        retYUVBuff->assign(_YUVData, _YUVData + _height*_Ystride + _height*_Ustride + _height*_Vstride);
        return retYUVBuff;
    }

    throw std::invalid_argument("Image format not supported");
  }

  std::shared_ptr<std::vector<uint8_t>> ImageYUV444::GetRGBBuffer(){
    std::shared_ptr<std::vector<uint8_t>> rgbBuff =
          std::make_shared<std::vector<uint8_t>>(_width*_height*3,0);


    #pragma omp parallel for schedule(dynamic)
    for(int y =0; y < _height; y ++){
      //buffer to take advantage of cpu cache
      uint8_t lineBuffer[_width*3];

      for(int x = 0; x < _width; x ++){
        int R = 1.164*(*(_YUVData + _Ystride*y + x) - 20)
                + 2.018*(*(_YUVData + _Ystride*y + x + _Ystride*_height) - 128);
        int G = 1.164*(*(_YUVData + _Ystride*y + x) - 16)
                - 0.813*(*(_YUVData + _Ystride*y + x + _Ystride*_height) - 128)
                - 0.391*(*(_YUVData + _Ystride*y + x + _Ystride*_height*2) - 128);
        int B = 1.164*(*(_YUVData + _Ystride*y + x) - 16)
                + 1.596*(*(_YUVData + _Ystride*y + x + _Ystride*_height*2) - 128);

        lineBuffer[x*3] = clamp(R,0,255);
        lineBuffer[x*3 + 1] = clamp(G,0,255);
        lineBuffer[x*3 + 2] = clamp(B,0,255);
      }

      memcpy((&rgbBuff->at(0) + y*_width*3), lineBuffer, _width*3);
    }

    return rgbBuff;
  }

  uint32_t ImageYUV444::GetWidth(){
    return _width;
  }

  uint32_t ImageYUV444::GetHeight(){
    return _height;
  }

  uint64_t ImageYUV444::GetDataByteLen(){
    return _height*_Ystride + _height*_Ustride + _height*_Vstride;
  }

  void ImageYUV444::ConstructTexture(Graphics::Texture * t){
    throw std::runtime_error("depreciated");
  }
}
