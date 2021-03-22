#ifndef WORLD3DOBJECT_H
#define WORLD3DOBJECT_H

#include "Tools/Model.h"
#include "Tools/Shader.h"
#include "Tools/Light.h"
#include "Collision.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class World3DObject
{
protected:
	//Collision &collision;
	Shader shader;
	glm::vec3 position;
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::vec3 scale;

	void VertexShaderMatrixSet(Camera camera, ProgramParams params)
	{
		glm::mat4 model = rotation;
		model = glm::translate(model, position);
		model = glm::scale(model, scale);
		shader.setMat4("model", model);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)params.SCR_WIDTH / (float)params.SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
	}

public:
	string name;

	World3DObject(Shader shader,string name)
	{
		//this->collision = collision;
		this->shader = shader;
		this->position = glm::vec3(1.0f);
		this->rotation = glm::mat4(1.0f);
		this->scale = glm::vec3(1.0f);
		this->name = name;
	}

	World3DObject(Shader shader, string name, glm::vec3 position, glm::vec3 normal, glm::vec3 scale)
	{
		//this->collision = collision;
		this->shader = shader;
		this->position = position;
		this->rotation = glm::mat4(1.0f);
		RotationToVector(normal);
		this->scale = scale;
		this->name = name;
	}

	virtual void Draw(Camera camera, ProgramParams params)
	{
		shader.use();
		VertexShaderMatrixSet(camera, params);
	}

	virtual void Draw(Camera camera, ProgramParams &params, DirectedLight &dLight, vector<PointLight> &pLights, vector<SpotLight> &sLights)
	{
		shader.use();
		VertexShaderMatrixSet(camera, params);

		dLight.AddToShader(shader,"directedlight");

		PointLight::lightCount = pLights.size();
		for (int i = 0; i < PointLight::lightCount; i++)
			pLights[i].AddToShaderI(shader, "pointlights", i);
		PointLight::AddCount(shader, "COUNT_POINT_LIGHTS");

		SpotLight::lightCount = sLights.size();
		for (int i = 0; i < sLights.size(); i++)
			sLights[i].AddToShaderI(shader, "spotLights", i);
		SpotLight::AddCount(shader,"COUNT_SPOTLIGHTS");

		shader.setFloat("shininess", 32.0f);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("faceColor", glm::vec3(0.7));
	}

	void Move(double x, double y, double z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
	}

	void MoveTo(double x, double y, double z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void Rotation(float angle, glm::vec3 vector)
	{
		rotation = glm::rotate(rotation, angle, vector);
	}

	void Rotation(float angle, float Ox, float Oy, float Oz)
	{
		Rotation(angle, glm::vec3(Ox, Oy, Oz));
	}

	void RotationToVector(glm::vec3 vector)
	{
		vector = glm::normalize(vector);
		glm::vec3 objNormal = (glm::vec3)(rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		float angle = glm::acos( glm::dot(objNormal, vector));
		glm::vec3 normal = glm::cross(objNormal, vector);
		if (normal != glm::vec3(0.0f))
			rotation = glm::rotate(rotation, angle, normal);
	}

	void RotationToVector(float Ox, float Oy, float Oz)
	{
		RotationToVector(glm::vec3(Ox, Oy, Oz));
	}

	void Scale(double x, double y, double z)
	{
		scale.x *= x;
		scale.y *= y;
		scale.z *= z;
	}

	void ScaleTo(double x, double y, double z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}
};

class SingleObject :World3DObject
{
private:
	Model** models;
	int count;

public:

	SingleObject(Shader shader, string name) :World3DObject(shader,name)
	{
		models = NULL;
		count = 0;
	}

	SingleObject(Shader shader, string name, string path) :World3DObject(shader, name)
	{
		models = new Model*[1];
		models[0] = new Model(path);
		count = 1;
	}

	SingleObject(Shader shader, string name, string path, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		models = new Model*[1];
		models[0] = new Model(path);
		count = 1;
	}

	SingleObject(Shader shader, string name, Model* model, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		models = new Model * [1];
		models[0] = model;
		count = 1;
	}

	void Draw(Camera camera, ProgramParams params)
	{
		World3DObject::Draw(camera, params);
		for (int i = 0; i < count; i++)
			(*models[i]).Draw(shader);
	}

	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights)
	{
		World3DObject::Draw(camera, params, dLight, pLights, sLights);
		for (int i = 0; i < count; i++)
			(*models[i]).Draw(shader);
	}
};

class CombinedObject :World3DObject
{
protected:
	SingleObject** parts;
	int count;

public:

	CombinedObject(Shader shader, string name) :World3DObject(shader, name)
	{
		parts = NULL;
	}

	void Draw(Camera camera, ProgramParams params)
	{
		World3DObject::Draw(camera, params);
		for (int i = 0; i < count; i++)
			(*parts[i]).Draw(camera,params);
	}

	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights)
	{
		World3DObject::Draw(camera, params, dLight, pLights, sLights);
		for (int i = 0; i < count; i++)
			(*parts[i]).Draw(camera, params, dLight, pLights, sLights);
	}
};

#endif