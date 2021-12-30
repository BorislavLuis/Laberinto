#version 330 core

struct Material
{
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform sampler2D diffuse0;
uniform sampler2D specular0;

#define MAX_POINT_LIGHTS 20
struct PointLight
{
	vec3 position;

	float k0;
	float k1;
	float k2;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float farPlane;
	samplerCube depthBuffer;
};
uniform PointLight pointLight[MAX_POINT_LIGHTS];
uniform int noPointLights;

struct DirLight
{
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float farPlane;

	sampler2D depthBuffer;
	mat4 lightSpaceMatrix;
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

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float nearPlane;
	float farPlane;

	sampler2D depthBuffer;
	mat4 lightSpaceMatrix;
};
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform Material material;
uniform int noTex;
uniform vec3 viewPos;
uniform bool useBlinn;
uniform bool useGamma;

float calcDirLightShadow(vec3 norm,vec3 viewVec,vec3 lightDir);
float calcSpotLightShadow(int idx,vec3 norm,vec3 viewVec,vec3 lightDir);
float calcPointLightShadow(int idx,vec3 norm,vec3 viewVec,vec3 lightDir);
vec4 calcDirLight(vec3 norm,vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap);
vec4 calcPointLight(int idx,vec3 norm,vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap);
vec4 calcSpotLight(int idx,vec3 norm,vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap);

#define NUM_SAMPLE 20
#define NUM_SAMPLEF 20.0
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  0, 0 )
);   

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewVec = viewPos - FragPos;
	vec3 viewDir = normalize(viewVec);

	vec4 diffMap;
	vec4 specMap;

	if(noTex == 1)
	{
		diffMap = material.diffuse;
		specMap = material.specular;
	}
	else
	{
		diffMap = texture(diffuse0,TexCoord);
		specMap = texture(specular0,TexCoord);
	}
	vec4 result = vec4(0.0,0.0,0.0,1.0);
	
	result = calcDirLight(norm,viewVec,viewDir,diffMap,specMap);

	for(int i =0; i < noPointLights;i++)
	{
		result+= calcPointLight(i,norm,viewVec,viewDir,diffMap,specMap);
	}
	for(int i = 0; i < noSpotLights;i++)
	{
		result+=calcSpotLight(i,norm,viewVec,viewDir,diffMap,specMap);
	}
	if(useGamma)
	{
		float gamma = 2.2;
		result.rgb = pow(result.rgb,vec3(1.0/gamma));
	}

	float near = 0.1;
	float far = 1000.0;
	float z = gl_FragCoord.z * 2.0 - 1.0; // transform to NDC [0, 1] => [-1, 1]
	float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // take inverse of the projection matrix (perspective)
	float factor = (near + linearDepth) / (near - far); // convert back to [0, 1]

	result.rgb *= 1 - factor;
	FragColor = result;

}
float calcDirLightShadow(vec3 norm,vec3 viewVec,vec3 lightDir)
{
	vec4 fragPosLightSpace = dirLight.lightSpaceMatrix*vec4(FragPos,1.0);

	vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;
	
	if(projCoords.z > 1.0)
	{
		return 0.0;
	}

	//float closestDepth = texture(dirLight.depthBuffer,projCoords.xy).r;

	float currentDepth = projCoords.z;

	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias *(1.0 - dot(norm,lightDir)),minBias);

	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(dirLight.depthBuffer,0);
	float viewDist = length(viewVec);
	float diskRadius = (1.0 +(viewDist/dirLight.farPlane))/30.0;

	for(int y = -1;y <= 1;y++)
	{
		for(int x = -1; x <= 1;x++)
		{
			float pcfDepth = texture(dirLight.depthBuffer,
				 projCoords.xy + vec2(x,y)*texelSize* diskRadius).r;
			shadowSum += currentDepth-bias > pcfDepth ? 1.0:0.0;
		}
	}

	return shadowSum/9.0;
}
vec4 calcDirLight(vec3 norm,vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap)
{
	vec4 ambient = dirLight.ambient*diffMap;
	
	vec3 lightDir = normalize(-dirLight.direction - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec4 diffuse = dirLight.diffuse * (diff * diffMap);

	vec4 specular = vec4(0.0,0.0,0.0,1.0);
	if(diff>0)
	{
		float dotProd = 0.0;
		if(useBlinn)
		{
			vec3 halfwayDir = normalize(lightDir+viewDir);
			dotProd = dot(norm,halfwayDir);
		}
		else 
		{
			vec3 reflectionDir = reflect(-lightDir,norm);
			dotProd = dot(viewDir,reflectionDir);
		}
			
		float spec = pow(max(dotProd,0.0),material.shininess*128);
		specular = dirLight.specular * (spec*specMap);
	}

	float shadow = calcDirLightShadow(norm,viewVec,lightDir);

	return vec4(ambient+(1.0-shadow)*diffuse+specular);
}

float calcSpotLightShadow(int idx,vec3 norm,vec3 viewVec,vec3 lightDir)
{
	vec4 fragPosLightSpace = spotLight[idx].lightSpaceMatrix * vec4(FragPos,1.0);

	vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

	projCoords = projCoords*0.5 + 0.5;

	if(projCoords.z > 1.0)
	{
		return 0.0;
	}

	//float closestDepth = texture(spotLight[idx].depthBuffer,projCoords.xy).r;
	//float z = closestDepth*2.0 - 1.0;
	//closestDepth = (2.0 * spotLight[idx].nearPlane * spotLight[idx].farPlane)/
		//(spotLight[idx].farPlane + spotLight[idx].nearPlane 
		//- z * (spotLight[idx].farPlane - spotLight[idx].nearPlane)); 
	//closestDepth /= spotLight[idx].farPlane;

	float currentDepth = projCoords.z;
	
	float maxBias = 0.05;
	float minBias = 0.005;
	float bias = max(maxBias *(1.0 - dot(norm,lightDir)),minBias);

	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(spotLight[idx].depthBuffer,0);
	float viewDist = length(viewVec);
	float diskRadius = (1.0 +(viewDist/spotLight[idx].farPlane))/30.0;
	for(int y = -1;y <= 1;y++)
	{
		for(int x = -1; x <= 1;x++)
		{
			float pcfDepth = texture(spotLight[idx].depthBuffer,
				 projCoords.xy + vec2(x,y)*texelSize * diskRadius).r;
			
			pcfDepth *= spotLight[idx].farPlane;
			shadowSum += currentDepth-bias > pcfDepth ? 1.0:0.0;

		}
	}

	return shadowSum/9.0;
}

float calcPointLightShadow(int idx,vec3 norm,vec3 viewVec,vec3 lightDir)
{
	vec3 lightToFrag = FragPos - pointLight[idx].position;

	float closestDepth = texture(pointLight[idx].depthBuffer,lightToFrag).r;

	closestDepth*=pointLight[idx].farPlane;

	float currentDepth = length(lightToFrag);

	float minBias = 0.005;
	float maxBias = 0.05;
	float bias = max(maxBias *(1.0 - dot(norm,lightDir)),minBias);

	float shadow = 0.0;
	float viewDist = length(viewVec);
	float diskRadius = (1.0 +(viewDist/pointLight[idx].farPlane))/30.0;

	for(int i = 0;i < NUM_SAMPLE;i++ )
	{
		float pcfDepth = texture(pointLight[idx].depthBuffer,
				lightToFrag + sampleOffsetDirections[i]*diskRadius).r;
		pcfDepth *= pointLight[idx].farPlane;
		
		if(currentDepth - bias > pcfDepth)
		{
			shadow+=1.0;
		}
	}
	shadow /= NUM_SAMPLEF;
	return shadow;

	//return currentDepth - bias > closestDepth ? 1.0:0.0;
}

vec4 calcPointLight(int idx,vec3 norm,vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap)
{
	vec4 ambient = pointLight[idx].ambient*diffMap;
	
	vec3 lightDir = normalize(pointLight[idx].position - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec4 diffuse = pointLight[idx].diffuse * (diff * diffMap);

	vec4 specular = vec4(0.0,0.0,0.0,1.0);
	if(diff>0)
	{
		float dotProd = 0.0;
		if(useBlinn)
		{
			vec3 halfwayDir = normalize(lightDir+viewDir);
			dotProd = dot(norm,halfwayDir);
		}
		else 
		{
			vec3 reflectionDir = reflect(-lightDir,norm);
			dotProd = dot(viewDir,reflectionDir);
		}
			
		float spec = pow(max(dotProd,0.0),material.shininess*128);
		specular = dirLight.specular * (spec*specMap);
	}

	float dist = length(pointLight[idx].position - FragPos);
	float attenuation = 1.0f/ (pointLight[idx].k0 + pointLight[idx].k1*dist + pointLight[idx].k2*(dist*dist));

	ambient*=attenuation;
	diffuse*=attenuation;
	specular*=attenuation;

	float shadow = calcPointLightShadow(idx,norm,viewVec,lightDir);

	return vec4(ambient+(1.0 - shadow)*(diffuse+specular))*attenuation;
}

vec4 calcSpotLight(int idx,vec3 norm,vec3 viewVec,vec3 viewDir,vec4 diffMap,vec4 specMap)
{
	vec3 lightDir =normalize(spotLight[idx].position -FragPos);
	float theta = dot(lightDir,normalize(-spotLight[idx].direction));

	vec4 ambient = spotLight[idx].ambient *diffMap;

	if(theta > spotLight[idx].outerCutOff)
	{
		float diff = max(dot(norm,lightDir),0.0);
		vec4 diffuse = spotLight[idx].diffuse * (diff*diffMap);
		
		vec4 specular = vec4(0.0,0.0,0.0,1.0);
		if(diff>0)
		{
			float dotProd = 0.0;
			if(useBlinn)
			{
				vec3 halfwayDir = normalize(lightDir+viewDir);
				dotProd = dot(norm,halfwayDir);
			}
			else 
			{
				vec3 reflectionDir = reflect(-lightDir,norm);
				dotProd = dot(viewDir,reflectionDir);
			}
			
			float spec = pow(max(dotProd,0.0),material.shininess*128);
			specular = dirLight.specular * (spec*specMap);
		}
		
		float intesity = (theta - spotLight[idx].outerCutOff)/(spotLight[idx].cutOff -spotLight[idx].outerCutOff);
		intesity = clamp(intesity,0.0,1.0);
		diffuse *= intesity;
		specular *= intesity;

		float dist = length(spotLight[idx].position - FragPos);
		float attenuation = 1.0f/ (spotLight[idx].k0 + spotLight[idx].k1*dist + spotLight[idx].k2*(dist*dist));

		float shadow = calcSpotLightShadow(idx,norm,viewVec,lightDir);

		return vec4(ambient+(1.0-shadow)*diffuse+specular)*attenuation;
	}
	else
	{
		return ambient;
	}

}

