#ifndef LAMP_H
#define LAMP_H

#include "cube.hpp"
#include "../light.h"
class Lamp:public Cube
{
public:
	glm::vec3 lightColor;

	PointLight pointLight;
	Lamp(){}
	Lamp(glm::vec3 lightColor,
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular,
		float k0,
		float k1,
		float k2,
		glm::vec3 pos,
		glm::vec3 size)
		: lightColor(lightColor),
		pointLight({pos,k0,k1,k2,ambient,diffuse,specular}),
		Cube(Material::white_plastic,pos,size){}

	void render(Shader shader)
	{
		shader.set3Float("lightColor", lightColor);
		Cube::render(shader);
	}
};

#endif
