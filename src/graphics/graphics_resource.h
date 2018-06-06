#pragma once

#include "graphics.h"

namespace Graphics {

class GraphicsResource
{

public:
	GLuint Handle()
	{ return _handle; }

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