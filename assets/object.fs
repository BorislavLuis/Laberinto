#version 330 core

struct Material
{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
#define MAX_POINT_LIGHTS 20
struct PointLight
{
	vec3 position;

	float k0;
	float k1;
	float k2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform PointLight pointLight[MAX_POINT_LIGHTS];
uniform int noPointLights;

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

#define MAX_SPOT_LIGHTS 5
struct SpotLight
{
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	float k0;
	float k1;
	float k2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform Material material;
uniform vec3 viewPos;

vec3 calcDirLight(vec3 norm,vec3 viewDir,vec3 diffMap,vec3 specMap);
vec3 calcPointLight(int idx,vec3 norm,vec3 viewDir,vec3 diffMap,vec3 specMap);
vec3 calcSpotLight(int idx,vec3 norm,vec3 viewDir,vec3 diffMap,vec3 specMap);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos-FragPos);
	vec3 diffMap = vec3(texture(material.diffuse,TexCoord));
	vec3 specMap = vec3(texture(material.specular,TexCoord));

	vec3 result;
	
	result = calcDirLight(norm,viewDir,diffMap,specMap);

	for(int i =0; i < noPointLights;i++)
	{
		result+= calcPointLight(i,norm,viewDir,diffMap,specMap);
	}
	for(int i = 0; i < noSpotLights;i++)
	{
		result+=calcSpotLight(i,norm,viewDir,diffMap,specMap);
	}
	FragColor = vec4(result,1.0f);

}

vec3 calcDirLight(vec3 norm,vec3 viewDir,vec3 diffMap,vec3 specMap)
{
	vec3 ambient = dirLight.ambient*diffMap;
	
	vec3 lightDir = normalize(-dirLight.direction - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = dirLight.diffuse * (diff * diffMap);


	vec3 reflectionDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectionDir),0.0),material.shininess*128);
	vec3 specular = dirLight.specular * (spec*specMap);

	return vec3(ambient+diffuse+specular);
}

vec3 calcPointLight(int idx,vec3 norm,vec3 viewDir,vec3 diffMap,vec3 specMap)
{
	vec3 ambient = pointLight[idx].ambient*diffMap;
	
	vec3 lightDir = normalize(pointLight[idx].position - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = pointLight[idx].diffuse * (diff * diffMap);


	vec3 reflectionDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectionDir),0.0),material.shininess*128);
	vec3 specular = pointLight[idx].specular * (spec*specMap);

	float dist = length(pointLight[idx].position - FragPos);
	float attenuation = 1.0f/ (pointLight[idx].k0 + pointLight[idx].k1*dist + pointLight[idx].k2*(dist*dist));


	return vec3(ambient+diffuse+specular)*attenuation;
}

vec3 calcSpotLight(int idx,vec3 norm,vec3 viewDir,vec3 diffMap,vec3 specMap)
{
	vec3 lightDir =normalize(spotLight[idx].position -FragPos);
	float theta = dot(lightDir,normalize(-spotLight[idx].direction));

	vec3 ambient = spotLight[idx].ambient *diffMap;

	if(theta > spotLight[idx].outerCutOff)
	{
		float diff = max(dot(norm,lightDir),0.0);
		vec3 diffuse = spotLight[idx].diffuse * (diff*diffMap);
		
		vec3 reflectionDir = reflect(-lightDir,norm);
		float spec = pow(max(dot(viewDir,reflectionDir),0.0),material.shininess * 128);
		vec3 specular = spotLight[idx].specular*(spec*specMap);
		
		float intesity = (theta - spotLight[idx].outerCutOff)/(spotLight[idx].cutOff -spotLight[idx].outerCutOff);
		intesity = clamp(intesity,0.0,1.0);
		diffuse *= intesity;
		specular *= intesity;

		float dist = length(spotLight[idx].position - FragPos);
		float attenuation = 1.0f/ (spotLight[idx].k0 + spotLight[idx].k1*dist + spotLight[idx].k2*(dist*dist));

		return vec3(ambient+diffuse+specular)*attenuation;
	}
	else
	{
		return ambient;
	}

}

