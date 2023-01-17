#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
    vec3 FragPos;
    mat3 TBN;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}fs_in;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_specular1;
};

//uniform sampler2D texture_diffuse;
//uniform sampler2D texture_normal;
uniform Material material;

void main()
{
    //point lighting
    vec3 color = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    //ambient
    vec3 ambient = color * 0.5;
    //diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 normal = texture(material.texture_normal1, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = color * diff;
    //specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    float spec =  pow(max(dot(normal, viewDir), 0.0), 64);
    vec3 specular = vec3(0.2) * spec;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}