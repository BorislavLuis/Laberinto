#include "screen.h"

#include "keyboard.h"
#include "mouse.h"

unsigned int Screen::SCR_WIDTH = 1024;
unsigned int Screen::SCR_HEIGHT = 780;

void Screen::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

Screen::Screen()
	:window(nullptr)
{
}

bool Screen::init()
{
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Laberinto", NULL, NULL);
	if (!window)
	{
		return false;
	}
	glfwMakeContextCurrent(window);
	return true;
}

void Screen::setParameters()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwSetFramebufferSizeCallback(window, Screen::frameBufferSizeCallback);
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
}

void Screen::update()
{
	glClearColor(0.0, 0.8, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::newFrame()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Screen::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Screen::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}
