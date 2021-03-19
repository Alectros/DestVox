#ifndef SCENE_H
#define SCENE_H

#include "Tools/Camera.h"
#include "Tools/Model.h"
#include "Tools/Light.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Scene
{
public:
	glm::vec3 backgroundColor;
	Model *ground;
	Model **staticObstacles;
	DirectedLight dirLight;
	Light** lights;

	Scene()
	{
		backgroundColor = glm::vec3(0.5);
		ground = NULL;
		staticObstacles = NULL;
		lights = NULL;
	}

	void DrawStaticScene(Shader shader)
	{
		if (ground != NULL)
		{
			(*ground).Draw(shader);
		}
	}
	
};


#endif