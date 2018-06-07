#pragma once

#ifdef __APPLE__

#include <CoreGraphics/CoreGraphics.h>

#elif __linux__
#include <gdk/gdk.h>
#endif

#include "../Graphics/Texture.h"

using Graphics::Texture;

namespace IO {

class ScreenCapture
{
public:
	ScreenCapture();

	std::shared_ptr<Texture> GetScreenFrameBuffer();

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

