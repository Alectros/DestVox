#ifndef SINGLEOBJECT3D_H
#define SINGLEOBJECT3D_H

#include "Tools/Model.h"
#include "Tools/Shader.h"
#include "Tools/Light.h"
#include "Collision.h"
#include "World3DObject.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class SingleObject :World3DObject
{
private: 
	Model** models;

public:
	SingleObject()
	{
		models = NULL;
	}

};


#endif