#include "ScreenCapture.h"
#include "ImageRGB.h"

#ifdef __APPLE__
  #include <CoreGraphics/CoreGraphics.h>
#elif __linux__
  #include <gdk/gdk.h>
#endif

namespace IO {

  ScreenCapture::ScreenCapture()
  {
  #ifdef __APPLE__
  	_desktop = CGMainDisplayID();
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
  	QueryDisplayInformation();
  }


  void ScreenCapture::QueryDisplayInformation()
  {
  #ifdef __APPLE__
  	CGRect bounds = CGDisplayBounds(_desktop);
  	_desktopX = (int)(bounds.origin.x);
  	_desktopY = (int)(bounds.origin.y);
  	_desktopWidth = (int)(bounds.size.width);
  	_desktopHeight = (int)(bounds.size.height);

  #elif __linux__
  	_desktopWidth = gdk_window_get_width(mDesktop);
  	_desktopHeight= gdk_window_get_height(mDesktop);
  	gdk_window_get_origin(_desktop, &_desktopX, &_desktopY);
  #endif
  }

  std::shared_ptr<Image> ScreenCapture::GetScreenFrameBuffer()
  {
  #ifdef __APPLE__
  	CGRect r;
  	r.origin.x = 0;
  	r.origin.y = 0;
  	r.size.width = _desktopWidth;
  	r.size.height = _desktopHeight;
  	CGImage* img = CGWindowListCreateImage(r, 0x02, kCGNullWindowID, 0x10);
  	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(img));
  	const unsigned char* raw = CFDataGetBytePtr(data);
  	auto len = (uint32_t)CFDataGetLength(data);

  	//strip alpha
  	unsigned char * rawRGB = new unsigned char[len];
  	uint32_t rgbIndex = 0;
  	for (uint32_t i = 0; i < len; i += 4)
  	{
  		rawRGB[rgbIndex++] = raw[i];
  		rawRGB[rgbIndex++] = raw[i + 1];
  		rawRGB[rgbIndex++] = raw[i + 2];
  	}

  	//flip BGR to RGB
  	for (uint32_t i = 0; i < len; i += 3)
  	{
  		uint8_t tmp = rawRGB[i];
  		rawRGB[i] = rawRGB[i + 2];
  		rawRGB[i + 2] = tmp;
  	}

  	auto rgbImg = std::make_shared<ImageRGB>((uint8_t*)rawRGB, CGImageGetWidth(img), CGImageGetHeight(img), len);
    delete[] rawRGB;
  	CFRelease(data);
    CFRelease(img);
    return rgbImg;

  #elif __linux__
  	GdkPixbuf * pix = gdk_pixbuf_get_from_window(_desktop, 0, 0, _desktopWidth, _desktopHeight);
  	  if(pix){
  		return std::make_shared<ImageRGB>((uint8_t *)gdk_pixbuf_read_pixels(pix),
  				  gdk_pixbuf_get_width(pix), gdk_pixbuf_get_height(pix), gdk_pixbuf_get_byte_lenght(pix));

  	  }
  #endif

	return nullptr;
  }
}
