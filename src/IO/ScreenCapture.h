#pragma once

#include <memory>
#include "Image.h"

#ifdef __APPLE__
#elif __linux__
  typedef struct _GdkWindow GdkWindow;
  typedef struct _GdkPixbuf GdkPixbuf;
#endif


namespace IO {

class ScreenCapture
{
public:
	ScreenCapture();
	std::shared_ptr<Image> GetScreenFrameBuffer();

private:
#ifdef __linux__
	GdkWindow * _displayHandle;
#elif __APPLE__
	uint32_t _displayHandle;

#endif
};

}
