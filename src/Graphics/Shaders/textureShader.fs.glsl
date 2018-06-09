R"=====(

#version 330 core
uniform sampler2D textureSampler;
in vec2 textureCoordinate;
out vec4 color;

const float SRGB_GAMMA = 1.0 / 2.2;
const float SRGB_INVERSE_GAMMA = 2.2;

vec3 rgb_to_srgb_fast(vec3 rgb) {
    return pow(rgb, vec3(SRGB_GAMMA));
}

void main(void)
{
    vec4 texels = texture(textureSampler, textureCoordinate);
    vec3 rgb = rgb_to_srgb_fast(texels.rgb);
    color = vec4(rgb, 1);
}

)=====";