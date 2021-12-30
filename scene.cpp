#include "scene.h"

unsigned int Scene::scrWidth = 0;
unsigned int Scene::scrHeight = 0;


std::string Scene::generateId()
{
	for (int i = currentId.length() - 1; i >= 0; i--)
	{
		if ((int)currentId[i] != (int)'z')
		{
			currentId[i] = (char)(((int)currentId[i] + 1));
			break;
		}
		else
		{
			currentId[i] = 'a';
		}
	}
	return currentId;
}

void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Scene::scrWidth = width;
	Scene::scrHeight = height;
}

Scene::Scene():currentId("aaaaaaaa"){}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor),
	title(title), activeCamera(-1), activePointLights(0), activeSpotLights(0), currentId("aaaaaaaa")
{
	Scene::scrWidth = scrWidth;
	Scene::scrHeight = scrHeight;

	defaultFBO = FramebufferObject(scrWidth, scrHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	setWindowColor(0.0, 0.8, 0.4, 1.0);
}

bool Scene::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(scrWidth, scrHeight, title, NULL, NULL);
	if (window == NULL)
	{
		return false;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	glViewport(0, 0, scrWidth, scrHeight);
	GLFWimage images[1];
	int we, he, chh;
	unsigned char* data = stbi_load("assets/textures/earth.png", &we, &he, &chh, 0);
	images[0].pixels = data;
	images[0].width = we;
	images[0].height = he;
	glfwSetWindowIcon(window, 1, images);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_DISABLED);
	
	octree = new Octree::node(BoundingRegion(glm::vec3(-128.0f), glm::vec3(128.0f)));

	if(FT_Init_FreeType(&ft))
	{ 
		std::cout << "Could not init FreeType" << std::endl;
		return false;
	}

	fonts.insert("comic", new TextRenderer(32));
	if (!fonts["comic"]->loadFont(ft, "assets/fonts/comic.ttf"))
	{
		std::cout << "Could not load font" << std::endl;
		return false;
	}

	FT_Done_FreeType(ft);
	
	variableLog["useBlinn"] = true;
	variableLog["useGamma"] = false;
	variableLog["displayOutlines"] = false;

	return true;
}

void Scene::prepare(Box& box)
{
	octree->update(box);
}

void Scene::processInput(float dt)
{
	if (activeCamera != -1 && activeCamera < cameras.size())
	{
		double dx = Mouse::getDX();
		double dy = Mouse::getDY();
		if (dx != 0 || dy != 0)
		{
			cameras[activeCamera]->updateCameraDirection(dx, dy);
		}
		double scrollDy = Mouse::getScrollDY();
		if (scrollDy != 0)
		{
			cameras[activeCamera]->updateCameraZoom(scrollDy);
		}

		if (Keyboard::key(GLFW_KEY_W))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_S))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_D))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);
		}
		if (Keyboard::key(GLFW_KEY_A))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);
		}
		if (Keyboard::key(GLFW_KEY_SPACE))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
		}
		if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
		}

		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(
			glm::radians(cameras[activeCamera]->getZoom()),
			(float)scrWidth / (float)scrHeight,
			0.1f, 1000.0f);
		textProjection = glm::ortho(0.0f, (float)scrWidth, 0.0f, (float)scrHeight);
		cameraPos = cameras[activeCamera]->cameraPos;
	}
		if(Keyboard::keyWentDown(GLFW_KEY_B))
		{
			variableLog["useBlinn"] = !variableLog["useBlinn"].val<bool>();
		}
		if (Keyboard::keyWentDown(GLFW_KEY_G))
		{
			variableLog["useGamma"] = !variableLog["useGamma"].val<bool>();
		}
		if (Keyboard::keyWentDown(GLFW_KEY_O))
		{
			variableLog["displayOutlines"] = !variableLog["displayOutlines"].val<bool>();
		}
}

void Scene::update()
{
	glClearColor(bg[0], bg[1],bg[2], bg[3]);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	defaultFBO.clear();
}

