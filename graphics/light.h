#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>
#include "shader.h"
#include "../algorithms/bounds.h"
#include "framememory.hpp"


struct PointLight
{
	glm::vec3 position;

	float k0;
	float k1;
	float k2;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	float nearPlane;
	float farPlane;

	glm::mat4 lightSpaceMatrices[6];
	FramebufferObject shadowFBO;

	PointLight();
	PointLight(glm::vec3 position,
		float k0, float k1, float k2,
		glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
		float nearPlane, float farPlane);

	void render(Shader shader,int idx,unsigned int textureIdx);
	
	void updateMatrices();

	static glm::vec3 directions[6];
	static glm::vec3 ups[6];

};

struct DirLight
{
	glm::vec3 direction;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	float farPlane;

	BoundingRegion br;
	glm::mat4 lightSpaceMatrix;
	FramebufferObject shadowFBO;

	DirLight();
	DirLight(glm::vec3 direction,
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		BoundingRegion br);

	void updateMatrices();

	void render(Shader shader,unsigned int textureIdx);
};

struct SpotLight
{
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;

	float cutOff;
	float outerCutOff;

	float k0;
	float k1;
	float k2;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	
	float nearPlane;
	float farPlane;

	glm::mat4 lightSpaceMatrix;

	FramebufferObject shadowFBO;
	SpotLight();
	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 up,
		float cutOff, float outerCutOff,
		float k0, float k1, float k2,
		glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
		float nearPlane, float farPlane);

	void render(Shader shader, int idx, unsigned int textureIdx);
	void updateMatrices();
};
#endif // ! LIGHT_H
