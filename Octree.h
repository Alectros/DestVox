#ifndef OCTREE_H
#define OCTREE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include "Octree.h"
#include "Voxel.h"

using namespace std;

enum States {
	OCTOSTATE_NEAREST = 0,
	OCTOSTATE_ACTUAL
};

template <class T>
int Stepi(T _x, int _a);

template <class T>
class OctreeBase
{
public:
	glm::vec3 position;

	OctreeBase();
	OctreeBase(int size);

	float Distance(glm::vec3 position);

	virtual OctreeBase* FindNearestNode(glm::vec3 position);
	virtual T* FindNearestObj(glm::vec3 position);
	virtual OctreeBase* ExistNodeInPoint(glm::vec3 position);
	virtual T* ExistObjInPoint(glm::vec3 position);

	//virtual void GetShell(OctreeBase<T>* head, vector<T*>& shell);
};

template <class T>
class OctreeLeaf : public OctreeBase<T>
{
public:
	T* obj;

	OctreeBase<T>* FindNearestNode(glm::vec3 position);
	T* FindNearestObj(glm::vec3 position);
	OctreeBase<T>* ExistNodeInPoint(glm::vec3 position);
	T* ExistObjInPoint(glm::vec3 position);

	//void GetShell(OctreeBase<T>* head, vector<T*>& shell);
};

template <class T>
class OctreeNode : OctreeBase<T>
{
public:
	OctreeBase<T> **obj;

	OctreeNode();
	OctreeNode(int size);

	OctreeBase<T>* FindNearestNode(glm::vec3 position);
	T* FindNearestObj(glm::vec3 position);
	OctreeBase<T>* ExistNodeInPoint(glm::vec3 position);
	T* ExistObjInPoint(glm::vec3 position);

	void SetLeaf(glm::vec3 position, T leaf, int type = OCTOSTATE_ACTUAL);

	//void GetShell(OctreeBase<T>* head, vector<T*>& shell);

private:

	OctreeBase<T>** NodeCreator(glm::vec3 position, int size);

	OctreeBase<T>** LeafCreator(glm::vec3 position);
};



#endif