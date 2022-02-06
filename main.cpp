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
#include "graphics/cubemap.h"

#include "graphics/model.h"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"
#include "graphics/models/plane.hpp"
#include "graphics/models/brickwall.hpp"

#include "graphics/framememory.hpp"
#include "graphics/uniformmemory.hpp"

#include "physics/environment.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"

#include "algorithms/states.hpp"

#include "scene.h"

#include <ft2build.h>

float test = 1.0;

void proccessInput(double dt);
void renderScene(Shader shader);
void launchItem(float dt);

float mixVal = 0.5f;
unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 780;

Scene scene;
Camera camera;
Keyboard keyboard;


//Gun g(1);
//Model g("m4a1", BoundTypes::AABB, 1,CONST_INSTANCES);
//Model troll("troll", BoundTypes::AABB, 1, CONST_INSTANCES);
double dt = 0.0f;
double lastFrame = 0.0f;

//Sphere sphere(10);
//Cube cube(21);
Lamp lamp(4);
Brickwall wall;
std::string Shader::defaultDirectory = "assets/shaders";

int main()
{
	glm::vec4 vec(1.0f, 1.0f, 1.0f, 1.0f);

	int success;
	char infoLog[512];
	std::cout << vec.x << " " << vec.y << " " << vec.z << " " << std::endl;
	std::cout << "Hello world" << std::endl;

	scene = Scene(3, 3, "Laberinto", 1920, 1080);
	if (!scene.init())
	{
		std::cout << "Could not open window " << std::endl;
		glfwTerminate();
		return -1;
	}
	scene.cameras.push_back(&camera);
	scene.activeCamera = 0;
	Shader::loadIntoDefault("defaultHead.gh");
	//Shader shader("assets/object.vs", "assets/object.fs");
	//Shader gunShader(false,"object.vs", "object.fs");
	//Shader lampShader("assets/shaders/instanced/instanced.vs", "assets/shaders/lamp.fs");
	Shader shader(true,"instanced/instanced.vs", "object.fs");
	Shader boxShader(false,"instanced/box.vs", "instanced/box.fs");
	Shader textShader(false,"text.vs", "text.fs");
	//Shader skyboxShader(false,"skybox.vs", "skybox.fs");
	//Shader outlineShader("assets/shaders/outline.vs", "assets/shaders/outline.fs");
	//Shader bufferShader("assets/shaders/buffer.vs", "assets/shaders/buffer.fs");
	Shader dirShadowShader(false,"shadows/dirSpotShadow.vs",
		"shadows/dirShadow.fs");
	Shader spotShadowShader(false,"shadows/dirSpotShadow.vs",
		"shadows/pointSpotShadow.fs");
	Shader pointShadowShader(false,"shadows/pointShadow.vs",
		"shadows/pointSpotShadow.fs",
		"shadows/pointShadow.gs");
	Shader::clearDefaults();

	//skyboxShader.activate();
	//skyboxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
	//skyboxShader.set3Float("max", 0.945f, 1.000f, 0.682f);
	//Cubemap skybox;
	//skybox.init();
	//skybox.loadTexture("assets/skybox");
	//shader.activate();
	

	//g.loadModel("assets/textures/models/m4a1/scene.gltf");
	//troll.loadModel("assets/models/lotr_troll/scene.gltf");
	scene.registerModel(&lamp);
	scene.registerModel(&wall);
	//scene.registerModel(&sphere);
	//scene.registerModel(&g);
	//scene.registerModel(&troll);
	//scene.registerModel(&cube);

	Box box;
	box.init();
	


	DirLight dirLight (glm::vec3(-0.2f, -0.9f, -0.2f),
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
		glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
		glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
		BoundingRegion (glm::vec3(-20.0f, -20.0f, 0.5f), glm::vec3(20.0f, 20.0f, 20.0f)));

	//Plane map;
	//map.init(spotLight.shadowFBO.textures[0]);
	//scene.registerModel(&map);

	scene.loadModels();

	scene.dirLight = &dirLight;
	glm::vec3 pointLightPositions[] = {
			glm::vec3(1.0f, 1.0f,  0.0f),
			glm::vec3(0.0f, 15.0f,  0.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.0014f;
	float k2 = 0.000007f;

	PointLight pointLights[4];

	for (unsigned int i = 0; i < 1; i++)
	{
		pointLights[i] = PointLight(
				pointLightPositions[i],
				k0,k1,k2,
				ambient,diffuse,specular,
				0.5f,50.0f
		);
		scene.generateInstance(lamp.id, glm::vec3(0.25),0.25,pointLightPositions[i]);
		scene.pointLights.push_back(&pointLights[i]);
		States::activate(&scene.activePointLights, i);
	}
	////scene.generateInstance(g.id, glm::vec3(0.01f), 0.25f,glm::vec3(2.0f));
	//scene.generateInstance(troll.id, glm::vec3(0.010f), 0.25f, glm::vec3(0.0f, -8.7f, -2.5f));
	//scene.generateInstance(map.id, glm::vec3(2.0f, 2.0f, 0.0f), 0.0f, glm::vec3(0.0f));

	SpotLight spotLight(camera.cameraPos, camera.cameraFront, camera.cameraUp,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0)),
		1.0f, 0.0014f, 0.000007f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f),
		0.1f, 1000.0f);
	scene.spotLights.push_back(&spotLight);
	//scene.activeSpotLights = 1;

	//scene.generateInstance(cube.id, glm::vec3(1020.0f, 0.1f,1020.0f), 100.0f, glm::vec3(0.0f, -10.0f, 0.0f));
	glm::vec3 cubePositions[] = {
		{ 1.0f, 3.0f, -5.0f },
		{ -7.25f, 2.1f, 1.5f },
		{ -15.0f, 2.55f, 9.0f },
		{ 4.0f, -3.5f, 5.0f },
		{ 2.8f, 1.9f, -6.2f },
		{ 3.5f, 6.3f, -1.0f },
		{ -3.4f, 10.9f, -5.5f },
		{ 0.0f, 11.0f, 0.0f },
		{ 0.0f, 5.0f, 0.0f },
	};

	for (unsigned int i = 0; i < 9; i++) {
		//scene.generateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
	}

	scene.generateInstance(wall.id, glm::vec3(1.0f), 1.0f, glm::vec3(0.0f, 0.0f, -2.0f));
	scene.initInstances();
	scene.prepare(box, { shader });

	scene.variableLog["time"] = (double)0.0;
	scene.defaultFBO.bind();
	shader.activate();
	shader.setBool("skipNormalMapping", false);

	while (!scene.shouldClose())
	{
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		scene.variableLog["time"] += dt;
		scene.variableLog["fps"] = 1.0f / dt;
		
		scene.update();
		proccessInput(dt);
		
		dirLight.shadowFBO.activate();

		//skyboxShader.activate();
		//skyboxShader.setFloat("time", scene.variableLog["time"].val<float>());
		//skybox.render(skyboxShader, &scene);

		//scene.renderText("comic", textShader, "Hello, OpenGL!", 50.0f, 50.0f, glm::vec2(1.0f), glm::vec3(1.0f, 0.0f, 0.2f));
		//scene.renderText("comic", textShader, "Time: " + scene.variableLog["time"].dump(), 50.0f, 1000.0f, glm::vec2(1.0f), glm::vec3(1.0f));
		//scene.renderText("comic", textShader, "FPS: " + scene.variableLog["fps"].dump(), 50.0f, 1000.0f - 40.0f, glm::vec2(1.0f), glm::vec3(1.0f));


		//for (int i = 0; i < sphere.currentNoInstances; i++)
		//{
		//	if (glm::length(camera.cameraPos - sphere.instances[i]->pos) > 250.0f)
		//	{
		//		scene.markForDeletion(sphere.instances[i]->instanceId);
		//	}
		//}

		//scene.renderShader(lampShader, false);
		//scene.renderInstances(lamp.id, lampShader, dt);
	
		//if (scene.variableLog["displayOutlines"].val<bool>())
		//{
		//	glStencilMask(0x00);  
		//}

		scene.renderDirLightShader(dirShadowShader);
		renderScene(dirShadowShader);
		
		for (unsigned int i = 0, len = scene.pointLights.size(); i < len; i++)
		{
			if (States::isIndexActive(&scene.activePointLights, i))
			{
				scene.pointLights[i]->shadowFBO.activate();
				scene.renderPointLightShader(pointShadowShader, i);
				renderScene(pointShadowShader);
			}
		}

		for (unsigned int i = 0, len = scene.spotLights.size(); i < len; i++)
		{
			if (States::isIndexActive(&scene.activeSpotLights, i))
			{
				scene.spotLights[i]->shadowFBO.activate();
				scene.renderSpotLightShader(spotShadowShader,i);
				renderScene(spotShadowShader);
			}
		}
	
		scene.defaultFBO.activate();
		scene.renderShader(shader);
		//skybox.render(skyboxShader, &scene);
		renderScene(shader);
		
		//scene.renderInstances(troll.id, shader, dt);
		//if (scene.variableLog["displayOutlines"].val<bool>())
		//{
		//	glStencilFunc(GL_ALWAYS, 1, 0xFF);
		//	glStencilMask(0xFF);
		//	scene.renderInstances(cube.id, shader, dt);

		//	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//	glStencilMask(0x00);
		//	glDisable(GL_DEPTH_TEST);
		//	scene.renderShader(outlineShader, false);
		//	scene.renderInstances(cube.id, outlineShader, dt);

		//	
		//	glStencilFunc(GL_ALWAYS, 1, 0xFF);
		//	glStencilMask(0xFF);
		//	glEnable(GL_DEPTH_TEST);
		//}
		//else
		//{
		//	scene.renderInstances(cube.id, shadowShader, dt);
		//}
		
		//scene.renderShader(gunShader);
		//scene.renderInstances(g.id, gunShader, dt);

		//scene.renderShader(shader);
		//scene.renderInstances(g.id, shader, dt);

		//scene.renderShader(shader);
		//scene.renderInstances(troll.id, shader, dt);

		scene.renderShader(boxShader,false);
		box.render(boxShader);
		//
		//scene.defaultFBO.activate();
		//scene.renderInstances(map.id, bufferShader, dt);

		scene.newFrame(box);
		scene.clearDeadInstances();

	}
	//g.cleanup();
	//skybox.cleanup();
	scene.cleanup();
	return 0;
}

