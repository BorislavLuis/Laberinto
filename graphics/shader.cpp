#include "shader.h"

Shader::Shader()
{
}

Shader::Shader(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geoShaderPath)
{
	generate(includeDefaultHeader,vertexShaderPath, fragmentShaderPath,geoShaderPath);
}
void Shader::generate(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geoShaderPath)
{
	int success;
	char infoLog[512];
	GLuint vertexShader = compileShader(includeDefaultHeader,vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(includeDefaultHeader,fragmentShaderPath, GL_FRAGMENT_SHADER);
	
	GLuint geoShader = 0;
	if (geoShaderPath)
	{
		geoShader = compileShader(includeDefaultHeader,geoShaderPath, GL_GEOMETRY_SHADER);
	}
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	if (geoShaderPath)
	{
		glAttachShader(id, geoShader);
	}
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Linking error: " << std::endl << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geoShaderPath)
	{
		glDeleteShader(geoShader);
	}
}


void Shader::activate()
{
	glUseProgram(id);
}


void Shader::loadIntoDefault(const char* filepath)
{
	std::string fileContents = Shader::loadShaderSrc(false,filepath);
	
	Shader::defaultHeaders << fileContents;
}

void Shader::clearDefaults()
{
	Shader::defaultHeaders.clear();
}

std::string Shader::loadShaderSrc(bool includeDefaultHeader, const char* filename)
{
	std::ifstream file;
	std::stringstream buf;
	std::string ret = "";

	if (includeDefaultHeader)
	{
		buf << Shader::defaultHeaders.str();
	}
	std::string fullPath = Shader::defaultDirectory + '/' + filename;

	file.open(fullPath.c_str());

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

GLuint Shader::compileShader(bool includeDefaultHeader, const char* filepath, GLuint type)
{
	int success;
	char infoLog[512];
	
	unsigned int ret = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(includeDefaultHeader, filepath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);
	

	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ret, 512, NULL, infoLog);
		std::cout << "Error with shader comp: " << filepath  << ":" << std::endl << infoLog << std::endl;
	}
	return ret;
}

void Shader::setMat4(const std::string& name, glm::mat4 val)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,glm::value_ptr(val));
}

void Shader::setMat3(const std::string& name, glm::mat3 val)
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setInt(const std::string& name, int val)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()),val);
}

void Shader::setBool(const std::string& name, bool val)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::setFloat(const std::string& name, float val)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::set3Float(const std::string& name, glm::vec3 v)
{
	set3Float(name, v.x, v.y, v.z);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3)
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set4Float(const std::string& name, aiColor4D color)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);
}

void Shader::set4Float(const std::string& name, glm::vec4 v)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), v.x,v.y,v.z,v.w);
}

std::stringstream Shader::defaultHeaders;
