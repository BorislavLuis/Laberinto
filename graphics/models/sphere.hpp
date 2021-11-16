#ifndef SPHERE_H
#define SPHERE_H

#include "../model.h"

class Sphere : public Model
{
public:
	Sphere(unsigned int maxNoInstances)
		:Model("sphere", BoundTypes::SPHERE, maxNoInstances, NO_TEX | DYNAMIC)
	{}

	void init() 
	{ 
		loadModel("assets/textures/models/sphere/scene.gltf"); 
	}
};

#endif