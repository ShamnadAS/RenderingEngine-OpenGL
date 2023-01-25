#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
}fs_in;

struct Material
{
    sampler2D texture_diffuse;
};

uniform samplerCube depthMap;
uniform Material material;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform int flipNormal;
uniform float far_plane;

float ShadowCalculation(vec3 fragPos)
{
vec3 fragToLight = fragPos - lightPos;
float currentDepth = length(fragToLight);

vec3 sampleOffsetDirections[20] = vec3[]
(
vec3( 1, 1, 1), vec3( 1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
vec3( 1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
vec3( 1, 1, 0), vec3( 1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
vec3( 1, 0, 1), vec3(-1, 0, 1), vec3( 1, 0, -1), vec3(-1, 0, -1),
vec3( 0, 1, 1), vec3( 0, -1, 1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float shadow = 0.0;
float bias = 0.15;
int samples = 20;
float viewDistance = length(viewPos - fragPos);
float diskRadius = 0.05;
for(int i = 0; i < samples; ++i)
{
float closestDepth = texture(depthMap, fragToLight +
sampleOffsetDirections[i] * diskRadius).r;
closestDepth *= far_plane; // undo mapping [0;1]
if(currentDepth - bias > closestDepth)
shadow += 1.0;
}
shadow /= float(samples);
return shadow;
}

void main()
{
    //point lighting
    vec3 color = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
    //ambient
    vec3 ambient = color * 0.2;
    //diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    if(flipNormal == 1)
    {
        normal = -normal;
    }
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = color * diff;
    //specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec =  pow(max(dot(reflectDir, viewDir), 0.0), 32);
    vec3 specular = vec3(0.2) * spec;

    float shadow = ShadowCalculation(fs_in.FragPos);
    vec3 result = ambient + ( 1 - shadow ) * (diffuse + specular);
    //vec3 result = ambient + diffuse + specular;
    vec3 fragToLight = fs_in.FragPos - lightPos;
    float closestDepth = texture(depthMap, fragToLight).r;
    FragColor = vec4(vec3(closestDepth), 1.0);
    //FragColor = vec4(result, 1.0);
}