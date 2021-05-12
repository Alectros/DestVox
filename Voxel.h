#ifndef Voxel_H
#define Voxel_H

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
#include "VoxelMaterial.h"

using namespace std;

class Voxel
{
public:
	glm::vec3 position;
	VoxelMaterial material;
	int size;

	Voxel(glm::vec3 colorAMB = glm::vec3(0.6f), glm::vec3 colorDIF = glm::vec3(0.6f), glm::vec3 colorSPEC = glm::vec3(0.6f), float shininess = 64, glm::vec3 position = glm::vec3(0.0f),int size = 1)
	{
		this->position = position;
		this->size = size;
		this->material = VoxelMaterial(colorAMB, colorDIF, colorSPEC, shininess);
	}

	Voxel(VoxelMaterial& material, glm::vec3 position = glm::vec3(0.0f),  int size = 1)
	{
		this->position = position;
		this->size = size;
		this->material = material;
	}

	void Draw(Model voxelModel,Shader shader)
	{
		voxelModel.Draw(shader);
	}

	Voxel& operator = (Voxel &v)
	{
		this->position = v.position;
		this->material = v.material;
		this->size = v.size;

		return *this;
	}

	void operator = (Voxel* v)
	{
		this->position = v->position;
		this->material = v->material;
		this->size = v->size;
	}
};


#endif