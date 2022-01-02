#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
public:
	unsigned int id;

	Shader();
	Shader(bool includeDefaultHeader,const char* vertexShaderPath, const char* fragmentShaderPath, const char* geoShaderPath = nullptr);

	void generate(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragmentShaderPath, const char* geoShaderPath = nullptr);
	void activate();


	GLuint compileShader(bool includeDefaultHeader,const char* filepath, GLenum type);

	void setMat4(const std::string& name, glm::mat4 val);
	void setInt(const std::string& name, int val);
	void setBool(const std::string& name, bool val);
	void setFloat(const std::string& name, float val);
	void set3Float(const std::string& name, glm::vec3 v);
	void set3Float(const std::string& name, float v1, float v2, float v3);
	void set4Float(const std::string& name, aiColor4D color);
	void set4Float(const std::string& name, glm::vec4 v);

	static std::string defaultDirectory;
	static std::stringstream defaultHeaders;
	static void loadIntoDefault(const char* filepath);
	static void clearDefaults();
	static std::string loadShaderSrc(bool includeDefaultHeader, const char* filename);
};
#endif
