#pragma once

#include <string>
#include <map>
#include "Shader.h"

namespace Graphics {
namespace Shaders {

using std::string;
using std::map;

class ShaderProgram : public GraphicsResource
{

public:
	ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

	void Use();

	map<string, GLuint> Attributes()
	{ return _attributes; }

	map<string, GLuint> Uniforms()
	{ return _uniforms; }

protected:
	GLuint _handle;
	map<string, GLuint> _attributes;
	map<string, GLuint> _uniforms;

private:
	Shader* _vertexShader;
	Shader* _fragmentShader;

	void Create();
	void Link();
	void FindAttributes();
	void FindUniforms();
};

};
};
