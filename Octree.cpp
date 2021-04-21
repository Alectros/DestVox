#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Octree.h"

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;



template <class T>
T Stepi(T _x, int _a)
{
	int ar = 0;
	T xr = 1;

	while (ar < _a)
	{
		int ak = 1;
		T xk = _x;

		while (ar + 2 * ak < abs(_a))
		{
			xk *= xk;
			ak *= 2;
		}

		xr *= xk;
		ar += ak;
	}
	if (_a < 0)
		xr = 1 / xr;
	return xr;
}


template <class T>
OctreeBase<T>::OctreeBase()
{}

template <class T>
OctreeBase<T>::OctreeBase(int size)
{}

template <class T>
float OctreeBase<T>::Distance(glm::vec3 position)
{
	float x = this->position.x - position.x;
	float y = this->position.y - position.y;
	float z = this->position.z - position.z;
	return sqrtf(x * x + y * y + z * z);
}

template <class T>
OctreeBase<T>* OctreeBase<T>::FindNearestNode(glm::vec3 position)
{
	return NULL;
}
template <class T>
T* OctreeBase<T>::FindNearestObj(glm::vec3 position)
{
	return NULL;
}
template <class T>
OctreeBase<T>* OctreeBase<T>::ExistNodeInPoint(glm::vec3 position)
{
	return NULL;
}
template <class T>
T* OctreeBase<T>::ExistObjInPoint(glm::vec3 position)
{
	return NULL;
}
//template <class T>
//void OctreeBase<T>::GetShell(OctreeBase<T>* head, vector<T*>& shell) 
//{}







template <class T> 
OctreeBase<T>* OctreeLeaf<T>::FindNearestNode(glm::vec3 position)
{
	return NULL;
}

template <class T>
T* OctreeLeaf<T>::FindNearestObj(glm::vec3 position)
{
	return this->obj;
}

template <class T>
OctreeBase<T>* OctreeLeaf<T>::ExistNodeInPoint(glm::vec3 position)
{
	return NULL;
}

template <class T>
T* OctreeLeaf<T>::ExistObjInPoint(glm::vec3 position)
{
	if (this->position == position)
		return this->obj;
	else
		return NULL;
}

//template <class T>
//void OctreeLeaf<T>::GetShell(OctreeBase<T>* head, vector<T*>& shell)
//{
//	T* leftO = head->ExistObjInPoint(this->position + glm::vec3(-1.0f, 0.0f, 0.0f));
//	T* rightO = head->ExistObjInPoint(this->position + glm::vec3(1.0f, 0.0f, 0.0f));
//	T* topO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, 1.0f, 0.0f));
//	T* bottomO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, -1.0f, 0.0f));
//	T* nearO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, 0.0f, -1.0f));
//	T* farO = head->ExistObjInPoint(this->position + glm::vec3(0.0f, 0.0f, 1.0f));
//
//	if (leftO == NULL || rightO == NULL || topO == NULL || bottomO == NULL || nearO == NULL || farO == NULL)
//		shell.push_back(obj);
//}










template <class T>
OctreeNode<T>::OctreeNode()
{
	obj = NULL;
}

template <class T>
OctreeNode<T>::OctreeNode(int size)
{
	this->position = glm::vec3(0.0f);
	int mxsize = round(log2(size));
	
	if (size == 0|| mxsize == 0)
	{
		delete obj;
		obj = new OctreeBase<T>*[8];

		OctreeLeaf<T> leaf;
		leaf.position = this->position;
		for (int i = 0; i < 8; i++)
			obj[i] = &leaf;
		return;
	}
	
	if (mxsize == 1)
	{
		obj = LeafCreator(this->position);
	}
	else
	{
		obj = (NodeCreator(this->position, mxsize - 1));
	}

}