void Scene::newFrame(Box& box)
{
	box.positions.clear();
	box.sizes.clear();
	octree->processPending();
	octree->update(box);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Scene::renderShader(Shader shader, bool applyLighting)
{
	shader.activate();
	
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);

  	if (applyLighting)
	{
		unsigned int textureIdx = 31;

		dirLight->render(shader, textureIdx--);

		unsigned int noLights = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++)
		{
			if (States::isActive(&activePointLights, i))
			{
				pointLights[i]->render(shader, noActiveLights,textureIdx--);
				noActiveLights++;
			}
		}
		shader.setInt("noPointLights", noActiveLights);
		noLights = spotLights.size();
		noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++)
		{
			if (States::isActive(&activeSpotLights, i))
			{
				spotLights[i]->render(shader, noActiveLights,textureIdx--);
				noActiveLights++;
			}
		}
		shader.setInt("noSpotLights", noActiveLights);
		shader.setBool("useBlinn", variableLog["useBlinn"].val<bool>());
		shader.setBool("useGamma", variableLog["useGamma"].val<bool>());
	}
}

void Scene::renderDirLightShader(Shader shader)
{
	shader.activate();
	shader.setMat4("lightSpaceMatrix", dirLight->lightSpaceMatrix);

}

void Scene::renderPointLightShader(Shader shader, unsigned int idx)
{
	shader.activate();
	for (unsigned int i = 0; i < 6; i++)
	{
		shader.setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", pointLights[idx]->lightSpaceMatrices[i]);
	}
	shader.set3Float("lightPos", pointLights[idx]->position);
	shader.setFloat("farPlane", pointLights[idx]->farPlane);
}

void Scene::renderSpotLightShader(Shader shader, unsigned int idx)
{
	shader.activate();
	shader.setMat4("lightSpaceMatrix", spotLights[idx]->lightSpaceMatrix);
	shader.set3Float("lightPos", spotLights[idx]->position);
	shader.setFloat("farPlane", spotLights[idx]->farPlane);
}

void Scene::renderInstances(std::string modelId, Shader shader, float dt)
{
	shader.activate();
	models[modelId]->render(shader, dt, this);
}

void Scene::renderText(std::string font, Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color)
{
	shader.activate();
	shader.setMat4("projection", textProjection);
	fonts[font]->render(shader, text, x, y,scale, color);
}

void Scene::cleanup()
{
	models.traverse([](Model* model)->void {model->cleanup(); });
	models.cleanup();
	instances.cleanup();
	fonts.traverse([](TextRenderer* tr)->void {tr->cleanup(); });
	fonts.cleanup();
	octree->destroy();
	glfwTerminate();
}

bool Scene::shouldClose()
{
	return glfwWindowShouldClose(window);
}

Camera* Scene::getActiveCamera()
{
	return (activeCamera >= 0 && activeCamera < cameras.size())? cameras[activeCamera]: nullptr;
}

void Scene::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Scene::setWindowColor(float r, float g, float b, float a)
{
	bg[0] = r;
	bg[1] = g;
	bg[2] = b;
	bg[3] = a;
}

void Scene::registerModel(Model* model)
{
	models.insert(model->id, model);
}

RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos)
{
	RigidBody* rb = models[modelId]->generateInstances(size, mass, pos);
	if (rb)
	{
		std::string id = generateId();
		rb->instanceId = id;
		instances.insert(id ,rb);
		octree->addToPending(rb, models);
		return rb;
	}
	return nullptr;
}

void Scene::initInstances()
{
	models.traverse([](Model* model)->void {model->initInstances();});
}

void Scene::loadModels()
{
	models.traverse([](Model* model)->void {model->init();});
}

void Scene::removeInstance(std::string instanceId)
{
	std::string targetModel = instances[instanceId]->modelId;
	models[targetModel]->removeInstance(instanceId);
	//delete instances[instanceId];
	//instances[instanceId] = nullptr;
	instances.erase(instanceId);
}

void Scene::markForDeletion(std::string instanceId)
{
	States::activate(&instances[instanceId]->state, INSTANCE_DEAD);
	instancesToDelete.push_back(instances[instanceId]);
}

void Scene::clearDeadInstances()
{
	for (RigidBody* rb : instancesToDelete)
	{
		removeInstance(rb->instanceId);
	}
	instancesToDelete.clear();
}
