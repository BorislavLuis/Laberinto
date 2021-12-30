#include "light.h"

DirLight::DirLight(){}

DirLight::DirLight(glm::vec3 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, BoundingRegion br)
	:direction(direction),ambient(ambient),diffuse(diffuse),specular(specular),br(br),shadowFBO(2048,2048,GL_DEPTH_BUFFER_BIT)
{
	shadowFBO.generate();
	shadowFBO.bind();
	shadowFBO.disableColorBuffer();
	shadowFBO.allocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	updateMatrices();
}

void DirLight::updateMatrices()
{
	glm::mat4 proj = glm::ortho(br.min.x,br.max.x,br.min.y,br.max.y,br.min.z,br.max.z);
	glm::vec3 pos = -2.0f * direction;

	glm::mat4 lightView = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	lightSpaceMatrix = proj * lightView;
}

void DirLight::render(Shader shader, unsigned int textureIdx)
{
	std::string name = "dirLight";

	shader.set3Float(name + ".direction", direction);
	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);
	shader.setFloat(name + ".farPlane", br.max.z);

	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.textures[0].bind();
	shader.setInt("dirLight.depthBuffer", textureIdx);
	shader.setMat4("dirLight.lightSpaceMatrix", lightSpaceMatrix);
}

glm::vec3 PointLight::directions[6] ={
	{ 1.0f,  0.0f,  0.0f},
	{-1.0f,  0.0f,  0.0f},
	{ 0.0f,  1.0f,  0.0f},
	{ 0.0f, -1.0f,  0.0f},
	{ 0.0f,  0.0f,  1.0f},
	{ 0.0f,  0.0f, -1.0f}
};
glm::vec3 PointLight::ups[6] = {
	{ 0.0f, -1.0f,  0.0f},
	{ 0.0f,  1.0f,  0.0f},
	{ 0.0f,  0.0f,  1.0f},
	{ 0.0f,  0.0f, -1.0f},
	{ 0.0f, -1.0f,  0.0f},
	{ 0.0f,  1.0f,  0.0f}
};

PointLight::PointLight(){}

PointLight::PointLight(glm::vec3 position, float k0, float k1, float k2, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float nearPlane, float farPlane)
	:position(position),k0(k0),k1(k1),k2(k2),
	ambient(ambient),diffuse(diffuse),specular(specular),
	nearPlane(nearPlane),farPlane(farPlane),
	shadowFBO(2048,2048,GL_DEPTH_BUFFER_BIT)
{
	shadowFBO.generate();
	shadowFBO.bind();
	shadowFBO.disableColorBuffer();
	shadowFBO.allocateAndAttachCubemap(GL_DEPTH_ATTACHMENT,GL_DEPTH_COMPONENT,GL_FLOAT);
	updateMatrices();
}
void PointLight::render(Shader shader, int idx,unsigned int textureIdx)
{
	std::string name = "pointLight["+std::to_string(idx)+"]";

	shader.set3Float(name + ".position", position);
	
	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);

	shader.setFloat(name + ".nearPlane", nearPlane);
	shader.setFloat(name + ".farPlane", farPlane);

	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.cubemap.bind();
	shader.setInt(name + ".depthBuffer", textureIdx);
}
void PointLight::updateMatrices()
{
	glm::mat4 proj = glm::perspective(glm::radians(90.0f),
		(float)shadowFBO.height / (float)shadowFBO.width,
		nearPlane, farPlane);

	for (unsigned int i = 0; i < 6; i++)
	{
		lightSpaceMatrices[i] = proj * glm::lookAt(position,
			position + PointLight::directions[i],
			PointLight::ups[i]);
	}
}

void SpotLight::render(Shader shader, int idx,unsigned int textureIdx)
{
	std::string name = "spotLight[" + std::to_string(idx) + "]";
	
	shader.set3Float(name + ".position", position);
	shader.set3Float(name + ".direction", direction);

	shader.setFloat(name + ".cutOff", cutOff);
	shader.setFloat(name + ".outerCutOff", outerCutOff);

	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);

	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.textures[0].bind();
	shader.setInt(name + ".depthBuffer", textureIdx);
	shader.setMat4(name + ".lightSpaceMatrix",lightSpaceMatrix);
}

SpotLight::SpotLight(){}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float cutOff, float outerCutOff, float k0, float k1, float k2, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float nearPlane, float farPlane)
	:position(position),direction(direction),
	up(up),cutOff(cutOff),outerCutOff(outerCutOff),
	k0(k0),k1(k1),k2(k2),ambient(ambient),
	diffuse(diffuse),specular(specular),
	nearPlane(nearPlane),farPlane(farPlane),
	shadowFBO(2048,2048,GL_DEPTH_BUFFER_BIT)
{
	shadowFBO.generate();
	shadowFBO.disableColorBuffer();
	shadowFBO.allocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	updateMatrices();
}

void SpotLight::updateMatrices()
{
	glm::mat4 proj = glm::perspective(glm::acos(outerCutOff) * 2.0f,
		(float)shadowFBO.height / (float)shadowFBO.width,
		nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(position, position + direction, up);

	lightSpaceMatrix = proj * lightView;
}
