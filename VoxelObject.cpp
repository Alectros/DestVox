#include "World3DObject.h"
#include "VoxelObject.h"
#include "Octree.h"
#include "Voxel.h"
#include "Tools/Model.h"
#include "Tools/Shader.h"
#include "Tools/Light.h"
#include "Collision.h"
#include "ProgramParams.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vector>
#include <math.h>
#include <fstream>


VoxelObject::VoxelObject(Shader shader, string name) :World3DObject(shader, name)
{
	this->octo = NULL;
	this->voxelCubeModel = NULL;
}

VoxelObject::VoxelObject(Shader shader, string name, Model*voxcube) : World3DObject(shader, name)
{
	this->octo = NULL;
	this->voxelCubeModel = voxcube;
}

VoxelObject::VoxelObject(Shader shader, string name, Model* voxcube, OctreeNode<Voxel>* octo) : World3DObject(shader, name)
{
	this->octo = octo;
	this->voxelCubeModel = voxcube;
}
VoxelObject::VoxelObject(Shader shader, string name, Model* voxcube, OctreeNode<Voxel>* octo, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) : World3DObject(shader, name, position, normal, scale)
{
	this->octo = octo;
	this->voxelCubeModel = voxcube;
}
VoxelObject::VoxelObject(Shader shader, string name, Model* voxcube, OctreeNode<Voxel>* octo, vector<Voxel*> shell, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) : World3DObject(shader, name, position, normal, scale)
{
	this->octo = octo;
	this->voxelCubeModel = voxcube;
	this->shell = shell;
}

void VoxelObject::SetShell()
{
	this->shell.clear();
	octo->GetShell((OctreeBase<Voxel>*)octo,this->shell);
}

void VoxelObject::Draw(Camera camera, ProgramParams& params)
{
	shader.use();
	glm::mat4 objmodel = GetModelMatrix();
	glm::mat4 projection = GetProjectionMatrix(camera, params);
	glm::mat4 view = camera.GetViewMatrix();

	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setVec3("viewPos", camera.Position);
	for (int i = 0; i < shell.size(); i++)
	{
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(shell[i]->size));
		model = glm::translate(model, shell[i]->position);
		shader.setMat4("model", objmodel * model);
		shell[i]->material.SetToShader(shader, "material");

		shell[i]->Draw(*(this->voxelCubeModel), shader);
	}
}
void VoxelObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel)
{
	shader.use();
	glm::mat4 objmodel = GetModelMatrix();
	glm::mat4 projection = GetProjectionMatrix(camera, params);
	glm::mat4 view = camera.GetViewMatrix();

	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setVec3("viewPos", camera.Position);
	for (int i = 0; i < shell.size(); i++)
	{
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(shell[i]->size));
		model = glm::translate(model, shell[i]->position);
		shader.setMat4("model", objModel * objmodel * model);
		shell[i]->material.SetToShader(shader, "material");

		shell[i]->Draw(*(this->voxelCubeModel), shader);
	}
}
void VoxelObject::Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
{
	SetLights(dLight, pLights, sLights);
	this->Draw(camera, params);
}
void VoxelObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
{
	SetLights(dLight, pLights, sLights);
	this->Draw(camera, params, objModel);
}
void VoxelObject::SetInitialMovement(glm::vec3 position)
{

}
void VoxelObject::SetInitialDirection(glm::vec3 front)
{

}
void VoxelObject::SetInitialScale(glm::vec3 scales)
{

}