#ifndef WORLD3DOBJECT_H
#define WORLD3DOBJECT_H

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

//Status codes used for additional settings of some functions
enum Status{
	STATE_TO_FACE = 0,
	STATE_TO_LINE,
	STATE_MAX,
	STATE_MIN,
	STATE_AROUND_UP,
	STATE_AROUND_FRONT,
	STATE_AROUND_RIGHT
};

class World3DObject
{
public:
	Shader shader; //standart shader program
	glm::vec3 position = glm::vec3(0.0f); //positon
	glm::vec3 scale = glm::vec3(1.0f); //scale

	glm::quat qrotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); //quaterion used for define object's rotation

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); //vector indicated vertical direction of object
	glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f); //vector indicated front direction of object
	glm::vec3 right = glm::vec3(-1.0f, 0.0f, 0.0f); //vector indicated right side of object

	float pitch = 0.0f;//pitch
	float yaw = 0.0f;//yaw
	float roll = 0.0f;//roll

	string name; //name of the objects

		
	World3DObject(Shader shader, string name);//base constructor
	World3DObject(Shader shader, string name, glm::vec3 position, glm::vec3 normal, glm::vec3 scale);//constructor with position, normal vector and scale
	
	glm::mat4 GetProjectionMatrix(Camera camera, ProgramParams params);//calculated a projection matrix
	glm::mat4 GetModelMatrix();//calculated the model matrix of the object
	
	//Add lights to the objects
	void SetLights(DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights);	
	void Restruct();//using the rotation quaterion gets the current values of the vectors and angles	
	bool IsLCS(glm::vec3 first, glm::vec3 second, glm::vec3 third);//determines which coordinate system the 3 vectors belong to
	glm::vec3 IntersectSquareLinePoint(glm::vec3 sqN, glm::vec3 p);//calculated the projection of the vector(p) onto the plate defines by the normal(sqN)
	
	virtual void Draw(Camera camera, ProgramParams& params);//prepares and translates object's data to the shader without lights
	virtual void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel);
	virtual void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights);//prepares and translates object's data to the shader with 3 type of lights(Directed light, point light and spotlights)
	virtual void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights);
	virtual void SetInitialMovement(glm::vec3 position);//set initial rotate quaterion without changing origin state
	virtual void SetInitialDirection(glm::vec3 front);
	virtual void SetInitialScale(glm::vec3 scales);//set initial rotate quaterion without changing origin state


	//rotate object
	void LookDirection(glm::vec3 direction);
	void LookPoint(glm::vec3 point);

	void Move(double x, double y, double z);
	void Move(glm::vec3 pos);
	void MoveTo(double x, double y, double z);
	void MoveTo(glm::vec3 pos);

	void RotateToDegrees(float p, float y, float r);
	void RotateDegrees(float p, float y, float r);

	void LookToLocked(glm::vec3 setFront, int plateType = STATE_AROUND_UP, int state = STATE_TO_FACE, int stateType = STATE_MIN);//Rotate object to front vector
	void LookToLocked(float x, float y, float z, int plateType = STATE_AROUND_UP, int state = STATE_TO_FACE, int stateType = STATE_MIN);
	void LookPointLocked(glm::vec3 point, int plateType = STATE_AROUND_UP, int state = STATE_TO_FACE, int stateType = STATE_MIN);

	void RotationAxes(float angle, glm::vec3 vector);
	void RotationAxes(float angle, float Ox, float Oy, float Oz);
	void RotationAxes(float angle);

	void SetUpVector(glm::vec3 vector);
	void SetUpVector(float Ox, float Oy, float Oz);

	void Scale(double x, double y, double z);
	void ScaleTo(double x, double y, double z);

protected:
	glm::quat _setUpVector(glm::vec3 vector);
	glm::quat _rotationAxes(float angle, glm::vec3 vector);
	glm::quat _lookDirection(glm::vec3 front, glm::vec3 right, glm::vec3 direction);
};



class SingleObject :public World3DObject
{
public:
	Model* model;

	string name;

	SingleObject(Shader shader, string name);
	SingleObject(Shader shader, string name, string path);
	SingleObject(Shader shader, string name, Model* model);
	SingleObject(Shader shader, string name, string path, glm::vec3 position, glm::vec3 normal, glm::vec3 scale);
	SingleObject(Shader shader, string name, Model* model, glm::vec3 position, glm::vec3 normal, glm::vec3 scale);

	void Draw(Camera camera, ProgramParams& params);
	void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel);
	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights);
	void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights);
	void SetInitialMovement(glm::vec3 position);
	void SetInitialDirection(glm::vec3 front);
	void SetInitialScale(glm::vec3 scale);
};


class CombinedObject :public World3DObject
{
protected:
	vector<World3DObject*> parts;
	bool sorted = 0;

public:

	CombinedObject(Shader shader, string name);
	CombinedObject(Shader shader, string name, World3DObject* obj, glm::vec3 position, glm::vec3 normal, glm::vec3 scale);
	CombinedObject(Shader shader, string name, vector<World3DObject*> model, glm::vec3 position, glm::vec3 normal, glm::vec3 scale);

	void AddObject(World3DObject* obj);
	void Sort();

	void Draw(Camera camera, ProgramParams& params);
	void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel);
	void Draw(Camera camera, ProgramParams& params, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights);
	void Draw(Camera camera, ProgramParams& params, glm::mat4 objModel, DirectedLight& dLight, vector<PointLight*>& pLights, vector<SpotLight*>& sLights);
	void SetInitialMovement(glm::vec3 position);
	void SetInitialDirection(glm::vec3 front);
	void SetInitialScale(glm::vec3 scale);

	void PartRotateToDegrees(vector<string> names, float p, float y, float r);
	void PartRotateDegrees(vector<string> names, float p, float y, float r);
	void PartLookToLocked(vector<string> names, glm::vec3 direction, int plateType = STATE_AROUND_UP, int state = STATE_TO_FACE, int stateType = STATE_MIN);
	void PartLookPointLocked(vector<string> names, glm::vec3 point, int plateType = STATE_AROUND_UP, int state = STATE_TO_FACE, int stateType = STATE_MIN);
	void PartLookDirection(vector<string> names, glm::vec3 direction);
	void PartLookPoint(vector<string> names, glm::vec3 point);
	void PartSetUpVector(vector<string> names, glm::vec3 vector);
	void PartRotationAxes(vector<string> names, float angle, glm::vec3 vector);
	void PartRotationAxes(vector<string> names, float angle);
};
#endif