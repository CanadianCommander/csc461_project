#include "Screen.h"

#include "TextureRGB.h"

#ifdef __APPLE__
  #include <CoreGraphics/CoreGraphics.h>
#elif __linux__
  #include <gdk/gdk.h>
#endif

namespace nscreen{

  Screen::Screen(){
    #ifdef __APPLE__
      mDesktop = CGMainDisplayID();

    #else
      if(!gdk_init_check(NULL, NULL)){
        throw std::runtime_error("Gdk initialization error!");
      }

      //fetch system default screen
      mDesktop = gdk_get_default_root_window();
      if(!mDesktop){
        throw std::runtime_error("Could not fetch root window!");
      }
    #endif

    queryDisplayInformation();
  }

  Screen::~Screen(){

  }

  std::shared_ptr<Texture> Screen::getScreenFB(){
    return getScreenPixelBuffer();
  }

  void Screen::queryDisplayInformation(){
    #ifdef __APPLE__
      CGRect bounds = CGDisplayBounds(mDesktop);
      mDesktopX       = bounds.origin.x;
      mDesktopY       = bounds.origin.y;
      mDesktopWidth   = bounds.size.width;
      mDesktopHeight  = bounds.size.height;

    #elif __linux__
      mDesktopWidth = gdk_window_get_width(mDesktop);
      mDesktopHeight= gdk_window_get_height(mDesktop);
      gdk_window_get_origin(mDesktop, &mDesktopX, &mDesktopY);
    #endif
  }

  std::shared_ptr<Texture> Screen::getScreenPixelBuffer(){
    #ifdef __APPLE__
      CGRect r;
      r.origin.x = 0;
      r.origin.y = 0;
      r.size.width = mDesktopWidth;
      r.size.height = mDesktopHeight;
      CGImage * img = CGWindowListCreateImage(r, 0x02, kCGNullWindowID, 0x10);
      CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(img));
      const unsigned char * raw = CFDataGetBytePtr(data);
      uint32_t len = CFDataGetLength(data);

      //strip alpha
      unsigned char rawRGB[len];
      uint32_t rgbIndex =0;
      for(uint32_t i =0; i < len; i+=4){
        rawRGB[rgbIndex++] = raw[i];
        rawRGB[rgbIndex++] = raw[i + 1];
        rawRGB[rgbIndex++] = raw[i + 2];
      }

      //flip BGR to RGB
      for(uint32_t i =0; i < len; i+=3){
        uint8_t tmp = rawRGB[i];
        rawRGB[i] = rawRGB[i + 2];
        rawRGB[i + 2] = tmp;
      }


      auto res = std::make_shared<TextureRGB>((uint8_t *)rawRGB, CGImageGetWidth(img), CGImageGetHeight(img), len);
      CFRelease(data);

      return res;
    #elif __linux__
      GdkPixbuf * pix = gdk_pixbuf_get_from_window(mDesktop, 0, 0, mDesktopWidth, mDesktopHeight);
      if(pix){
        return std::make_shared<TextureRGB>((uint8_t *)gdk_pixbuf_read_pixels(pix),
                  gdk_pixbuf_get_width(pix), gdk_pixbuf_get_height(pix), gdk_pixbuf_get_byte_lenght(pix));

      }
    #endif
    return nullptr;
  }

}