void renderScene(Shader shader)
{
	//if (sphere.currentNoInstances > 0)
	//{
	//	scene.renderInstances(sphere.id, shader, dt);
	//}
	//scene.renderInstances(troll.id, shader, dt);
	//scene.renderInstances(cube.id, shader, dt);
	scene.renderInstances(lamp.id, shader, dt);
	scene.renderInstances(wall.id, shader, dt);
}

void launchItem(float dt)
{
	
	//RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(1.0f),1.0f, g.rb.pos);
	//RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(1.0f), 1.0f,scene.cameras[scene.activeCamera]->cameraPos);
	//if (rb)
	//{
	//	rb->transferEnergy(10.0f, camera.cameraFront);
	//	rb->applyAcceleration(Environment::gravitationalAcceleration);
	//}
	//float x = Camera::defaultCamera.cameraFront.x;
	//float z = Camera::defaultCamera.cameraFront.z;
	//float xz = x / z;
	//float zx = z / x;
	///RigidBody rb(1.0f, Camera::defaultCamera.cameraPos + glm::vec3(/*zx */0.0f, 0.0f,/*xz */ 0.0f));
	//RigidBody rb(1.0f, g.rb.pos);
	//rb.transferEnergy(10000.0f, scene.getActiveCamera()->cameraFront);// Camera::defaultCamera.cameraFront);
	//rb.applyAcceleration(Environment::gravitationalAcceleration);
	//launchObjects.instances.push_back(rb);
}
void proccessInput(double dt)
{
	scene.processInput(dt);

	if (States::isIndexActive(&scene.activeSpotLights, 0))
	{
		scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
		scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
		scene.spotLights[0]->up = scene.getActiveCamera()->cameraUp;
		scene.spotLights[0]->updateMatrices();
	}
	if (Keyboard::key(GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		scene.setShouldClose(true);
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
		States::toggleIndex(&scene.activeSpotLights, 0);
	}

	for (int i = 0; i < 4; i++)
	{
		if (Keyboard::keyWentDown(GLFW_KEY_1 + i))
		{
			States::toggleIndex(&scene.activePointLights, i);
		}
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
