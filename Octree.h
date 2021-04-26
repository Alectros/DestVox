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


//function exponentiation of a number
template <class T>
int Stepi(T _x, int _a);

template <class T>
class OctreeBase
{
public:
	glm::vec3 position;

	//constructors and destructors

	//Base constructor
	OctreeBase();
	//Destructor
	~OctreeBase();

	//calculated distance between current class object and point position
	float Distance(glm::vec3 position);

	//Find a nearest node to position point
	virtual OctreeBase* FindNearestNode(glm::vec3 position);
	//Find a nearest leaf to position point
	virtual OctreeBase* FindNearestLeaf(glm::vec3 position);
	//If in Node exist in position point return it othewise return NULL
	virtual OctreeBase* FindActualNode(glm::vec3 position);
	//If in Leaf exist in position point return it othewise return NULL
	virtual OctreeBase* FindActualLeaf(glm::vec3 position);

	//virtual int IsNULL();
	//virtual bool DeleteNULL();

	//virtual void GetShell(OctreeBase<T>* head, vector<T*>& shell);
};

template <class T>
class OctreeLeaf : public OctreeBase<T>
{
public:
	T* obj;

	//Base constructor
	OctreeLeaf();
	//Constructor assign object to leaf
	OctreeLeaf(T* obj);
	//Destructor
	~OctreeLeaf();

	//Find a nearest node to position point
	OctreeBase<T>* FindNearestNode(glm::vec3 position);
	//Find a nearest leaf to position point
	OctreeBase<T>* FindNearestLeaf(glm::vec3 position);
	//If in Node exist in position point return it othewise return NULL
	OctreeBase<T>* FindActualNode(glm::vec3 position);
	//If in Leaf exist in position point return it othewise return NULL
	OctreeBase<T>* FindActualLeaf(glm::vec3 position);

	//int IsNULL();
	//bool DeleteNULL();

	//void GetShell(OctreeBase<T>* head, vector<T*>& shell);
};

template <class T>
class OctreeNode : OctreeBase<T>
{
public:
	OctreeBase<T> **obj;

	//Base constructor
	OctreeNode();
	//Build the octotree with size level
	OctreeNode(int size);
	//Destructor
	~OctreeNode();

	//Find a nearest node to position point
	OctreeBase<T>* FindNearestNode(glm::vec3 position);
	//Find a nearest leaf to position point
	OctreeBase<T>* FindNearestLeaf(glm::vec3 position);
	//If in Node exist in position point return it othewise return NULL
	OctreeBase<T>* FindActualNode(glm::vec3 position);
	//If in Leaf exist in position point return it othewise return NULL
	OctreeBase<T>* FindActualLeaf(glm::vec3 position);

	//Search a object in leaf in position point, there are two ways to search actual(default) and nearest
	//	actual - return object exactly in position point
	//	nearest - return nearest to position point
	T* Find(glm::vec3 position, bool nearestSearch = true);
	//Set an object to leaf in position point
	//	actual - set object exactly in position point
	//	nearest - set nearest to position point
	void SetLeaf(glm::vec3 position, T *leaf, bool nearest = false);

	//int IsNULL();
	//bool DeleteNULL();
	//void GetShell(OctreeBase<T>* head, vector<T*>& shell);

private:
	//Create octotree node(lvl > 1)
	OctreeBase<T>** NodeCreator(glm::vec3 position, int size);
	//Create octotree node with leafs(lvl = 1)
	OctreeBase<T>** LeafCreator(glm::vec3 position);
};



#endif