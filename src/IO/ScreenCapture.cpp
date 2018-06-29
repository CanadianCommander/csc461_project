#ifdef __APPLE__
#include <CoreGraphics/CoreGraphics.h>
#elif __linux__
#include <gdk/gdk.h>
#endif

#include "ScreenCapture.h"
#include "Image.h"
#include "../Debug.h"

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
	auto imageReference = CGWindowListCreateImage(CGRectInfinite, kCGWindowListOptionOnScreenOnly, kCGNullWindowID,
	                                              kCGWindowImageNominalResolution);
	LogVerboseOrElseCritical(LogCategory::IO, imageReference != nullptr, "CGWindowListCreateImage");
	auto dataProvider = CGImageGetDataProvider(imageReference);
	auto dataReference = CGDataProviderCopyData(dataProvider);
	const uint8_t* data = CFDataGetBytePtr(dataReference);
	auto dataLength = (uint32_t)CFDataGetLength(dataReference);

	auto colorSpaceReference = CGImageGetColorSpace(imageReference);
	LogVerboseOrElseCritical(LogCategory::IO, colorSpaceReference != nullptr, "CGImageGetColorSpace");
	auto colorSpaceName = CGColorSpaceGetName(colorSpaceReference);
	LogVerboseOrElseCritical(LogCategory::IO, colorSpaceName != nullptr, "CGImageGetColorSpace");
	CFRelease(colorSpaceReference);

	std::shared_ptr<Image> image;
	if (CFEqual(colorSpaceName, kCGColorSpaceSRGB))
	{
		image = std::make_shared<Image>(ImageType::BGRA, CGImageGetWidth(imageReference), CGImageGetHeight(imageReference),
		                                    (uint8_t*)data, dataLength);
	} else
	{
		LogCritical(LogCategory::IO, "Support for the the color space '%s' is not yet implemented.", colorSpaceName);
	}
	CFRelease(colorSpaceName);

	image->SetAppleMembers(dataReference, imageReference);

	return image;

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
