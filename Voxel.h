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

using namespace std;


class Voxel
{
public:
	glm::vec3 position;
	glm::vec3 color;
	int size;

	Voxel(glm::vec3 position = glm::vec3(0.0f), glm::vec3 color = glm::vec3(0.6f), int size = 1)
	{
		this->position = position;
		this->color = color;
		this->size = size;
	}

	void Draw(Model voxelModel,Shader shader)
	{
		voxelModel.faceColor = this->color;
		voxelModel.Draw(shader);
	}

	Voxel& operator = (Voxel &v)
	{
		this->position = v.position;
		this->color = v.color;
		this->size = v.size;

		return *this;
	}

	void operator = (Voxel* v)
	{
		this->position = v->position;
		this->color = v->color;
		this->size = v->size;
	}
};


#endif