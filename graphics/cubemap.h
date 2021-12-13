#ifndef  CUBEMAP_H
#define CUBEMAP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "shader.h"
#include "glmemory.hpp"

class Cubemap
{
public:
	Cubemap();
	
	void loadTexture(std::string _dir,
		std::string right = "right.png",
		std::string left = "left.png",
		std::string top = "top.png",
		std::string bottom = "bottom.png",
		std::string front = "front.png",
		std::string back = "back.png");
	
	void init();
	void render(Shader shader);
	void cleanup();
private:
	unsigned int id;
	std::string dir;
	std::vector<std::string> faces;
	bool hasTextures;

	ArrayObject VAO;

};

#endif // ! CUBEMAP_H

