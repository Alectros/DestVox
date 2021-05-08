#ifndef VOXELOBJECT_H
#define VOXELOBJECT_H

#include "World3DObject.h"
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


class VoxelObject : public World3DObject
{
public:
	OctreeNode<Voxel>* octo;
	Model* voxelCubeModel;
	vector<Voxel*> shell;

	VoxelObject(Shader shader, string name);
	VoxelObject(Shader shader, string name, Model* voxcube);
	VoxelObject(Shader shader, string name, Model* voxcube, OctreeNode<Voxel>* octo);
	VoxelObject(Shader shader, string name, Model* voxcube, OctreeNode<Voxel>* octo, glm::vec3 position, glm::vec3 normal, glm::vec3 scale);
	VoxelObject(Shader shader, string name, Model* voxcube, OctreeNode<Voxel>* octo, vector<Voxel*> shell, glm::vec3 position, glm::vec3 normal, glm::vec3 scale);

	void SetShell();
	void Draw(Camera camera, ProgramParams& params);//prepares and translates object's data to the shader without lights
	void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel);
	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights);//prepares and translates object's data to the shader with 3 type of lights(Directed light, point light and spotlights)
	void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights);
	void SetInitialMovement(glm::vec3 position);//set initial rotate quaterion without changing origin state
	void SetInitialDirection(glm::vec3 front);
	void SetInitialScale(glm::vec3 scales);//set initial rotate quaterion without changing origin state
};


#endif