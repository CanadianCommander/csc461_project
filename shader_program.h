
#pragma once

#include "../_graphics.h"
#include "shader.h"

#include <set>

namespace Graphics {
namespace Shaders {

using std::string;
using std::map;

class ShaderProgram {
public:
  ShaderProgram(string vertex_shader_filepath, string fragment_shader_filepath);
  GLuint handle() { return _handle; };
  map<string, GLuint> attributes() { return _attribute_handles; }
  map<string, GLuint> uniforms() { return _uniform_handles; }

protected:
  GLuint _handle;
  map<string, GLuint> _attribute_handles;
  map<string, GLuint> _uniform_handles;

private:
  Shader* _vertex_shader;
  Shader* _fragment_shader;

  void create();
  void link();
  void findAttributes();
  void findUniforms();
};
};
};
