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
#include <vector>

enum Status{
	STATE_TO_FACE = 0,
	STATE_TO_LINE = 1,
	STATE_MAX = 2,
	STATE_MIN = 3
};

class World3DObject
{
protected:
	//Collision &collision;
	Shader shader;
	glm::vec3 position = glm::vec3(1.0f);
	glm::quat qrotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 right = glm::vec3(-1.0f, 0.0f, 0.0f);
	float pitch = 0.0f;//тангаж
	float yaw = 0.0f;//рысканье
	float roll = 0.0f;//крен
	glm::vec3 scale = glm::vec3(1.0f);;

	void VertexShaderMatrixSet(Camera camera, ProgramParams params, glm::mat4 objModel = glm::mat4(1.0f), glm::mat4 objProj = glm::mat4(1.0f), glm::mat4 objView = glm::mat4(1.0f))
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, scale);

		qrotation = glm::normalize(qrotation);
		glm::mat4 rotation = glm::toMat4(qrotation);
		model = rotation * model;
		model = glm::translate(model, position);
		shader.setMat4("model", model);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)params.SCR_WIDTH / (float)params.SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", objProj*projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", objView*view);
	}

	void Restruct()
	{
		glm::mat3 A = glm::toMat3(qrotation);
		up = glm::normalize(A * glm::vec3(0.0f, 1.0f, 0.0f));
		front = glm::normalize(A * glm::vec3(0.0f, 0.0f, 1.0f));
		right = glm::normalize(A * glm::vec3(-1.0f, 0.0f, 0.0f));

		glm::vec3 OX(1.0f, 0.0, 0.0);
		glm::vec3 OY(0.0f, 1.0, 0.0);
		glm::vec3 OZ(0.0f, 0.0, -1.0);

		glm::vec3 XY = glm::vec3(up.x, up.y, 0.0f);
		if (XY == glm::vec3(0.0f))
			XY = OY;
		XY = glm::normalize(XY);

		glm::vec3 YZ = glm::vec3(0.0f, up.y, up.z);
		if (YZ == glm::vec3(0.0f))
			YZ = OY;
		YZ = glm::normalize(YZ);

		glm::vec3 XZ = glm::vec3(up.x, 0.0f, up.z);
		if (XZ == glm::vec3(0.0f))
			XZ = OX;
		XZ = glm::normalize(XZ);


		//troubles with Eilers
		if (IsLCS(up, OX, OZ))
			pitch = glm::degrees(glm::acos(glm::dot(XY, OY)));
		else
			pitch = glm::degrees(glm::acos(glm::dot(XY, OY)));

		if (IsLCS(up, OY, OX))
			roll = glm::degrees(glm::acos(glm::dot(YZ, OY)));
		else
			roll = glm::degrees(glm::acos(glm::dot(YZ, OY)));

		if (IsLCS(up, OY, OX))
			yaw = glm::degrees(glm::acos(glm::dot(XZ, OX)));
		else
			yaw = glm::degrees(glm::acos(glm::dot(XZ, OX)));
	}

	bool IsLCS(glm::vec3 first, glm::vec3 second,glm::vec3 third)
	{
		glm::mat3 A(first, second, third);
		return glm::determinant(glm::transpose(A)) > 0;
	}

	void CorrectSC()
	{		
		if (up.y <=0.0f)
		{
			glm::vec3 tekRight = glm::toMat3(qrotation) * glm::vec3(-1.0f, 0.0f, 0.0f);
			float vy = (-1 * (up.x * right.x) - (up.z * right.z)) / up.y;

			glm::vec3 realRight(right.x, vy, right.z);

			if (realRight.x * tekRight.x < 0.0f)
			{
				Rotation(180.0f);
			}
		}
	}

public:
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
		RotationToVector(normal);
		this->scale = scale;
		this->name = name;
	}

	virtual void Draw(Camera camera, ProgramParams params, glm::mat4 objModel = glm::mat4(1.0f), glm::mat4 objProj = glm::mat4(1.0f), glm::mat4 objView = glm::mat4(1.0f))
	{
		shader.use();
		VertexShaderMatrixSet(camera, params, objModel, objProj, objView);
	}

	virtual void Draw(Camera camera, ProgramParams &params, DirectedLight &dLight, vector<PointLight> &pLights, vector<SpotLight> &sLights, glm::mat4 objModel = glm::mat4(1.0f), glm::mat4 objProj = glm::mat4(1.0f), glm::mat4 objView = glm::mat4(1.0f))
	{
		shader.use();
		VertexShaderMatrixSet(camera, params, objModel, objProj, objView);

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
		shader.setVec3("faceColor", glm::vec3(0.5,0.5,1.0));
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

	void RotationEilers(float p,float y, float r)
	{

	}

	void RotateToFrontVector(glm::vec3 setFront, int state = STATE_TO_FACE, int stateType = STATE_MIN)//Rotate object to front vector
	{
		if (up.y != 0.0f)
		{
			float vy = (-1 * (up.x * setFront.x) - (up.z * setFront.z)) / up.y;
			glm::vec3 realFront = glm::normalize(glm::vec3(setFront.x, vy, setFront.z));

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
					Rotation(angle);
				}
				else
				{
					Rotation(-angle);
				}
				Restruct();
			}
		}
	}

	void RotateToFrontVector(float x,float y, float z, int state = STATE_TO_FACE, int stateType = STATE_MIN)
	{
		RotateToFrontVector(glm::vec3(x, y, z), state, stateType);
	}

	void Rotation(float angle, glm::vec3 vector)
	{
		vector = glm::normalize(vector);

		float sinhangle = glm::sin(glm::radians(angle / 2));
		float coshangle = glm::cos(glm::radians(angle / 2));

		glm::quat a(coshangle, vector.x * sinhangle, vector.y * sinhangle, vector.z * sinhangle);

		this->qrotation = a * this->qrotation;
		Restruct();
	}

	void Rotation(float angle, float Ox, float Oy, float Oz)
	{
		Rotation(angle, glm::vec3(Ox, Oy, Oz));
	}

	void Rotation(float angle)
	{
		Rotation(angle, this->up);
	}

	void RotationToVector(glm::vec3 vector)
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

class SingleObject :public World3DObject
{
private:
	Model* model;

public:
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

	void Draw(Camera camera, ProgramParams params, glm::mat4 objModel = glm::mat4(1.0f), glm::mat4 objProj = glm::mat4(1.0f), glm::mat4 objView = glm::mat4(1.0f))
	{
		World3DObject::Draw(camera, params, objModel, objProj, objView);
		(*model).Draw(shader);
	}

	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights, glm::mat4 objModel = glm::mat4(1.0f), glm::mat4 objProj = glm::mat4(1.0f), glm::mat4 objView = glm::mat4(1.0f))
	{
		World3DObject::Draw(camera, params, dLight, pLights, sLights, objModel, objProj, objView);
		(*model).Draw(shader);
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
		if (!sorted)
		{
			Sort();
		}

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, scale);

		for (int i = 0; i < parts.size(); i++)
			(*parts[i]).Draw(camera, params, model);
	}

	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight>& pLights, vector<SpotLight>& sLights)
	{
		if (!sorted)
		{
			Sort();
		}

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, scale);

		for (int i = 0; i < parts.size(); i++)
			(*parts[i]).Draw(camera, params, dLight, pLights, sLights, model);
	}
};
#endif