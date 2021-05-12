#include "Tools/Model.h"
#include "Tools/Shader.h"
#include "Tools/Light.h"
#include "Collision.h"
#include "World3DObject.h"
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

	//calculated a projection matrix
	glm::mat4 World3DObject::GetProjectionMatrix(Camera camera, ProgramParams params)
	{
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)params.SCR_WIDTH / (float)params.SCR_HEIGHT, 0.1f, 100.0f);
		return projection;
	}

	//Add lights to the objects
	void World3DObject::SetLights(DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
	{
		dLight.AddToShader(shader, "directedlight");

		for (int i = 0; i < pLights.size(); i++)
			pLights[i]->AddToShaderI(shader, "pointlights", i);
		PointLight::AddCount(shader, "COUNT_POINT_LIGHTS", pLights.size());

		for (int i = 0; i < sLights.size(); i++)
			sLights[i]->AddToShaderI(shader, "spotLights", i);
		SpotLight::AddCount(shader, "COUNT_SPOTLIGHTS", sLights.size());
	}

	//using the rotation quaterion gets the current values of the vectors and angles
	void World3DObject::Restruct()
	{
		qrotation = glm::normalize(qrotation);
		glm::mat3 A = glm::toMat3(qrotation);

		up = glm::normalize(A * glm::vec3(0.0f, 1.0f, 0.0f));
		front = glm::normalize(A * glm::vec3(0.0f, 0.0f, 1.0f));
		right = glm::normalize(A * glm::vec3(-1.0f, 0.0f, 0.0f));

		pitch = glm::degrees(glm::pitch(qrotation));
		yaw = glm::degrees(glm::yaw(qrotation));
		roll = glm::degrees(glm::roll(qrotation));
	}

	//determines which coordinate system the 3 vectors belong to
	bool World3DObject::IsLCS(glm::vec3 first, glm::vec3 second, glm::vec3 third)
	{
		glm::mat3 A(first, second, third);
		return glm::determinant(glm::transpose(A)) > 0;
	}

	//calculated the projection of the vector(p) onto the plate defines by the normal(sqN)
	glm::vec3 World3DObject::IntersectSquareLinePoint(glm::vec3 sqN, glm::vec3 p)
	{
		float lya = -1 * (sqN.x * p.x + sqN.y * p.y + sqN.z * p.z) / (sqN.x * sqN.x + sqN.y * sqN.y + sqN.z * sqN.z);
		glm::vec3 interp(lya * sqN.x + p.x, lya * sqN.y + p.y, lya * sqN.z + p.z);
		return interp;
	}

	//public:

		//calculated the model matrix of the object
	glm::mat4 World3DObject::GetModelMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, position);
		qrotation = glm::normalize(qrotation);
		glm::mat4 rotation = glm::toMat4(qrotation);
		model = model * rotation;
		model = glm::scale(model, scale);
		return model;
	}

	//base constructor
	World3DObject::World3DObject(Shader shader, string name)
	{
		//this->collision = collision;
		this->shader = shader;
		this->name = name;

		Restruct();
	}

	//constructor with position, normal vector and scale
	World3DObject::World3DObject(Shader shader, string name, glm::vec3 position, glm::vec3 normal, glm::vec3 scale)
	{
		//this->collision = collision;
		this->shader = shader;
		this->position = position;
		this->scale = scale;
		this->name = name;
		SetUpVector(normal);

		Restruct();
	}

	//prepares and translates object's data to the shader without lights
	void World3DObject::Draw(Camera camera, ProgramParams& params)
	{
		shader.use();

		glm::mat4 model = GetModelMatrix();

		shader.setMat4("model", model);

		glm::mat4 projection = GetProjectionMatrix(camera, params);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
	}

	void World3DObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel)
	{
		shader.use();

		glm::mat4 model = GetModelMatrix();

		shader.setMat4("model", objModel * model);

		glm::mat4 projection = GetProjectionMatrix(camera, params);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
	}

	//prepares and translates object's data to the shader with 3 type of lights(Directed light, point light and spotlights)
	void World3DObject::Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
	{
		Draw(camera, params);

		SetLights(dLight, pLights, sLights);

		shader.setFloat("shininess", 32.0f);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("faceColor", glm::vec3(0.6));
	}

	void World3DObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
	{
		Draw(camera, params, objModel);

		SetLights(dLight, pLights, sLights);

		shader.setFloat("shininess", 32.0f);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("faceColor", glm::vec3(0.6));
	}


	void World3DObject::SetInitialMovement(glm::vec3 position)
	{}

	void World3DObject::SetInitialDirection(glm::vec3 direction)
	{}
	
	void World3DObject::SetInitialScale(glm::vec3 scale)
	{}

	
	void World3DObject::LookDirection(glm::vec3 direction)
	{
		glm::quat a = _lookDirection(front, right, direction);
		qrotation = a * qrotation;
		Restruct();
	}

	void World3DObject::LookPoint(glm::vec3 point)
	{
		LookDirection(point - position);
	}

	void World3DObject::Move(double x, double y, double z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
	}

	void World3DObject::Move(glm::vec3 pos)
	{
		Move(pos.x, pos.y, pos.z);
	}

	void World3DObject::MoveTo(double x, double y, double z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void World3DObject::MoveTo(glm::vec3 pos)
	{
		MoveTo(pos.x, pos.y, pos.z);
	}

	void World3DObject::RotateToDegrees(float p, float y, float r)
	{
		qrotation = glm::eulerAngleXYZ(glm::radians(p), glm::radians(y), glm::radians(r));

		Restruct();
	}

	void World3DObject::RotateDegrees(float p, float y, float r)
	{
		RotateToDegrees(pitch + p, yaw + y, roll + r);
	}

	void World3DObject::LookToLocked(glm::vec3 setFront, int plateType, int state, int stateType)//Rotate object to front vector
	{
		glm::vec3 rotationAxe(up);
		glm::vec3 plateAxe(front);

		if (plateType == STATE_AROUND_RIGHT)
		{
			rotationAxe = right;
			plateAxe = front;
		}
		if (plateType == STATE_AROUND_FRONT)
		{
			rotationAxe = front;
			plateAxe = up;
		}

		glm::vec3 realFront = glm::normalize(IntersectSquareLinePoint(rotationAxe, setFront));

		float angle = glm::degrees(glm::acos(glm::dot(realFront, plateAxe)));

		if (state == STATE_TO_LINE)
		{
			glm::vec3 realFrontMir(-1.0f * realFront);
			float angle2 = glm::degrees(glm::acos(glm::dot(realFrontMir, plateAxe)));
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
			if (!IsLCS(rotationAxe, realFront, plateAxe))
			{
				RotationAxes(angle, rotationAxe);
			}
			else
			{
				RotationAxes(-angle, rotationAxe);
			}
		}
	}

	void World3DObject::LookToLocked(float x, float y, float z, int plateType, int state, int stateType)
	{
		LookToLocked(glm::vec3(x, y, z), plateType, state, stateType);
	}

	void World3DObject::LookPointLocked(glm::vec3 point,int plateType, int state, int stateType)
	{
		LookToLocked(point - position, plateType, state, stateType);
	}

	void World3DObject::RotationAxes(float angle, glm::vec3 vector)
	{
		glm::quat a = _rotationAxes(angle, vector);
		qrotation = a * qrotation;
		Restruct();
	}

	void World3DObject::RotationAxes(float angle, float Ox, float Oy, float Oz)
	{
		RotationAxes(angle, glm::vec3(Ox, Oy, Oz));
	}

	void World3DObject::RotationAxes(float angle)
	{
		RotationAxes(angle, this->up);
	}

	void World3DObject::SetUpVector(glm::vec3 vector)
	{
		glm::quat a = _setUpVector(vector);
		this->qrotation = a;
		Restruct();
	}

	void World3DObject::SetUpVector(float Ox, float Oy, float Oz)
	{
		SetUpVector(glm::vec3(Ox, Oy, Oz));
	}

	void World3DObject::Scale(double x, double y, double z)
	{
		scale.x *= x;
		scale.y *= y;
		scale.z *= z;
	}

	void World3DObject::ScaleTo(double x, double y, double z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	glm::quat World3DObject::_setUpVector(glm::vec3 vector)
	{
		vector = glm::normalize(vector);
		glm::vec3 currRight(0.0f, 0.0f, 1.0f);
		glm::vec3 currup(0.0f, 1.0f, 0.0f);

		float angle = glm::degrees(glm::acos(glm::dot(currup, vector)));

		glm::quat a(1.0f, 0.0f, 0.0f, 0.0f);

		glm::vec3 crossvector = glm::cross(currup, vector);
		if (crossvector != glm::vec3(0.0f))
		{
			crossvector = glm::normalize(crossvector);

			float sinhangle = glm::sin(glm::radians(angle / 2));
			float coshangle = glm::cos(glm::radians(angle / 2));

			a = glm::quat(coshangle, crossvector.x * sinhangle, crossvector.y * sinhangle, crossvector.z * sinhangle);
		}
		else if (vector.y < 0)
		{
			crossvector = glm::normalize(currRight);

			float sinhangle = glm::sin(glm::radians(180.0f / 2));
			float coshangle = glm::cos(glm::radians(180.0f / 2));

			a = glm::quat(coshangle, crossvector.x * sinhangle, crossvector.y * sinhangle, crossvector.z * sinhangle);
		}
		return a;
	}

	glm::quat World3DObject::_rotationAxes(float angle, glm::vec3 vector)
	{
		vector = glm::normalize(vector);

		float sinhangle = glm::sin(glm::radians(angle / 2));
		float coshangle = glm::cos(glm::radians(angle / 2));

		glm::quat a(coshangle, vector.x * sinhangle, vector.y * sinhangle, vector.z * sinhangle);

		return a;
	}

	//rotate object
	glm::quat World3DObject::_lookDirection(glm::vec3 front, glm::vec3 right, glm::vec3 direction)
	{
		glm::quat a(1.0f, 0.0f, 0.0f, 0.0f);

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
			a = _rotationAxes(angle, glm::vec3(0.0f, 1.0f, 0.0f)) * a;
		}
		else
		{
			a = _rotationAxes(-angle, glm::vec3(0.0f, 1.0f, 0.0f)) * a;
		}

		glm::mat3 A = glm::toMat3(a);

		front = A * front;
		right = A * right;

		glm::vec3 crossV = glm::normalize(glm::cross(direction, front));

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
			a = _rotationAxes(angle, crossV) * a;
		}
		else
		{
			a = _rotationAxes(-angle, crossV) * a;
		}

		return a;
	}





	SingleObject::SingleObject(Shader shader, string name) :World3DObject(shader, name)
	{
		model = NULL;
	}

	SingleObject::SingleObject(Shader shader, string name, string path) :World3DObject(shader, name)
	{
		model = new Model(path);
	}

	SingleObject::SingleObject(Shader shader, string name, Model* model) :World3DObject(shader, name)
	{
		this->model = model;
	}

	SingleObject::SingleObject(Shader shader, string name, string path, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		model = new Model(path);
	}

	SingleObject::SingleObject(Shader shader, string name, Model* model, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		this->model = model;
	}

	void SingleObject::Draw(Camera camera, ProgramParams& params)
	{
		if (model == NULL)
			return;

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

	void SingleObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel)
	{
		if (model == NULL)
			return;

		shader.use();
		glm::mat4 modelMX = GetModelMatrix();

		shader.setMat4("model", objModel*modelMX);

		glm::mat4 projection = GetProjectionMatrix(camera, params);
		shader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		shader.setFloat("shininess", model->shininess);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("faceColor", model->faceColor);

		model->Draw(shader);
	}

	void SingleObject::Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
	{
		if (model == NULL)
			return;

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

	void SingleObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel , DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
	{
		if (model == NULL)
			return;
		shader.use();
		glm::mat4 modelMX = GetModelMatrix();

		shader.setMat4("model", objModel * modelMX);

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

	void SingleObject::SetInitialMovement(glm::vec3 movement)
	{
		if (model == NULL)
			return;
		for (int i = 0; i < model->meshes.size(); i++)
		{
			if (model != NULL)
				model->meshes[i]->Move(movement);
		}
	}
	void SingleObject::SetInitialDirection(glm::vec3 direction)
	{
		if (model == NULL)
			return;
		glm::mat3 A = glm::toMat3(_lookDirection(front, right,direction));
		for (int i = 0; i < model->meshes.size(); i++)
		{
			if (model != NULL)
				model->meshes[i]->Rotate(A);
		}
	}
	void SingleObject::SetInitialScale(glm::vec3 scale)
	{
		if (model == NULL)
			return;
		for (int i = 0; i < model->meshes.size(); i++)
		{
			if (model != NULL)
				model->meshes[i]->Scale(scale);
		}
	}

bool CompW3D(World3DObject* a, World3DObject* b)
{
	return strcmp(a->name.c_str(), b->name.c_str()) < 0;
}
 



CombinedObject::CombinedObject(Shader shader, string name) :World3DObject(shader, name)
	{
	}

CombinedObject::CombinedObject(Shader shader, string name, World3DObject* obj, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		parts.push_back(obj);
	}

CombinedObject::CombinedObject(Shader shader, string name, vector<World3DObject*> model, glm::vec3 position, glm::vec3 normal, glm::vec3 scale) :World3DObject(shader, name, position, normal, scale)
	{
		parts = model;
		Sort();
	}

	void CombinedObject::AddObject(World3DObject* obj)
	{
		parts.push_back(obj);
		Sort();
	}

	void CombinedObject::Sort()
	{
		sort(parts.begin(), parts.end(), CompW3D);
		sorted = 1;
	}

	void CombinedObject::Draw(Camera camera, ProgramParams& params)
	{
		glm::mat4 modelMXComb = GetModelMatrix();

		for (int i = 0; i < parts.size(); i++)
		{
			if (parts[i] != NULL)
				parts[i]->Draw(camera, params, modelMXComb);
		}
	}

	void CombinedObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel)
	{
		glm::mat4 modelMXComb = GetModelMatrix();

		for (int i = 0; i < parts.size(); i++)
		{
			if (parts[i] != NULL)
				parts[i]->Draw(camera, params, objModel * modelMXComb);
		}
	}

	void CombinedObject::Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
	{
		glm::mat4 modelMXComb = GetModelMatrix();

		for (int i = 0; i < parts.size(); i++)
		{
			if (parts[i] != NULL)
				parts[i]->Draw(camera, params, modelMXComb, dLight, pLights, sLights);
		}
	}

	void CombinedObject::Draw(Camera camera, ProgramParams& params, glm::mat4 objModel, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights)
	{
		glm::mat4 modelMXComb = GetModelMatrix();

		for (int i = 0; i < parts.size(); i++)
		{
			if (parts[i] != NULL)
				parts[i]->Draw(camera, params, objModel * modelMXComb, dLight, pLights, sLights);
		}
	}

	void CombinedObject::SetInitialMovement(glm::vec3 movement)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			parts[i]->SetInitialMovement(movement);
		}
	}
	void CombinedObject::SetInitialDirection(glm::vec3 direction)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			parts[i]->SetInitialDirection(direction);
		}
	}
	void CombinedObject::SetInitialScale(glm::vec3 scale)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			parts[i]->SetInitialScale(scale);
		}
	}


	void CombinedObject::PartRotateToDegrees(vector<string> names, float p, float y, float r)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->RotateToDegrees(p, y, r);
			}
		}
	}
	void CombinedObject::PartRotateDegrees(vector<string> names, float p, float y, float r)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->RotateDegrees(p, y, r);
			}
		}
	}
	void CombinedObject::PartLookToLocked(vector<string> names, glm::vec3 direction, int plateType, int state, int stateType)
	{
		glm::mat3 A = glm::inverse(GetModelMatrix());
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->LookToLocked(A * direction, plateType, state, stateType);
			}
		}
	}
	void CombinedObject::PartLookPointLocked(vector<string> names, glm::vec3 point, int plateType, int state, int stateType)
	{
		glm::mat3 A = glm::inverse(GetModelMatrix());
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->LookPointLocked(A * (point - position), plateType, state, stateType);
			}
		}
	}
	void CombinedObject::PartLookDirection(vector<string> names, glm::vec3 direction)
	{
		glm::mat3 A = glm::inverse(GetModelMatrix());
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->LookDirection(A * direction);
			}
		}
	}
	void CombinedObject::PartLookPoint(vector<string> names, glm::vec3 point)
	{
		glm::mat3 A = glm::inverse(GetModelMatrix());
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->LookPoint(A * (point - position));
			}
		}
	}
	void CombinedObject::PartSetUpVector(vector<string> names, glm::vec3 vector)
	{
		glm::mat3 A = glm::inverse(GetModelMatrix());
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->SetUpVector(A * vector);
			}
		}
	}
	void CombinedObject::PartRotationAxes(vector<string> names, float angle, glm::vec3 vector)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->RotationAxes(angle, vector);
			}
		}
	}
	void CombinedObject::PartRotationAxes(vector<string> names, float angle)
	{
		for (int i = 0; i < parts.size(); i++)
		{
			if (MeshNameBiSearchVector(names, parts[i]->name))
			{
				parts[i]->RotationAxes(angle);
			}
		}
	}