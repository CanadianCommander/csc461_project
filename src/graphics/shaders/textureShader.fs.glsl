R"=====(

#version 330 core
uniform sampler2D textureSampler;
in vec2 textureCoordinate;
out vec4 color;

void main(void)
{
    color = texture(textureSampler, textureCoordinate);
}

)=====";