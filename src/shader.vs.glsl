#version 330 // OpenGL 3.3
uniform mat4 modelViewProjectionMatrix;
in vec3 position;
in vec2 textureCoordinate;
smooth out vec2 fragmentTextureCoordinate;

void main(void)
{
    fragmentTextureCoordinate = textureCoordinate;
    // projection * view * model * vec4
    gl_Position = modelViewProjection * vec4(position, 1.0);
}