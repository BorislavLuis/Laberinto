#ifndef  GUN_H
#define GUN_H

#include "../model.h"
#include "../../io/camera.h"
#include "../../io/keyboard.h"
#include "../../scene.h"

class Gun : public Model
{
public:
	Gun(unsigned int maxNoInstaces) : Model("m4a1",BoundTypes::AABB, maxNoInstaces,CONST_INSTANCES | NO_TEX) {}

	void render(Shader shader,float dt,Scene* scene, bool setModel = false)
	{
		glm::mat4 model = glm::mat4(1.0f);

		rb.pos = scene->getActiveCamera()->cameraPos + glm::vec3(scene->getActiveCamera()->cameraFront*0.5f)-glm::vec3(scene->getActiveCamera()->cameraUp*0.205f);
		model = glm::translate(model, rb.pos);
		float theta;

		theta = acos(glm::dot(scene->getActiveCamera()->worldUp, scene->getActiveCamera()->cameraFront) /
			glm::length(scene->getActiveCamera()->cameraUp) / glm::length(scene->getActiveCamera()->cameraFront));
		model = glm::rotate(model,  atanf(1) * 2 - theta , scene->getActiveCamera()->cameraRight);

		glm::vec2 front2D = glm::vec2(scene->getActiveCamera()->cameraFront.x, scene->getActiveCamera()->cameraFront.z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), front2D)/glm::length(front2D));
		model = glm::rotate(model, scene->getActiveCamera()->cameraFront.z < 0 ? theta : -theta, scene->getActiveCamera()->worldUp);
		
		model = glm::scale(model, size);
		shader.setMat4("model", model);
		Model::render(shader, dt,scene,setModel);
	}
	void init()
	{
		loadModel("assets/textures/models/m4a1/scene.gltf");
	}
};

#endif // ! GUN_H
