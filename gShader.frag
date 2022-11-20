#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D texture_diffuse1;
};

uniform Material material;

in VS_OUT {
    vec2 texCoords;
} fs_in;

void main()
{
    FragColor = texture(material.texture_diffuse1, fs_in.texCoords);
}