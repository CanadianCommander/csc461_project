R"=====(

#version 330 core

in vec2 vertexPosition;
in vec2 vertexTextureCoordinate;
out vec2 textureCoordinate;

void main(void)
{
    textureCoordinate = vertexTextureCoordinate;
    gl_Position = vec4(vertexPosition, 0, 1);
}

)=====";