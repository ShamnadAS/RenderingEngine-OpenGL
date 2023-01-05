#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

//uniform sampler2D texture_diffuse1;
uniform float colorValue;

void main()
{
    float gamma = 2.2;
    //FragColor = pow(texture(texture_diffuse1, TexCoords), vec4( 1.0 / gamma ));
    FragColor = vec4(colorValue, 0.0, 0.0, 1.0);
}