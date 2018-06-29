#pragma once

#include "Image.h"

//GTK 2 forward defs
#ifdef __linux__
typedef struct _GdkWindow GdkWindow;
typedef struct _GdkPixbuf GdkPixbuf;
#endif


namespace IO {

class ScreenCapture
{
public:
	ScreenCapture();

	std::shared_ptr<Image> GetScreenFrameBuffer();

protected:
	void QueryDisplayInformation();

private:
#ifdef __linux__
	GdkWindow * _desktop;
#elif __APPLE__
	uint32_t _desktop;
#endif
	int _desktopX;
	int _desktopY;
	int _desktopWidth;
	int _desktopHeight;
};

}
