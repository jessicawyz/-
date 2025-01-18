#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include "tiny_obj_loader.h"

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	std::string diffuseMap;
	std::string specularMap;
	std::string normalMap;
	glm::vec3 rotation = glm::vec3(0.0f); // Ðý×ª½Ç¶È
	/*glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	std::string diffuseMap;
	std::string specularMap;
	std::string normalMap;
	std::string bump_texname;
	unsigned int diffuseTextureID = 0;
	unsigned int specularTextureID = 0;
	unsigned int bumpTextureID = 0;*/
};

class Model {
public:
	Model(const std::string& objPath, const std::string& mtlPath);
	~Model();
	void Draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

private:
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<Material> materials;
	std::vector<unsigned int> textureIDs;
	std::string directory;
	unsigned int VAO, VBO, EBO;

	std::vector<tinyobj::shape_t> shapes;
	tinyobj::attrib_t attrib;

	bool loadOBJ(const char* path, const char* mtlPath);
	void processMaterials(const std::vector<tinyobj::material_t>& materials);
	unsigned int loadTexture(const char* path);
	void setupMesh();

	std::string convertToASCII(const std::string& path);
	std::string getFileNameFromPath(const std::string& path);
};

#endif
#pragma once