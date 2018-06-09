#pragma once

#include "GraphicsDebug.h"

namespace Graphics {

class GraphicsResource
{

public:
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
	GraphicsResource()
	{
		_handle = 0;
	};

	GLuint _handle{};
};
};
