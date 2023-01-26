#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
}fs_in;

struct Material
{
    sampler2D texture_diffuse;
};

uniform sampler2D shadowMap;
uniform Material material;
uniform vec3 viewPos;
uniform vec3 lightPos;

float shadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
  
    float currentDepth = projCoords.z;

    float bias = max( 0.005 * dot(normal, lightDir), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 0.5 : 0.0;
        }
    }

    shadow /= 9.0;
    if(projCoords.z > 1.0)
    shadow = 0.0;
    return shadow;
}

void main()
{
    //point lighting
    vec3 color = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
    //ambient
    vec3 ambient = color * 0.5;
    //diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = color * diff;
    //specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec =  pow(max(dot(reflectDir, viewDir), 0.0), 32);
    vec3 specular = vec3(0.2) * spec;

    float shadow = shadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    vec3 result = ambient + ( 1 - shadow ) * (diffuse + specular);
    FragColor = vec4(result, 1.0);
}