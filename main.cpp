#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include <string>
#include <stack>
#include <fstream>
#include <streambuf>
#include <sstream>
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/light.h"
#include "graphics/model.h"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"

#include "physics/environment.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"

#include "scene.h"

float test = 1.0;

void proccessInput(double dt);
void launchItem(float dt);

float mixVal = 0.5f;
unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 780;

Scene scene;

Keyboard keyboard;

Gun g;
double dt = 0.0f;
double lastFrame = 0.0f;
bool flashLightOn = true;
float x, y, z;



SphereArray launchObjects;
int main()
{
	glm::vec4 vec(1.0f, 1.0f, 1.0f, 1.0f);

	int success;
	char infoLog[512];
	std::cout << vec.x << " " << vec.y << " " << vec.z << " " << std::endl;
	std::cout << "Hello world" << std::endl;

	scene = Scene(3, 3, "Laberinto", 1024, 780);
	if (scene.init())
	{
		std::cout << "Could not open window " << std::endl;
		glfwTerminate();
		return -1;
	}
	Shader shader("assets/object.vs", "assets/object.fs");
	//Shader lampShader("assets/object.vs", "assets/lamp.fs");
	Shader lampShader("assets/instanced/instanced.vs", "assets/lamp.fs");
	Shader launchShader("assets/instanced/instanced.vs", "assets/object.fs");
	Shader boxShader("assets/instanced/box.vs", "assets/instanced/box.fs");

	shader.activate();
	
	Model m(BoundTypes::AABB, glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.05f),false);
	m.loadModel("assets/textures/models/lotr_troll/scene.gltf");
	g.loadModel("assets/textures/models/m4a1/scene.gltf");
	launchObjects.init();
	Box box;
	box.init();

	DirLight dirLight = { glm::vec3(-0.2f,-1.0f,-1.5f),
			glm::vec4(0.1f,0.1f,0.1f,1.0f),
			glm::vec4(0.4f,0.4f,0.4f,1.0f),
			glm::vec4(0.75f,0.75f,0.75f,1.0f) };

	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.09f;
	float k2 = 0.032f;

	LampArray lamps;
	lamps.init();
	for (unsigned int i = 0; i < 4; i++)
	{
		lamps.lightInstances.push_back(
			{
				pointLightPositions[i],
				k0,k1,k2,
				ambient,diffuse,specular
			});
	}

	SpotLight s = {
		camera.cameraPos,camera.cameraFront,
		glm::cos(glm::radians(12.5f)),glm::cos(glm::radians(20.0f)),1.0f,0.07f,0.032f,
		glm::vec4(0.0f,0.0f,0.0f,1.0f),
		glm::vec4(1.0f,1.0f,1.0f,1.0f),
		glm::vec4(1.0f,1.0f,1.0f,1.0f)};
	x = 0.0f;
	y = 0.0f;
	z = 3.0f;

	while (!scene.shouldClose())
	{
		box.positions.clear();
		box.sizes.clear();
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;
		proccessInput(dt);

		Camera::defaultCamera = camera;
		screen.update();
		
		shader.activate();
		shader.set3Float("viewPos", camera.cameraPos);
		launchShader.activate();
		launchShader.set3Float("viewPos", camera.cameraPos);


		dirLight.direction = 
			glm::rotate(glm::mat4(1.0f), glm::radians(0.05f), glm::vec3(1.0f, 0.0f, 0.0f))*
			glm::vec4(dirLight.direction,1.0f);
		
		shader.activate();
		dirLight.render(shader);
		launchShader.activate();
		dirLight.render(launchShader);

		for (int i = 0; i < 4; i++)
		{

			shader.activate();
			lamps.lightInstances[i].render(shader, i);
			launchShader.activate();
			lamps.lightInstances[i].render(launchShader, i);
		}

		shader.activate();
		shader.setInt("noPointLights", 4);
		launchShader.activate();
		launchShader.setInt("noPointLights", 4);
	
		shader.activate();
		if (flashLightOn)
		{
			s.position = camera.cameraPos;
			s.direction = camera.cameraFront;
			s.render(shader, 0);
			shader.activate();
			shader.setInt("noSpotLights", 4);
			launchShader.activate();
			s.render(launchShader, 0);
			launchShader.setInt("noSpotLights", 4);
		}
		else
		{
			shader.activate();
			shader.setInt("noSpotLights", 0);
			launchShader.activate();
			launchShader.setInt("noSpotLights", 0);
		}
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = Camera::defaultCamera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getZoom()), (float)Scene::scrWidth / (float)Scene::scrHeigt, 0.1f, 1000.0f);
		shader.activate();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		
		m.render(shader, dt,&box,true,true);
		g.render(shader, dt, &box);
		//std::cout << g.rb.pos.x << " - " << g.rb.pos.y << " - " << g.rb.pos.z << std::endl;
		//std::cout << Camera::defaultCamera.cameraFront.x << " - " << Camera::defaultCamera.cameraFront.y << " - " << Camera::defaultCamera.cameraFront.z << std::endl;
		std::stack<int> removeObjects;
		for (int i = 0; i < launchObjects.instances.size(); i++)
		{
			if (glm::length(Camera::defaultCamera.cameraPos - launchObjects.instances[i].pos) > 50.0f)
			{
				removeObjects.push(i);
				continue;
			}
		}
		for (int i = 0; i < removeObjects.size(); i++)
		{
			launchObjects.instances.erase(launchObjects.instances.begin() + removeObjects.top());
			removeObjects.pop();
		}
		if (launchObjects.instances.size() > 0)
		{
			launchShader.activate();
			launchShader.setMat4("view", view);
			launchShader.setMat4("projection", projection);
			launchObjects.render(launchShader, dt, &box);
		}
		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);

		lamps.render(lampShader, dt, &box);
		
		if (box.positions.size() > 0)
		{
			boxShader.activate();
			boxShader.setMat4("view", view);
			boxShader.setMat4("projection", projection);
			//boxShader.set3Float("siz", glm::vec3(10.0,15.0f,20.0f));
			box.render(boxShader);
		}
		screen.newFrame();
	}
	g.cleanup();
	lamps.cleanup();
	box.cleanup();
	launchObjects.cleanup();
	glfwTerminate();
	return 0;
}
void launchItem(float dt)
{
	
	//float x = Camera::defaultCamera.cameraFront.x;
	//float z = Camera::defaultCamera.cameraFront.z;
	//float xz = x / z;
	//float zx = z / x;
	//RigidBody rb(1.0f, Camera::defaultCamera.cameraPos + glm::vec3(/*zx */0.0f, 0.0f,/*xz */ 0.0f));
	RigidBody rb(1.0f, g.rb.pos);
	rb.transferEnergy(10000.0f, Camera::defaultCamera.cameraFront);
	rb.applyAcceleration(Environment::gravitationalAcceleration);
	launchObjects.instances.push_back(rb);
}
void proccessInput(double dt)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		screen.setShouldClose(true);
	}
	if (Keyboard::key(GLFW_KEY_KP_ADD))
	{
		mixVal += 0.1f;
	}
	if (Keyboard::key(GLFW_KEY_KP_SUBTRACT))
	{
		mixVal -= 0.1f;
	}
	if (Keyboard::keyWentDown(GLFW_KEY_L))
	{
		flashLightOn = !flashLightOn;
	}

	if (Keyboard::keyWentDown(GLFW_KEY_F))
	{
		launchItem(dt);
	}
	if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_1))
	{
		launchItem(dt);
	}

	double dx = Mouse::getDX();
	double dy = Mouse::getDY();
	if (dx != 0 || dy != 0)
	{
		camera.updateCameraDirection(dx, dy);
	}
	double scrollDY = Mouse::getScrollDY();
	if (scrollDY != 0)
	{
		camera.updateCameraZoom(scrollDY);

	}
}
