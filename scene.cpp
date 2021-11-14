#include "scene.h"

unsigned int Scene::scrWidth = 0;
unsigned int Scene::scrHeight = 0;

void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Scene::scrWidth = width;
	Scene::scrHeight = height;
}

Scene::Scene(){}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight)
	:glfwVersionMajor(glfwVersionMajor),glfwVersionMinor(glfwVersionMinor),
	title(title),activeCamera(-1)
{
	Scene::scrWidth = scrWidth;
	Scene::scrHeight = scrHeight;

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
	glfwSetFramebufferSizeCallback(window, glfwSetFramebufferSizeCallback);
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR_DISABLED);
	
	return true;
}

void Scene::processInput(float dt)
{
	if (activeCamera != -1 && activeCamera < cameras.size())
	{
		if (Mouse::getDY() != 0 && Mouse::getDY() != 0)
		{
			cameras[activeCamera]->updateCameraDirection(Mouse::getDX(), Mouse::getDY());
		}
		if (Mouse::getScrollDY() != 0)
		{
			cameras[activeCamera]->updateCameraZoom(Mouse::getScrollDY());
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
			glm::radians(cameras[activeCamera]->getZoom(),
			(float)scrWidth / (float)scrHeight,
			0.1, 100.0f);
		cameraPos = camera[activeCamera]->cameraPos;
	}
}

void Scene::update()
{
	glClearColor(bg[0], bg[1].bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::newFrame()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Scene::render(Shader shader, bool applyLighting)
{
	shader.activate();
	
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);

	if (applyLighting)
	{
		unsigned int noLights = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++)
		{
			if (States::isActive(&activePointLights, i))
			{
				pointLights[i]->render(shader, noActiveLights);
				noActiveLights++;
			}
		}
		shader.setInt("noPointLights", noActiveLights);
		noLights = spotLights.size();
		for (unsigned int i = 0; i < noLights; i++)
		{
			if (States::isActive(&activeSpotLights, i))
			{
				spotLights[i]->render(shader, noActiveLights);
			}
		}
		shader.setInt("noSpotLights", noActiveLights);
		dirLight->render(shader);
	}
}

void Scene::cleanup()
{
}

bool Scene::shouldClose()
{
	return false;
}

Camera* Scene::getActiveCamera()
{
	return nullptr;
}

void Scene::setShouldClose(bool shouldClose)
{
}

void Scene::setWindowColor(float r, float g, float b, float a)
{
}
