#ifndef MODELARRAY_HPP
#define MODELARRAY_HPP

#include "../model.h"
#include "box.hpp"

#define UPPER_BOUND 100

template <class T>
class ModelArray
{
public:
	std::vector<RigidBody> instances;

	void init()
	{
		model.init();
		posVBO = BufferObject(GL_ARRAY_BUFFER);
		posVBO.generate();
		posVBO.bind();
		posVBO.setData<glm::vec3>(UPPER_BOUND, NULL, GL_DYNAMIC_DRAW);


		sizeVBO = BufferObject(GL_ARRAY_BUFFER);
		sizeVBO.generate();
		sizeVBO.bind();
		sizeVBO.setData<glm::vec3>(UPPER_BOUND, NULL, GL_DYNAMIC_DRAW);
		for (unsigned int i = 0, size = model.meshes.size(); i < size; i++)
		{

			model.meshes[i].VAO.bind();
			posVBO.bind();
			posVBO.setAttPointer<glm::vec3>(3, 3, GL_FLOAT, 1, 0, 1);
			sizeVBO.bind();
			sizeVBO.setAttPointer<glm::vec3>(4, 3, GL_FLOAT, 1, 0, 1);

			ArrayObject::clear();
		}
		
	}
	void render(Shader shader, float dt,Box* box, bool setLists = true)
	{
		if (setLists)
		{
			positions.clear();
			sizes.clear();
			for (RigidBody& rb : instances)
			{
				rb.update(dt);
				positions.push_back(rb.pos);
				sizes.push_back(model.size);
			}
		}

		shader.setMat4("model", glm::mat4(1.0f));
		model.render(shader, dt,box,false,true);
		int instances = std::min(UPPER_BOUND, (int)positions.size());
		if (instances != 0)
		{
			
			posVBO.bind();
			posVBO.updateData<glm::vec3>(0, instances, &positions[0]);
			sizeVBO.bind();
			sizeVBO.updateData<glm::vec3>(0, instances, &sizes[0]);

			sizeVBO.clear();
		}

		for (unsigned int i = 0, lenght = model.meshes.size(); i < lenght; i++)
		{
			for (unsigned int j = 0; j < instances; j++)
			{
				box->addInstance(model.meshes[i].br, positions[j], sizes[j]);
			}

			model.meshes[i].VAO.bind();
			model.meshes[i].VAO.draw(GL_TRIANGLES, model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, instances);
			ArrayObject::clear();
		}
	}
	void setSize(glm::vec3 size)
	{
		model.size = size;
	}
	void cleanup()
	{
		model.cleanup();
		posVBO.cleanup();
		sizeVBO.cleanup();
	}
protected:
	T model;

	BufferObject posVBO;
	BufferObject sizeVBO;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> sizes;
};

#endif