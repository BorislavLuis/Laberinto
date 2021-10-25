#ifndef SPHERE_H
#define SPHERE_H

#include "../model.h"
#include "modelarray.hpp"

class Sphere : public Model
{
public:
	Sphere(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f))
			:Model(pos,size,true)
	{
		//rb.mass = 1.0f;
	}

	void init() 
	{ 
		loadModel("assets/textures/models/sphere/scene.gltf"); 
	}
};

class SphereArray :public ModelArray<Sphere>
{
public:
	void init()
	{
		model = Sphere(glm::vec3(0.0f), glm::vec3(0.25f));
		ModelArray::init();
	}
};
#endif