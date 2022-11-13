#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform DirLight dirLight;
uniform vec3 viewPos;

uniform samplerCube skyBox;

//function prototype
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos); //fragment to camera
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    FragColor = texture(material.texture_diffuse1, TexCoords);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
   vec3 lightDir = normalize(vec3(light.direction));

   //ambient
   vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

   //diffuse
   float diff = max(dot(normal, -lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;  

   //specular	
   vec3 reflectDir = reflect(lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   //vec3 specular = spec * light.specular * texture(material.texture_specular1, TexCoords).rgb;
   vec3 specular = spec * light.specular;

   return ambient + diffuse;
}
