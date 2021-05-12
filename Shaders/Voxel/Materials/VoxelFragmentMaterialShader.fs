#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

struct DirectedLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
vec3 CalcDirectedLight(DirectedLight light, vec3 viewDir, vec3 norm, vec3 ambColor,vec3 diffColor, vec3 specColor, float shininess);
 
uniform DirectedLight directedlight;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 viewDir, vec3 norm, vec3 ambColor,vec3 diffColor, vec3 specColor, float shininess);


#define MAXN_POINT_LIGHTS 16   
uniform int COUNT_POINT_LIGHTS;
uniform PointLight pointlights[MAXN_POINT_LIGHTS];


struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 viewDir, vec3 norm, vec3 ambColor,vec3 diffColor, vec3 specColor, float shininess);

#define MAXN_SPOTLIGHTS 16 
uniform int COUNT_SPOTLIGHTS;
uniform SpotLight spotLights[MAXN_SPOTLIGHTS];

struct VXLMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;		

    float shininess;
};

uniform VXLMaterial material;


uniform vec3 viewPos;


void main()
{   
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 norm = normalize(Normal);
	
	vec3 result = CalcDirectedLight(directedlight, viewDir, norm, material.ambient, material.diffuse, material.specular, material.shininess);

	for(int i=0;i<COUNT_POINT_LIGHTS;i++)
	{
		result += CalcPointLight(pointlights[i],FragPos,viewDir,norm, material.ambient, material.diffuse, material.specular, material.shininess);
	}

	for(int i=0;i<COUNT_SPOTLIGHTS;i++)
	{
		result += CalcSpotLight(spotLights[i],FragPos,viewDir,norm, material.ambient, material.diffuse, material.specular, material.shininess);
	}
	FragColor =  vec4(result, 1.0);
}

vec3 CalcDirectedLight(DirectedLight light, vec3 viewDir, vec3 norm, vec3 ambColor,vec3 diffColor, vec3 specColor, float shininess)
{
	vec3 lightDir = normalize(-light.direction);
	vec3 ambient = light.ambient * ambColor;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffColor;
	
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0),shininess);	
	vec3 specular = light.specular * spec * specColor;
	
	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 viewDir, vec3 norm, vec3 ambColor,vec3 diffColor, vec3 specColor, float shininess)
{
	vec3 lightDir = normalize(light.position - fragPos);
	
	vec3 ambient = light.ambient * ambColor;

	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = light.diffuse * diff * diffColor;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = light.specular * spec * specColor;

	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return ambient + diffuse + specular;	
}

vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 viewDir, vec3 norm, vec3 ambColor,vec3 diffColor, vec3 specColor, float shininess)
{
	vec3 lightDir = normalize(light.position - fragPos);
	
	vec3 ambient = light.ambient * ambColor;

	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = light.diffuse * diff * diffColor;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = light.specular * spec * specColor;

	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float theta = dot(lightDir,normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	return ambient + diffuse + specular;
}

