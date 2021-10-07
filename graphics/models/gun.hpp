#ifndef  GUN_H
#define GUN_H

#include "../model.h"
#include "../../io/camera.h"
#include "../../io/keyboard.h"
class Gun : public Model
{
public:
	Gun() : Model(glm::vec3(0.0f), glm::vec3(0.05f), true) {}

	void render(Shader shader, bool setModel = false)
	{
		glm::mat4 model = glm::mat4(1.0f);

		pos = Camera::defaultCamera.cameraPos + glm::vec3(Camera::defaultCamera.cameraFront*8.0f)-glm::vec3(Camera::defaultCamera.cameraUp*3.15f);
		model = glm::translate(model, pos);
		float theta;

		theta = acos(glm::dot(Camera::defaultCamera.worldUp, Camera::defaultCamera.cameraFront) /
			glm::length(Camera::defaultCamera.cameraUp) / glm::length(Camera::defaultCamera.cameraFront));
		model = glm::rotate(model,  atanf(1) * 2 - theta , Camera::defaultCamera.cameraRight);

		glm::vec2 front2D = glm::vec2(Camera::defaultCamera.cameraFront.x, Camera::defaultCamera.cameraFront.z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), front2D)/glm::length(front2D));
		model = glm::rotate(model, Camera::defaultCamera.cameraFront.z < 0 ? theta : -theta, Camera::defaultCamera.worldUp);

		model = glm::scale(model, size);
		shader.setMat4("model", model);
		Model::render(shader, setModel);
	}
};

#endif // ! GUN_H
