#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void proccessInput(GLFWwindow* window);



int main()
{
	glm::vec4 vec(1.0f, 1.0f, 1.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

	int success;
	char infoLog[512];

	Shader shader("assets/vertex_core.glsl","assets/fragment_core.glsl");


	vec = trans * vec;
	std::cout << vec.x << " " << vec.y << " " << vec.z << " " << std::endl;
	std::cout << "Hello world" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(1024, 780, "Opengl is the best", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Couldn't create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return -1;
	}
	glViewport(0, 0, 1024, 780);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	

	float vertices[] = {
		0.5f,0.5f,0.0f,   1.0f,1.0f,0.5f,
		-0.5f, 0.5f,0.0f, 1.0f,0.4f,0.0f,
		-0.5f,-0.5f,0.0f, 1.0f,0.4f,0.0f,
		0.5f,-0.5f,0.0f,  1.0f,0.4f,0.0f
	};
	unsigned int indecies[] =
	{
		0,1,2,
		2,3,0
	};

	unsigned int VAO, VBO,EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 *sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indecies), indecies, GL_STATIC_DRAW);


	trans = glm::rotate(trans, glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	while (!glfwWindowShouldClose(window))
	{

		proccessInput(window);

		glClearColor(0.0, 0.8, 0.4, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		trans = glm::rotate(trans, glm::radians((float)glfwGetTime()/100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glBindVertexArray(VAO);
		shader.activate();
		shader.setMat4("transform", trans);

		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);

		glfwPollEvents();
		glfwSwapBuffers(window);
		
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void proccessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window,true);
	}
}
