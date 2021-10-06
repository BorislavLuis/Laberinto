#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/light.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"
#include "io/screen.h"

void proccessInput(double dt);

float mixVal = 0.5f;
unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 780;

Screen screen;

Keyboard keyboard;
Camera camera(glm::vec3(0.0f,0.0f,3.0f));
double deltaTime = 0.0f;
double lastFrame = 0.0f;
bool flashLightOn = true;
float x, y, z;
int main()
{
	glm::vec4 vec(1.0f, 1.0f, 1.0f, 1.0f);

	int success;
	char infoLog[512];
	std::cout << vec.x << " " << vec.y << " " << vec.z << " " << std::endl;
	std::cout << "Hello world" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	if (!screen.init())
	{
		std::cout << "Couldn't create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return -1;
	}

	screen.setParameters();
	glEnable(GL_DEPTH_TEST);


	Shader shader("assets/object.vs", "assets/object.fs");
	Shader lampShader("assets/object.vs", "assets/lamp.fs");

	shader.activate();
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	Cube cubes[10];
	for (unsigned int i = 0; i < 10; i++) {
		cubes[i] = Cube(Material::gold,cubePositions[i], glm::vec3(1.0f));
		cubes[i].init();
	}

	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};
	Lamp lamps[4];
	for (unsigned int i = 0; i < 4; i++) {
		lamps[i] = Lamp(glm::vec3(1.0f),
			glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f),
			1.0f, 0.07f, 0.032f,
			pointLightPositions[i], glm::vec3(0.25f));
		lamps[i].init();
	}
	Cube cube(Material::mix(Material::gold, Material::emerald),glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.75f));
	cube.init();
	

	DirLight dirLight = { glm::vec3(-0.2f,-1.0f,-1.5f),glm::vec3(0.1f),glm::vec3(0.4f),glm::vec3(0.75f) };

	Lamp lamp(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, 0.05f, 0.025f,  glm::vec3(-3.0f, -0.5f, -1.0f), glm::vec3(0.5f));
	lamp.init();

	SpotLight s = {
		camera.cameraPos,camera.cameraFront,
		glm::cos(glm::radians(12.5f)),glm::cos(glm::radians(20.0f)),1.0f,0.05f,0.025f,
		glm::vec3(0.0f),glm::vec3(1.0f),glm::vec3(1.0f)
			};
	x = 0.0f;
	y = 0.0f;
	z = 3.0f;

	while (!screen.shouldClose())
	{
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		proccessInput(deltaTime);

		screen.update();
		shader.activate();
		shader.set3Float("viewPos", camera.cameraPos);
		dirLight.direction = 
			glm::rotate(glm::mat4(1.0f), glm::radians(0.05f), glm::vec3(1.0f, 0.0f, 0.0f))*
			glm::vec4(dirLight.direction,1.0f);

		dirLight.render(shader);
		for (int i = 0; i < 4; i++)
		{
			lamps[i].pointLight.render(shader, i);
		}
		shader.setInt("noPointLights", 4);
		//lamp.pointLight.render(shader);
		if (flashLightOn)
		{
			s.position = camera.cameraPos;
			s.direction = camera.cameraFront;
			s.render(shader, 0);
			shader.setInt("noSpotLights", 4);
		}
		else
		{
			shader.setInt("noSpotLights", 0);
		}
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		
		for (int i = 0; i < 10; i++)
		{
			cubes[i].render(shader);
		}

		cube.render(shader);
		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);

		for (int i = 0; i < 4; i++)
		{
			lamps[i].render(lampShader);
		}
		
		screen.newFrame();
	}
	for (int i = 0; i < 10; i++)
	{
		cubes[i].cleanup();
	}
	for (int i = 0; i < 4; i++)
	{
		lamps[i].cleanup();
	}
	
	glfwTerminate();
	return 0;
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
	if (Keyboard::key(GLFW_KEY_W))
	{
		camera.updateCameraPos(CameraDirection::FORWARD, dt);
	}
	if (Keyboard::key(GLFW_KEY_S))
	{
		camera.updateCameraPos(CameraDirection::BACKWARD, dt);
	}
	if (Keyboard::key(GLFW_KEY_D))
	{
		camera.updateCameraPos(CameraDirection::RIGHT, dt);
	}
	if (Keyboard::key(GLFW_KEY_A))
	{
		camera.updateCameraPos(CameraDirection::LEFT, dt);
	}
	if (Keyboard::key(GLFW_KEY_SPACE))
	{
		camera.updateCameraPos(CameraDirection::UP, dt);
	}
	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
	{
		camera.updateCameraPos(CameraDirection::DOWN, dt);
	}
	if (Keyboard::keyWentDown(GLFW_KEY_L))
	{
		flashLightOn = !flashLightOn;
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
