#ifndef LAMP_H
#define LAMP_H

#include "cube.hpp"
#include "../light.h"

class Lamp:public Cube
{
public:
	glm::vec3 lightColor;

	Lamp(unsigned int maxNoInstances, glm::vec3 lightColor = glm::vec3(1.0f))
		:Cube(maxNoInstances,Material::white_rubber)
	{
		id = "lamp";
		this->lightColor = lightColor;
	}


	void render(Shader shader,float dt, Scene* scene,bool setModel=true)
	{
		shader.set3Float("lightColor", lightColor);
		Model::render(shader, dt,scene, setModel);
		//Cube::render(shader,dt, setModel,doRender);
	}
};

#endif
