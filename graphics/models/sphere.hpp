#ifndef SPHERE_H
#define SPHERE_H

#include "../model.h"

class Sphere : public Model
{
public:
	Sphere(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f))
			:Model(pos,size,true){}

	void init() 
	{ 
		loadModel("assets/textures/models/sphere/scene.gltf"); 
	}
};
#endif