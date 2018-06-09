#include "ScreenCapture.h"
#include "ImageSRGB.h"
#include "../Debug.h"

#ifdef __APPLE__

#include "AVCaptureSession.h"
using AVCaptureSession = Interop::AVFoundation::AVCaptureSession;

#elif __linux__
#include <gdk/gdk.h>
#endif

namespace IO {

ScreenCapture::ScreenCapture()
{
#ifdef __APPLE__
	_displayHandle = CGMainDisplayID();

	auto x = AVCaptureSession();

#else
	if(!gdk_init_check(NULL, NULL)){
		  throw std::runtime_error("Gdk initialization error!");
	}

	//fetch system default screen
	mDesktop = gdk_get_default_root_window();
	if(!mDesktop){
		  throw std::runtime_error("Could not fetch root window!");
	}

	_screenBoundsWidth = gdk_window_get_width(mDesktop);
	_screenBoundsHeight= gdk_window_get_height(mDesktop);
	gdk_window_get_origin(_displayHandle, &_desktopX, &_screenBoundsY);
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
		image = std::make_shared<ImageSRGB>(CGImageGetWidth(imageReference), CGImageGetHeight(imageReference),
		                                    (uint8_t*)data, dataLength);
	} else
	{
		LogCritical(LogCategory::IO, "Support for the the color space '%s' is not yet implemented.", colorSpaceName);
	}
	CFRelease(colorSpaceName);

	image->SetAppleMembers(dataReference, imageReference);

	return image;

#elif __linux__
	GdkPixbuf * pix = gdk_pixbuf_get_from_window(_displayHandle, 0, 0, _screenBoundsWidth, _screenBoundsHeight);
	  if(pix){
		return std::make_shared<ImageRGB>((uint8_t *)gdk_pixbuf_read_pixels(pix),
				  gdk_pixbuf_get_width(pix), gdk_pixbuf_get_height(pix), gdk_pixbuf_get_byte_lenght(pix));

	  }
#endif
}
}
