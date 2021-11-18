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
#include "graphics/models/box.hpp"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "algorithms/states.hpp"
#include "algorithms/trie.hpp"
#include "algorithms/octree.h"

namespace Octree
{
	class node;
}
class Model;
class Scene
{
public:
	trie::Trie<Model*> models;
	trie::Trie<RigidBody*> instances;

	std::vector<RigidBody*> instancesToDelete;

	Octree::node* octree;
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	Scene();
	Scene(int glfwVersionMajor, int glfwVersionMinor,
		const char* title, unsigned int scrWidth, unsigned int scrHeight);
	bool init();

	void prepare(Box& box);
	void processInput(float dt);
	void update();
	void newFrame(Box& box);
	void renderShader(Shader shader, bool applyLighting = true);
	void renderInstances(std::string modelId, Shader shader, float dt);
	void cleanup();

	bool shouldClose();
	Camera* getActiveCamera();

	void setShouldClose(bool shouldClose);
	void setWindowColor(float r, float g, float b, float a);

	void registerModel(Model* model);
	RigidBody* generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos);
	
	void initInstances();
	void loadModels();
	void removeInstance(std::string instanceId);
	void markForDeletion(std::string instanceId);
	void clearDeadInstances();


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
