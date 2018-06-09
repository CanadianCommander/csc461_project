#pragma once

#include "../GraphicsResource.h"

namespace Graphics {
namespace Shaders {

class ShaderProgram;

class Shader : public GraphicsResource
{
	friend class ShaderProgram;

public:
	Shader(const char* source, GLenum type);

private:
	GLenum _type;

};

}
}
