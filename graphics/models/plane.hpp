#ifndef PLANE_HPP
#define PLANE_HPP

#include "../model.h"

class Plane : public Model
{
public:
	Plane()
		:Model("plane", BoundTypes::AABB, 1, CONST_INSTANCES){}

	void init(std::vector<Texture> tex)
	{
		int noVertices = 4;
		float quadVertices[] = {
			0.5f,  0.5f, 0.0f,  0.0f,0.0f,1.0f, 1.0f, 1.0f,
		    0.5f, -0.5f, 0.0f,  0.0f,0.0f,1.0f, 1.0f, 0.0f,
		   -0.5f, -0.5f, 0.0f,  0.0f,0.0f,1.0f, 0.0f, 0.0f,
		   -0.5f,  0.5f, 0.0f,  0.0f,0.0f,1.0f, 0.0f, 1.0f
		};
		std::vector<unsigned int> indices = {
			0, 1, 3,
			1, 2, 3
		};

		BoundingRegion br(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f));

		Mesh ret(br, tex);
		ret.loadData(Vertex::genList(quadVertices, noVertices), indices,true);

		meshes.push_back(ret);
		boundingRegions.push_back(br);
	}
};

#endif // !PLANE_HPP
