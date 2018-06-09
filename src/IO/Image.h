#pragma once

#include <OpenGL/OpenGL.h>

#if __APPLE__

#include <CoreGraphics/CoreGraphics.h>
#include "../Debug.h"

#endif

namespace IO {

class Image
{

public:
	Image(size_t width, size_t height, uint8_t* data, uint32_t dataLength)
	{
		_width = width;
		_height = height;
		_data = data;
		_dataLength = dataLength;
	}

	virtual ~Image()
	{
#if __APPLE__
		CFRelease(_dataReference);
		CGImageRelease(_imageReference);
#endif
	}

	virtual uint8_t* GetData()
	{ return _data; }

	virtual size_t GetWidth()
	{ return _width; };

	virtual size_t GetHeight()
	{ return _height; }

	virtual GLint GetGLInternalFormat() = 0;
	virtual GLenum GetGLFormat() = 0;

#ifdef __APPLE__

	void SetAppleMembers(CFDataRef dataReference, CGImageRef imageReference)
	{
		_dataReference = dataReference;
		_imageReference = imageReference;
	}

#endif

private:
	uint8_t* _data;
	uint32_t _dataLength;
	size_t _width;
	size_t _height;

#ifdef __APPLE__
	CFDataRef _dataReference;
	CGImageRef _imageReference;
#endif

};

}
