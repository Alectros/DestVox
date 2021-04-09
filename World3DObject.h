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
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vector>
#include <math.h>

#include <fstream>

enum Status{
	STATE_TO_FACE = 0,
	STATE_TO_LINE,
	STATE_MAX,
	STATE_MIN,
	STATE_AROUND_X,
	STATE_AROUND_Y,
	STATE_AROUND_Z,
	STATE_AROUND_ALL
};

ofstream file("out.txt");

class World3DObject
{
public:
	//Collision &collision;
	Shader shader;
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat qrotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat initial = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 right = glm::vec3(-1.0f, 0.0f, 0.0f);
	float pitch = 0.0f;//тангаж
	float yaw = 0.0f;//рысканье
	float roll = 0.0f;//крен
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 GetProjectionMatrix(Camera camera, ProgramParams params)
	{
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)params.SCR_WIDTH / (float)params.SCR_HEIGHT, 0.1f, 100.0f);
		return projection;
	}

	void SetLights(DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights)
	{
		dLight.AddToShader(shader, "directedlight");

		PointLight::lightCount = pLights.size();
		for (int i = 0; i < PointLight::lightCount; i++)
			pLights[i].AddToShaderI(shader, "pointlights", i);
		PointLight::AddCount(shader, "COUNT_POINT_LIGHTS");

		SpotLight::lightCount = sLights.size();
		for (int i = 0; i < sLights.size(); i++)
			sLights[i].AddToShaderI(shader, "spotLights", i);
		SpotLight::AddCount(shader, "COUNT_SPOTLIGHTS");
	}

	void Restruct()
	{
		qrotation = glm::normalize(qrotation);
		glm::mat3 A = glm::toMat3(qrotation);
		up = glm::normalize(A * glm::vec3(0.0f, 1.0f, 0.0f) );
		front = glm::normalize(A * glm::vec3(0.0f, 0.0f, 1.0f));
		right = glm::normalize(A * glm::vec3(-1.0f, 0.0f, 0.0f));

		pitch = glm::degrees(glm::pitch(qrotation));
		yaw = glm::degrees(glm::yaw(qrotation));
		roll = glm::degrees(glm::roll(qrotation));
	}

	bool IsLCS(glm::vec3 first, glm::vec3 second,glm::vec3 third)
	{
		glm::mat3 A(first, second, third);
		return glm::determinant(glm::transpose(A)) > 0;
	}

	glm::vec3 IntersectSquareLinePoint(glm::vec3 sqN, glm::vec3 p)
	{
		float lya = -1 * (sqN.x * p.x + sqN.y * p.y + sqN.z * p.z) / (sqN.x * sqN.x + sqN.y * sqN.y + sqN.z * sqN.z);
		glm::vec3 interp(lya * sqN.x + p.x, lya * sqN.y + p.y, lya * sqN.z + p.z);
		return interp;
	}

