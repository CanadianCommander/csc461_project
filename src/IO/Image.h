#pragma once

#include <vector>
#include <memory>

#if __APPLE__

#include <CoreGraphics/CoreGraphics.h>

#endif

#include "../Graphics/Graphics.h"

namespace IO {

enum class ImageType
{
	RGB = 0,
	BGRA = 1,
	Y = 2,
	U = 3,
	V = 4
};

class Image
{
public:
	Image(ImageType type, uint32_t width, uint32_t height, void* data, uint32_t dataLength)
	{
		_type = type;
		_width = width;
		_height = height;
		_data = data;
		_dataLength = dataLength;

#if __APPLE__
		_dataReference = nullptr;
		_imageReference = nullptr;
#endif
	}

	virtual ~Image()
	{
#if __APPLE__
		if (_dataReference != nullptr) CFRelease(_dataReference);
		if (_imageReference != nullptr) CGImageRelease(_imageReference);
#endif
	}

	ImageType GetType() const
	{ return _type; }

	void* GetData() const
	{ return _data; }

	uint32_t GetDataLength() const
	{ return _dataLength; }

	uint32_t GetWidth() const
	{ return _width; };

	uint32_t GetHeight()
	{ return _height; }

#ifdef __APPLE__

	void SetAppleMembers(CFDataRef dataReference, CGImageRef imageReference)
	{
		_dataReference = dataReference;
		_imageReference = imageReference;
	}

#endif

private:
	ImageType _type;
	void* _data;
	uint32_t _dataLength;
	uint32_t _width;
	uint32_t _height;

#ifdef __APPLE__
	CFDataRef _dataReference;
	CGImageRef _imageReference;
#endif

};
}
