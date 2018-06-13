#include "ScreenCapture.h"
#include "ImageRGB.h"
#include "ImageBGRA.h"

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
	  _desktop = gdk_get_default_root_window();
	  if(!_desktop){
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
  	_desktopWidth = gdk_window_get_width(_desktop);
  	_desktopHeight= gdk_window_get_height(_desktop);
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

  	auto bgrImg = std::make_shared<ImageBGRA>((uint8_t*)raw, CGImageGetWidth(img), CGImageGetHeight(img), len);
    bgrImg->SetAppleDestructor([img, data] () -> void {CFRelease(data); CFRelease(img);});
  	//CFRelease(data);
    //CFRelease(img);
    return bgrImg;

  #elif __linux__
  	GdkPixbuf * pix = gdk_pixbuf_get_from_window(_desktop, 0, 0, _desktopWidth, _desktopHeight);
  	  if(pix){
		    auto res = std::make_shared<ImageRGB>((uint8_t *)gdk_pixbuf_read_pixels(pix),
  				  gdk_pixbuf_get_width(pix), gdk_pixbuf_get_height(pix), gdk_pixbuf_get_byte_length(pix));
        g_object_unref(pix);
        return res;
  	  }
  #endif

	return nullptr;
  }
}
