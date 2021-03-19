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

struct RotateInfo
{
	glm::vec3 vector;
	float angle;
};

class World3DObject
{
protected:
	Model model3D;
	//Collision &collision;
	Shader shader;
	glm::vec3 position;
	vector<RotateInfo> rotation;
	glm::vec3 scale;

public:
	World3DObject(string path,Shader shader)
	{
		Model newM(path);
		this->model3D = newM;
		//this->collision = collision;
		this->shader = shader;
		this->position = glm::vec3(1.0f);
		this->scale = glm::vec3(1.0f);
	}

	void Draw(Camera camera, ProgramParams params)
	{
		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		
		for (int i = 0; i < rotation.size(); i++)
			model = glm::rotate(model, rotation[i].angle, rotation[i].vector);

		model = glm::scale(model, scale);
		shader.setMat4("model", model);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)params.SCR_WIDTH / (float)params.SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection",projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		model3D.Draw(shader);
	}

	void Draw(Camera camera, ProgramParams &params, DirectedLight &dLight, vector<PointLight> &pLights, vector<SpotLight> &sLights)
	{
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);

		for (int i = 0; i < rotation.size(); i++)
			model = glm::rotate(model, rotation[i].angle, rotation[i].vector);

		model = glm::scale(model, scale);
		shader.setMat4("model", model);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)params.SCR_WIDTH / (float)params.SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
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
		
		model3D.Draw(shader);
	}

	void Translation(double x, double y, double z)
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
		RotateInfo a;
		a.angle = angle;
		a.vector = vector;
		rotation.push_back(a);
	}
	void Rotation(float angle, float Ox, float Oy, float Oz)
	{
		Rotation(angle, glm::vec3(Ox, Oy, Oz));
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

#endif