#include "shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	int success;
	char infoLog[512];
	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);
	
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Linking error: " << std::endl << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate()
{
	glUseProgram(id);
}

std::string Shader::loadShaderSrc(const char* filename)
{
	std::ifstream file;
	std::stringstream buf;
	std::string ret = "";

	file.open(filename);

	if (file.is_open())
	{
		buf << file.rdbuf();
		ret = buf.str();
	}
	else
	{
		std::cout << "Could not open " << filename << std::endl;
	}
	file.close();

	return ret;
}

GLuint Shader::compileShader(const char* filepath, GLenum type)
{
	int success;
	char infoLog[512];

	GLuint ret = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(filepath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);
	

	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ret, 512, NULL, infoLog);
		std::cout << "Error with shader comp: " << std::endl << infoLog << std::endl;
	}
	return ret;
}

void Shader::setMat4(const std::string& name, glm::mat4 val)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,glm::value_ptr(val));
}