//public:

	glm::mat4 GetModelMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, position);
		qrotation = glm::normalize(qrotation);
		glm::mat4 rotation = glm::toMat4(qrotation * initial);
		model = model * rotation;
		model = glm::scale(model, scale);
		return model;
	}

	string name;

	World3DObject(Shader shader,string name)
	{
		//this->collision = collision;
		this->shader = shader;

		Restruct();
		this->name = name;
	}

	World3DObject(Shader shader, string name, glm::vec3 position, glm::vec3 normal, glm::vec3 scale)
	{
		//this->collision = collision;
		this->shader = shader;
		this->position = position;
		SetUpVector(normal);
		this->scale = scale;
		this->name = name;
	}

	virtual void Draw(Camera camera, ProgramParams params, glm::mat4 objModel = glm::mat4(1.0f), glm::mat4 objProj = glm::mat4(1.0f), glm::mat4 objView = glm::mat4(1.0f))
	{
		shader.use();

		glm::mat4 model = GetModelMatrix();

		shader.setMat4("model", model);

		glm::mat4 projection = GetProjectionMatrix(camera, params);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
	}

	virtual void Draw(Camera camera, ProgramParams &params, DirectedLight &dLight, vector<PointLight> &pLights, vector<SpotLight> &sLights, glm::mat4 objModel = glm::mat4(1.0f), glm::mat4 objProj = glm::mat4(1.0f), glm::mat4 objView = glm::mat4(1.0f))
	{
		shader.use();	
		glm::mat4 model = GetModelMatrix();

		shader.setMat4("model", model);

		glm::mat4 projection = GetProjectionMatrix(camera, params);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		SetLights(dLight, pLights, sLights);

		shader.setFloat("shininess", 32.0f);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("faceColor", glm::vec3(0.5,0.5,1.0));
	}

	void SetInitial(glm::vec3 frontV)
	{
		glm::vec3 realFront = glm::normalize(IntersectSquareLinePoint(up, frontV));

		float angle = glm::degrees(glm::acos(glm::dot(realFront, front)));

		float currAngle = glm::degrees(glm::acos(glm::dot(frontV, realFront)));
		glm::quat a1(1.0f, 0.0f, 0.0f, 0.0f);

		if (!isnan(angle))
		{
			if (IsLCS(up, realFront, front))
			{
				angle *= -1;
			}

			glm::vec3 upV(0.0f, 1.0f, 0.0);

			float sinhangle = glm::sin(glm::radians(angle / 2));
			float coshangle = glm::cos(glm::radians(angle / 2));

			a1 = glm::quat(coshangle, upV.x * sinhangle, upV.y * sinhangle, upV.z * sinhangle);
		}

		initial = a1 * initial;
	}

	void LookDirection(glm::vec3 direction)
	{
		direction = glm::normalize(direction);

		glm::vec3 frontProj = glm::normalize(IntersectSquareLinePoint(glm::vec3(0.0f, 1.0f, 0.0f), front));
		glm::vec3 directionProj = glm::normalize(IntersectSquareLinePoint(glm::vec3(0.0f, 1.0f, 0.0f), direction));

		float angle = glm::degrees(glm::acos(glm::dot(frontProj, directionProj)));

		if (isnan(angle))
		{
			angle = 0.0f;
		}

		if (IsLCS(directionProj, glm::vec3(0.0f, 1.0f, 0.0f), frontProj))
		{
			RotationAxes(angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			RotationAxes(-angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		glm::vec3 crossV = glm::normalize( glm::cross(direction, front));

		if (crossV == glm::vec3(0.0f) || isnan(crossV.x) || isnan(crossV.y) || isnan(crossV.z))
		{
			crossV = right;
		}

		angle = glm::degrees(glm::acos(glm::dot(direction, front)));

		if (isnan(angle))
		{
			angle = 0.0f;
		}

		if (!IsLCS(crossV, direction, front))
		{
			RotationAxes(angle, crossV);
		}
		else
		{
			RotationAxes(-angle, crossV);
		}

	}

	void LookPoint(glm::vec3 point)
	{
		LookDirection(point - position);
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

	void RotateToDegrees(float p,float y, float r)
	{
		qrotation = glm::eulerAngleXYZ(glm::radians(p), glm::radians(y), glm::radians(r));

		Restruct();
	}

	void RotateDegrees(float p, float y, float r)
	{
		RotateToDegrees(pitch + p, yaw + y, roll + r);
	}

	void LookToYaw(glm::vec3 setFront, int state = STATE_TO_FACE, int stateType = STATE_MIN)//Rotate object to front vector
	{
		glm::vec3 realFront = glm::normalize(IntersectSquareLinePoint(up,setFront));

		float angle = glm::degrees(glm::acos(glm::dot(realFront, front)));

		if (state == STATE_TO_LINE)
		{
			glm::vec3 realFrontMir(-1.0f * realFront);
			float angle2 = glm::degrees(glm::acos(glm::dot(realFrontMir, front)));
			if (angle2 < angle && stateType == STATE_MIN || angle2 > angle && stateType == STATE_MAX)
			{
				angle = angle2;
				realFront = realFrontMir;
			}
		}

		float currAngle = glm::degrees(glm::acos(glm::dot(setFront, realFront)));
		if (state == STATE_TO_FACE && ((currAngle > 90.0f && stateType == STATE_MIN) || (currAngle < 90.0f && stateType == STATE_MAX)))
		{
			angle += 180.0f;
		}

		if (!isnan(angle))
		{
			if (!IsLCS(up, realFront, front))
			{
				RotationAxes(angle);
			}
			else
			{
				RotationAxes(-angle);
			}
		}
	}

	void LookToYaw(float x,float y, float z, int state = STATE_TO_FACE, int stateType = STATE_MIN)
	{
		LookToYaw(glm::vec3(x, y, z), state, stateType);
	}

	void LookPointYaw(glm::vec3 point, int state = STATE_TO_FACE, int stateType = STATE_MIN)
	{
		LookToYaw(point - position, state, stateType);
	}

	void RotationAxes(float angle, glm::vec3 vector)
	{
		vector = glm::normalize(vector);

		float sinhangle = glm::sin(glm::radians(angle / 2));
		float coshangle = glm::cos(glm::radians(angle / 2));

		glm::quat a(coshangle, vector.x * sinhangle, vector.y * sinhangle, vector.z * sinhangle);

		this->qrotation = a * this->qrotation;
		Restruct();
	}

	void RotationAxes(float angle, float Ox, float Oy, float Oz)
	{
		RotationAxes(angle, glm::vec3(Ox, Oy, Oz));
	}

	void RotationAxes(float angle)
	{
		RotationAxes(angle, this->up);
	}

	void SetUpVector(glm::vec3 vector)
	{
		vector = glm::normalize(vector);
		glm::vec3 currRight(0.0f, 0.0f, 1.0f);
		glm::vec3 currup(0.0f, 1.0f, 0.0f);

		float angle = glm::degrees(glm::acos(glm::dot(currup, vector)));

		glm::vec3 crossvector = glm::cross(currup, vector);
		if (crossvector != glm::vec3(0.0f))
		{
			crossvector = glm::normalize(crossvector);

			float sinhangle = glm::sin(glm::radians(angle / 2));
			float coshangle = glm::cos(glm::radians(angle / 2));

			glm::quat a(coshangle, crossvector.x * sinhangle, crossvector.y * sinhangle, crossvector.z * sinhangle);
			this->qrotation = a;
		}
		else if (vector.y < 0)
		{
			crossvector = glm::normalize(currRight);

			float sinhangle = glm::sin(glm::radians(180.0f / 2));
			float coshangle = glm::cos(glm::radians(180.0f / 2));

			glm::quat a(coshangle, crossvector.x * sinhangle, crossvector.y * sinhangle, crossvector.z * sinhangle);
			this->qrotation = a;
		}
		Restruct();
	}

	void SetUpVector(float Ox, float Oy, float Oz)
	{
		SetUpVector(glm::vec3(Ox, Oy, Oz));
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



class SingleObject :public World3DObject
{
public:
	Model* model;

	string name;

	SingleObject(Shader shader, string name) :World3DObject(shader,name)
	{
		model = NULL;
	}

	SingleObject(Shader shader, string name, string path) :World3DObject(shader, name)
	{
		model = new Model(path);
	}

	SingleObject(Shader shader, string name, string path, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		model = new Model(path);
	}

	SingleObject(Shader shader, string name, Model* model, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		this->model = model;
	}

	void Draw(Camera camera, ProgramParams params)
	{
		shader.use();
		glm::mat4 modelMX = GetModelMatrix();

		shader.setMat4("model", modelMX);

		glm::mat4 projection = GetProjectionMatrix(camera, params);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		shader.setFloat("shininess", model->shininess);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("faceColor", model->faceColor);

		model->Draw(shader);
	}

	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights)
	{
		shader.use();
		glm::mat4 modelMX = GetModelMatrix();

		shader.setMat4("model", modelMX);

		glm::mat4 projection = GetProjectionMatrix(camera, params);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		SetLights(dLight, pLights, sLights);

		shader.setFloat("shininess", model->shininess);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("faceColor", model->faceColor);

		model->Draw(shader);
	}
};

bool Comp(SingleObject* a, SingleObject* b)
{
	return strcmp(a->name.c_str(), b->name.c_str()) < 0;
}

class CombinedObject :public World3DObject
{
protected:
	vector<SingleObject*> parts;
	bool sorted = 0;

public:

	CombinedObject(Shader shader, string name) :World3DObject(shader, name)
	{
	}

	CombinedObject(Shader shader, string name, SingleObject* obj, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		parts.push_back(obj);
	}

	CombinedObject(Shader shader, string name, vector<SingleObject*> model, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		parts = model;
	}

	void AddObject(SingleObject* obj)
	{
		parts.push_back(obj);
		sorted = 0;
	}

	void Sort()
	{
		sort(parts.begin(), parts.end(), Comp);
		sorted = 1;
	}

	void Draw(Camera camera, ProgramParams params)
	{
		glm::mat4 modelMXComb = GetModelMatrix();

		for (int i = 0; i < parts.size(); i++)
		{
			parts[i]->shader.use();
			glm::mat4 modelMX = parts[i]->GetModelMatrix();

			shader.setMat4("model", modelMXComb * modelMX);

			glm::mat4 projection = GetProjectionMatrix(camera, params);
			shader.setMat4("projection", projection);

			glm::mat4 view = camera.GetViewMatrix();
			shader.setMat4("view", view);

			shader.setFloat("shininess",parts[i]->model->shininess);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("faceColor", parts[i]->model->faceColor);

			parts[i]->model->Draw(parts[i]->shader);
		}
	}

	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights)
	{
		if (!sorted)
		{
			//Sort();
		}

		glm::mat4 modelMXComb = GetModelMatrix();
		
		for (int i = 0; i < parts.size(); i++)
		{
			parts[i]->shader.use();
			glm::mat4 modelMX = parts[i]->GetModelMatrix();

			shader.setMat4("model", modelMXComb * modelMX);

			glm::mat4 projection = GetProjectionMatrix(camera, params);
			shader.setMat4("projection", projection);

			glm::mat4 view = camera.GetViewMatrix();
			shader.setMat4("view", view);

			SetLights(dLight, pLights, sLights);

			shader.setFloat("shininess", parts[i]->model->shininess);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("faceColor", parts[i]->model->faceColor);

			parts[i]->model->Draw(parts[i]->shader);
		}
	}
};
#endif