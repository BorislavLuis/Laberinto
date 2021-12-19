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
#include "graphics/models/plane.hpp"

#include "graphics/model.h"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"

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

Sphere sphere(10);

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

	//Shader shader("assets/object.vs", "assets/object.fs");
	Shader gunShader("assets/object.vs", "assets/object.fs");
	Shader lampShader("assets/instanced/instanced.vs", "assets/lamp.fs");
	Shader shader("assets/instanced/instanced.vs", "assets/object.fs");
	Shader boxShader("assets/instanced/box.vs", "assets/instanced/box.fs");
	Shader textShader("assets/text.vs", "assets/text.fs");
	Shader skyboxShader("assets/skybox/skybox.vs", "assets/skybox/skybox.fs");
	Shader outlineShader("assets/outline.vs", "assets/outline.fs");
	Shader bufferShader("assets/buffer.vs", "assets/buffer.fs");

	//skyboxShader.activate();
	//skyboxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
	//skyboxShader.set3Float("max", 0.945f, 1.000f, 0.682f);
	Cubemap skybox;
	skybox.init();
	skybox.loadTexture("assets/skybox");
	shader.activate();
	
	Lamp lamp(4);
	//g.loadModel("assets/textures/models/m4a1/scene.gltf");
	//troll.loadModel("assets/textures/models/lotr_troll/scene.gltf");
	scene.registerModel(&lamp);
	scene.registerModel(&sphere);
	//scene.registerModel(&g);
	//scene.registerModel(&troll);
	Cube cube(21);
	scene.registerModel(&cube);
	Box box;
	box.init();


	scene.loadModels();

	const GLuint BUFFER_WIDTH = 1920, BUFFER_HEIGHT = 1080;
	GLuint fbo;
	glGenFramebuffers(1, &fbo);

	Texture bufferTex("bufferTex");
	
	bufferTex.bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, BUFFER_WIDTH, BUFFER_HEIGHT, 0, GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferTex.id, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, BUFFER_WIDTH, BUFFER_HEIGHT);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,rbo);

	Plane map;
	map.init(bufferTex);
	scene.registerModel(&map);

	glBindBuffer(GL_FRAMEBUFFER, 0);


	DirLight dirLight = { glm::vec3(-0.2f,-1.0f,-1.5f),
			glm::vec4(0.1f,0.1f,0.1f,1.0f),
			glm::vec4(0.4f,0.4f,0.4f,1.0f),
			glm::vec4(0.75f,0.75f,0.75f,1.0f) };
	scene.dirLight = &dirLight;
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

	PointLight pointLights[4];

	for (unsigned int i = 0; i < 4; i++)
	{
		pointLights[i] = {
				pointLightPositions[i],
				k0,k1,k2,
				ambient,diffuse,specular
		};
		scene.generateInstance(lamp.id, glm::vec3(0.25),0.25,pointLightPositions[i]);
		scene.pointLights.push_back(&pointLights[i]);
		States::activate(&scene.activePointLights, i);
	}
	//scene.generateInstance(g.id, glm::vec3(0.01f), 0.25f,glm::vec3(2.0f));
	//scene.generateInstance(troll.id, glm::vec3(0.010f), 0.25f, glm::vec3(3.0, 0.5f, 2.5f));

	SpotLight spotLight = {
		camera.cameraPos,camera.cameraFront,
		glm::cos(glm::radians(12.5f)),glm::cos(glm::radians(20.0f)),1.0f,0.0014f,0.000007f,
		glm::vec4(0.0f,0.0f,0.0f,1.0f),glm::vec4(1.0f),glm::vec4(1.0f)};
	scene.spotLights.push_back(&spotLight);
	scene.activeSpotLights = 1;
	

	scene.generateInstance(cube.id, glm::vec3(20.0f, 0.1f, 20.0f), 100.0f, glm::vec3(0.0f, -10.0f, 0.0f));
	glm::vec3 cubePositions[] = {
		{ 1.0f, 3.0f, -5.0f },
		{ -7.25f, 2.1f, 1.5f },
		{ -15.0f, 2.55f, 9.0f },
		{ 4.0f, -3.5f, 5.0f },
		{ 2.8f, 1.9f, -6.2f },
		{ 3.5f, 6.3f, -1.0f },
		{ -3.4f, 10.9f, -5.5f },
		{ 10.0f, -2.0f, 13.2f },
		{ 2.1f, 7.9f, -8.3f },
	};

	for (unsigned int i = 0; i < 9; i++) {
		scene.generateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
	}

	scene.generateInstance(map.id, glm::vec3(2.0f, 2.0f, 0.0f), 0.0f, glm::vec3(0.0f));

	scene.initInstances();
	scene.prepare(box);

	scene.variableLog["time"] = (double)0.0;

	while (!scene.shouldClose())
	{
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		scene.variableLog["time"] += dt;
		scene.variableLog["fps"] = 1.0f / dt;

		
		
		scene.update();
		proccessInput(dt);

		glViewport(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		//skyboxShader.activate();
		//skyboxShader.setFloat("time", scene.variableLog["time"].val<float>());
		skybox.render(skyboxShader, &scene);

		scene.renderText("comic", textShader, "Hello, OpenGL!", 50.0f, 50.0f, glm::vec2(1.0f), glm::vec3(1.0f, 0.0f, 0.2f));
		scene.renderText("comic", textShader, "Time: " + scene.variableLog["time"].dump(), 50.0f, 1000.0f, glm::vec2(1.0f), glm::vec3(1.0f));
		scene.renderText("comic", textShader, "FPS: " + scene.variableLog["fps"].dump(), 50.0f, 1000.0f - 40.0f, glm::vec2(1.0f), glm::vec3(1.0f));


		for (int i = 0; i < sphere.currentNoInstances; i++)
		{
			if (glm::length(camera.cameraPos - sphere.instances[i]->pos) > 250.0f)
			{
				scene.markForDeletion(sphere.instances[i]->instanceId);
			}
		}

		scene.renderShader(lampShader, false);
		scene.renderInstances(lamp.id, lampShader, dt);

		if (scene.variableLog["displayOutlines"].val<bool>())
		{
			glStencilMask(0x00);  
		}

		scene.renderShader(shader);
		if (sphere.currentNoInstances > 0)
		{
			scene.renderInstances(sphere.id, shader, dt);
		}

		//scene.renderInstances(cube.id, shader, dt);

		if (scene.variableLog["displayOutlines"].val<bool>())
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			scene.renderInstances(cube.id, shader, dt);

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			scene.renderShader(outlineShader, false);
			scene.renderInstances(cube.id, outlineShader, dt);

			
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			scene.renderInstances(cube.id, shader, dt);
		}
		
		//scene.renderShader(gunShader);
		//scene.renderInstances(g.id, gunShader, dt);

		//scene.renderShader(shader);
		//scene.renderInstances(g.id, shader, dt);

		//scene.renderShader(shader);
		//scene.renderInstances(troll.id, shader, dt);

		scene.renderShader(boxShader,false);
		box.render(boxShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);
	
		scene.renderInstances(map.id, bufferShader, dt);

		scene.newFrame(box);
		scene.clearDeadInstances();

	}
	//g.cleanup();
	skybox.cleanup();
	scene.cleanup();
	return 0;
}
void launchItem(float dt)
{
	
	//RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(1.0f),1.0f, g.rb.pos);
	RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(1.0f), 1.0f,scene.cameras[scene.activeCamera]->cameraPos);
	if (rb)
	{
		rb->transferEnergy(10.0f, camera.cameraFront);
		rb->applyAcceleration(Environment::gravitationalAcceleration);
	}
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