template <class T>
OctreeBase<T>* OctreeNode<T>::FindNearestNode(glm::vec3 position)
{
	if (position == this->position)
		return this;

	glm::vec3 vector = position - this->position;

	int xv = vector.x > 0;
	int yv = vector.y > 0;
	int zv = vector.z > 0;

	OctreeBase<T> *nNode = obj[xv * 4 + yv * 2 + zv]->FindNearestNode(position);

	if (nNode == NULL)
		return this;
	else
		return nNode;
}

template <class T>
T* OctreeNode<T>::FindNearestObj(glm::vec3 position)
{
	glm::vec3 vector = position - this->position;

	int xv = vector.x > 0;
	int yv = vector.y > 0;
	int zv = vector.z > 0;

	T* nT = obj[xv * 4 + yv * 2 + zv]->FindNearestObj(position);
	return nT;
}

template <class T>
OctreeBase<T>* OctreeNode<T>::ExistNodeInPoint(glm::vec3 position)
{
	if (position == this->position)
		return this;

	glm::vec3 vector = position - this->position;

	int xv = vector.x > 0;
	int yv = vector.y > 0;
	int zv = vector.z > 0;

	return obj[xv * 4 + yv * 2 + zv]->ExistNodeInPoint(position);
}

template <class T>
T* OctreeNode<T>::ExistObjInPoint(glm::vec3 position)
{
	glm::vec3 vector = position - this->position;

	int xv = vector.x > 0;
	int yv = vector.y > 0;
	int zv = vector.z > 0;

	T* nT = obj[xv * 4 + yv * 2 + zv]->ExistObjInPoint(position);

	return nT;
}

template <class T>
void OctreeNode<T>::SetLeaf(glm::vec3 position, T leaf, int type)
{
	T* found = NULL;
	if (type == OCTOSTATE_ACTUAL)
	{
		found = ExistObjInPoint(position);
	}
	else
		if (type == OCTOSTATE_NEAREST)
	{
		found = FindNearestObj(position);
	}
	if (found != NULL)
		*found = leaf;
	else
		found = new T(leaf);
}

//template <class T>
//void OctreeNode<T>::GetShell(OctreeBase<T>* head, vector<T*>& shell)
//{
//	for (int i = 0; i < 2; i++)
//		for (int j = 0; j < 2; j++)
//			for (int k = 0; k < 2; k++)
//			{
//				obj[i * 4 + j * 2 + k]->GetShell(head, shell);
//			}
//}

template <class T>
OctreeBase<T>**  OctreeNode<T>::NodeCreator(glm::vec3 position, int size)
{
	OctreeBase<T>** node = new OctreeBase<T>*[8];

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				if (size <= 1)
				{
					glm::vec3 locateDiff(0.5f);
					if (!i)
						locateDiff.x *= -1;
					if (!j)
						locateDiff.y *= -1;
					if (!k)
						locateDiff.z *= -1;

					OctreeNode<T>* tekNode = new OctreeNode<T>();

					tekNode->position = this->position + locateDiff;
					tekNode->obj = LeafCreator(position + locateDiff);
					node[4 * i + 2 * j + k] = tekNode;
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

					OctreeNode<T>* tekNode = new OctreeNode<T>();

					tekNode->position = this->position + locateDiff;
					tekNode->obj = NodeCreator(position + locateDiff, size - 1);
					node[4 * i + 2 * j + k] = tekNode;
				}
			}
	return node;
}

template <class T>
OctreeBase<T>** OctreeNode<T>::LeafCreator(glm::vec3 position)
{
	OctreeBase<T>** leafs = new OctreeBase<T>*[8];
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				glm::vec3 locateDiff(0.5f);
				if (!i)
					locateDiff.x *= -1;
				if (!j)
					locateDiff.y *= -1;
				if (!k)
					locateDiff.z *= -1;

				OctreeLeaf<T> leaf;
				leaf.position = position + locateDiff;
				leafs[4 * i + 2 * j + k] = &leaf;
			}
	return leafs;
}