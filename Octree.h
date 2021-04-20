#ifndef Octree_H
#define Octree_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include "Octree.h"

using namespace std;

template <class T>
int Stepi(T _x, int _a);

template <class T>
class OctreeBase
{
public:
	glm::vec3 position;

	OctreeBase()
	{

	}

	OctreeBase(int size)
	{
	}

	float Distance(glm::vec3 position)
	{
		float x = this->position.x - position.x;
		float y = this->position.y - position.y;
		float z = this->position.z - position.z;
		return sqrtf(x * x + y * y + z * z);
	}

	virtual OctreeBase* FindNearestNode(glm::vec3 position) { return NULL; }
	virtual T* FindNearestObj(glm::vec3 position) { return NULL; }
	virtual OctreeBase* ExistNodeInPoint(glm::vec3 position) { return NULL; }
	virtual T* ExistObjInPoint(glm::vec3 position) { return NULL; }

	virtual void GetShell(OctreeBase<T>* head, vector<T*>& shell){}
};

template <class T>
class OctreeLeaf : public OctreeBase<T>
{
public:
	T* obj;

	OctreeBase<T>* FindNearestNode(glm::vec3 position)
	{
		return NULL;
	}
	T* FindNearestObj(glm::vec3 position)
	{
		return this->obj;
	}
	OctreeBase<T>* ExistNodeInPoint(glm::vec3 position)
	{
		return NULL;
	}
	T* ExistObjInPoint(glm::vec3 position)
	{
		if (this->position == position)
			return this->obj;
		else
			return NULL;
	}

	void GetShell(OctreeBase<T> *head, vector<T*>& shell)
	{		
		T* leftO = head->ExistObjInPoint(this->position + glm::vec3(-1.0f, 0.0f, 0.0f));
		T* rightO = head->ExistObjInPoint(this->position + glm::vec3(1.0f, 0.0f, 0.0f));
		T* topO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, 1.0f, 0.0f));
		T* bottomO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, -1.0f, 0.0f));
		T* nearO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, 0.0f, -1.0f));
		T* farO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, 0.0f, 1.0f));

		if (leftO == NULL || rightO == NULL || topO == NULL || bottomO == NULL || nearO == NULL || farO == NULL)
			shell.push_back(obj);
	}
};

template <class T>
class OctreeNode : public OctreeBase<T>
{
public:
	OctreeBase<T> obj[2][2][2];

	OctreeNode()
	{

	}

	OctreeNode(int size)
	{
		this->position = glm::vec3(0.0f);
		int mxsize = round(log2(size));
		*this = Creator(this->position, mxsize);
	}

	template <class T>

	OctreeBase<T>* FindNearestNode(glm::vec3 position)
	{
		if (position == this->position)
			return this;

		glm::vec3 vector = position - this->position;

		int xv = vector.x > 0;
		int yv = vector.y > 0;
		int zv = vector.z > 0;

		OctreeNode<T>* nNode = obj[xv][yv][zv].FindNearestNode(position);

		if (nNode == NULL)
			return this;
		else
			return nNode;
	}

	T* FindNearestObj(glm::vec3 position) 
	{
		glm::vec3 vector = position - this->position;

		int xv = vector.x > 0;
		int yv = vector.y > 0;
		int zv = vector.z > 0;

		T *nT = obj[xv][yv][zv].FindNearestObj(position);

		return nT;
	}

	OctreeBase<T>* ExistNodeInPoint(glm::vec3 position)
	{
		if (position == this->position)
			return this;

		glm::vec3 vector = position - this->position;

		int xv = vector.x > 0;
		int yv = vector.y > 0;
		int zv = vector.z > 0;

		return obj[xv][yv][zv].FindNearestNode(position);
	}
	T* ExistObjInPoint(glm::vec3 position) 
	{
		glm::vec3 vector = position - this->position;

		int xv = vector.x > 0;
		int yv = vector.y > 0;
		int zv = vector.z > 0;

		T* nT = obj[xv][yv][zv].FindNearestObj(position);

		return nT;
	}

	void GetShell(OctreeBase<T>* head, vector<T*>& shell)
	{
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
						obj[i][j][k].GetShell(head, shell);
				}
	}

private:

	OctreeNode<T> Creator(glm::vec3 position, int size)
	{
		OctreeNode<T> node;
		node.position = position;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					if (size == 0)
					{
						glm::vec3 locateDiff(1.0f);
						if (!i)
							locateDiff.x *= -1;
						if (!j)
							locateDiff.y *= -1;
						if (!k)
							locateDiff.z *= -1;

						OctreeLeaf<T> leaf;
						node.obj[i][j][k] = leaf;
					}
					else
					{
						int length = Stepi(2, size - 1);

						glm::vec3 locateDiff(length);
						if (!i)
							locateDiff.x *= -1;
						if (!j)
							locateDiff.y *= -1;
						if (!k)
							locateDiff.z *= -1;

						node.obj[i][j][k] = Creator(position + locateDiff, size - 1);
					}
				}
		return node;
	}
};

template <class T>
int Stepi(T _x, int _a)
{
	int ar = 0;
	int xr = 1;

	while (ar < _a)
	{
		int ak = 1;
		T xk = _x;

		while (ar + 2 * ak < _a)
		{
			xk *= xk;
			ak *= 2;
		}

		xr *= xk;
		ar += ak;
	}
	return xr;
}


#endif