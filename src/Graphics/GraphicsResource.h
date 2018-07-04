#pragma once

#include "Graphics.h"

namespace Graphics {

class GraphicsResource
{

public:
	GraphicsResource()
	{
	};

	GraphicsResource(const GraphicsResource &other)
	{
		_handle = other._handle;
	}

	GLuint Handle()
	{ return _handle; }

	GraphicsResource &operator=(const GraphicsResource &other)
	{
		if (this != &other)
		{
			_handle = other._handle;
		}
		return *this;
	}

	bool operator==(const GraphicsResource &other) const
	{
		return _handle == other._handle;
	}

	bool operator!=(const GraphicsResource &other) const
	{
		return _handle != other._handle;
	};

protected:
	GLuint _handle;
};
};
