#version 330 // OpenGL 3.3
uniform sampler2D textureSampler;
smooth in vec2 textureCoordinate;
out vec4 fragColor;

void main(void)
{
    fragColor = texture(textureSampler, fragmentTextureCoordinate);
}