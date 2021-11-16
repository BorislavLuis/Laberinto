#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "graphics/light.h"
#include "graphics/shader.h"
#include "graphics/model.h"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "algorithms/states.hpp"
class Model;
class Scene
{
public:
	std::map<std::string, Model*> models;
	std::map<std::string, std::pair<std::string, unsigned int>> instances;

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	Scene();
	Scene(int glfwVersionMajor, int glfwVersionMinor,
		const char* title, unsigned int scrWidth, unsigned int scrHeight);
	bool init();

	void processInput(float dt);
	void update();
	void newFrame();
	void renderShader(Shader shader, bool applyLighting = true);
	void renderInstances(std::string modelId, Shader shader, float dt);
	void cleanup();

	bool shouldClose();
	Camera* getActiveCamera();

	void setShouldClose(bool shouldClose);
	void setWindowColor(float r, float g, float b, float a);

	void registerModel(Model* model);
	std::string generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos);
	
	void initInstances();
	void loadModels();
	void removeInstance(std::string instanceId);


	std::string currentId;
	std::string generateId();
	std::vector<PointLight*> pointLights;
	unsigned int activePointLights;
	std::vector<SpotLight*> spotLights;
	unsigned int activeSpotLights;
	DirLight* dirLight;
	bool dirLightActive;

	std::vector<Camera*> cameras;
	unsigned int activeCamera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPos;

protected:
	GLFWwindow* window;
	const char* title;
	static unsigned int scrWidth;
	static unsigned int scrHeight;

	float bg[4];

	int glfwVersionMajor;
	int glfwVersionMinor;

};

#endif // !SCENE_H
