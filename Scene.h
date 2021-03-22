#ifndef SCENE_H
#define SCENE_H

#include "Tools/Camera.h"
#include "Tools/Model.h"
#include "Tools/Light.h"
#include "World3DObject.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Scene
{
public:
	glm::vec3 backgroundColor;
	World3DObject *ground;
	World3DObject **staticObjects;
	World3DObject **dynamicObjects;
	DirectedLight dirLight;
	Light** lights;

	Scene()
	{
		backgroundColor = glm::vec3(0.5);
		ground = NULL;
		staticObjects = NULL;
		dynamicObjects = NULL;
		lights = NULL;
	}

	void DrawStaticScene(Shader shader)
	{
		if (ground != NULL)
		{
			//(*ground).Draw(shader);
		}
	}
	
};


#endif