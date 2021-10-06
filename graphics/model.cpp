#include "model.h"

Model::Model(){}

Model::Model(glm::vec3 pos, glm::vec3 size)
	:pos(pos),size(size){}

void Model::init(){}


void Model::render(Shader& shader)
{
	for (Mesh mesh : meshes)
	{
		mesh.render(shader);
	}
}

void Model::cleanup()
{
	for (Mesh mesh : meshes)
	{
		mesh.cleanup();
	}
}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Could not load model at " << path << std::endl << import.GetErrorString() << std::endl;
		
		return;
	}
	directory = path.substr(0, path.find_last_of("/"));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	return Mesh();
}
