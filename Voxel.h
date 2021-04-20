#ifndef Voxel_H
#define Voxel_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

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
};


#endif