#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader
{
public:
	unsigned int id;

	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void activate();

	std::string loadShaderSrc(const char* filename);
	GLuint compileShader(const char* filepath, GLenum type);

	void setMat4(const std::string& name, glm::mat4 val);

};
#endif
