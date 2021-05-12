#ifndef VoxelMaterial_H
#define VoxelMaterial_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <vector>

#include "Tools/Model.h"

using namespace std;

class VoxelMaterial
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float shininess;

	VoxelMaterial();
	VoxelMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

	void SetToShader(Shader shader, string name);
};


map<string, VoxelMaterial> LoadMaterials(string path);

#endif