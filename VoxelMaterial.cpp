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
#include <map>
#include <fstream>

#include "Tools/Model.h"
#include "VoxelMaterial.h"


map<string, VoxelMaterial> LoadMaterials(string path)
{
	map<string, VoxelMaterial> voxel_materials;

	ifstream in(path);
	
	while (!in.eof())
	{
		string name;
		glm::vec3 amb, dif, spec;
		float shin;

		in >> name;
		in >> amb.r >> amb.g >> amb.b;
		in >> dif.r >> dif.g >> dif.b;
		in >> spec.r >> spec.g >> spec.b;
		in >> shin;

		voxel_materials.insert(std::pair<string, VoxelMaterial>(name, VoxelMaterial(amb, dif, spec, shin * 128)));
	}

	return voxel_materials;
}

VoxelMaterial::VoxelMaterial()
{
	this->ambient = glm::vec3(0.0f);
	this->diffuse = glm::vec3(0.0f);
	this->specular = glm::vec3(0.0f);
	this->shininess = 0.0f;
}

VoxelMaterial::VoxelMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
}

void VoxelMaterial::SetToShader(Shader shader, string name)
{
	shader.setVec3(name + ".ambient", ambient);
	shader.setVec3(name + ".diffuse", diffuse);
	shader.setVec3(name + ".specular", specular);

	shader.setFloat(name + ".shininess", shininess);
}