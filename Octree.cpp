#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Octree.h"

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

//Exponentiates a numeric object using binary multiplication
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
OctreeBase<T>::~OctreeBase()
{};

//Calculates the distance using the Pythagorean theorem for three-dimensional space
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
OctreeBase<T>* OctreeBase<T>::FindNearestLeaf(glm::vec3 position)
{
	return NULL;
}
template <class T>
OctreeBase<T>* OctreeBase<T>::FindActualNode(glm::vec3 position)
{
	return NULL;
}
template <class T>
OctreeBase<T>* OctreeBase<T>::FindActualLeaf(glm::vec3 position)
{
	return NULL;
}
//template <class T>
//int OctreeBase<T>::IsNULL()
//{
//	return 0;
//}
//template<class T>
//bool OctreeBase<T>::DeleteNULL()
//{
//	return 0;
//}
//template <class T>
//void OctreeBase<T>::GetShell(OctreeBase<T>* head, vector<T*>& shell) 
//{}




template <class T>
OctreeLeaf<T>::OctreeLeaf()
{
	this->obj = new T();
}

template <class T>
OctreeLeaf<T>::OctreeLeaf(T* obj)
{
	this->obj = obj;
}

template <class T>
OctreeLeaf<T>::~OctreeLeaf()
{
	delete obj;
}

template <class T> 
OctreeBase<T>* OctreeLeaf<T>::FindNearestNode(glm::vec3 position)
{
	return NULL;
}

template <class T>
OctreeBase<T>* OctreeLeaf<T>::FindNearestLeaf(glm::vec3 position)
{
	return this;
}

template <class T>
OctreeBase<T>* OctreeLeaf<T>::FindActualNode(glm::vec3 position)
{
	return NULL;
}

template <class T>
OctreeBase<T>* OctreeLeaf<T>::FindActualLeaf(glm::vec3 position)
{
	if (this->position == position)
		return this;
	else
		return NULL;
}

//template<class T>
//int OctreeLeaf<T>::IsNULL()
//{
//	return obj != NULL;
//}
//
//template<class T>
//bool OctreeLeaf<T>::DeleteNULL()
//{
//	if (obj == NULL)
//		return 1;
//	else
//		return 0;
//}


//template <class T>
//void OctreeLeaf<T>::GetShell(OctreeBase<T>* head, vector<T*>& shell)
//{
//	T* leftO = head->FindActualLeaf(this->position + glm::vec3(-1.0f, 0.0f, 0.0f));
//	T* rightO = head->FindActualLeaf(this->position + glm::vec3(1.0f, 0.0f, 0.0f));
//	T* topO = head->FindActualLeaf(this->position + glm::vec3(0.0f, 1.0f, 0.0f));
//	T* bottomO = head->FindActualLeaf(this->position + glm::vec3(0.0f, -1.0f, 0.0f));
//	T* nearO = head->FindActualLeaf(this->position + glm::vec3(0.0f, 0.0f, -1.0f));
//	T* farO = head->FindActualLeaf(this->position + glm::vec3(0.0f, 0.0f, 1.0f));
//
//	if (leftO == NULL || rightO == NULL || topO == NULL || bottomO == NULL || nearO == NULL || farO == NULL)
//		shell.push_back(obj);
//}










template <class T>
OctreeNode<T>::OctreeNode()
{
	obj = NULL;
}


//Calculates the level of an octree and builds it recursively
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
OctreeNode<T>::~OctreeNode()
{
	delete[] obj;
}

//Searches for the nearest node to the position point
//	Pass existing objects and selects the closest point from all nodes
template <class T>
OctreeBase<T>* OctreeNode<T>::FindNearestNode(glm::vec3 position)
{
	if (obj == NULL)
		return NULL;

	double distNear = this->Distance(position);
	OctreeBase<T>* nodeNear = this;
	for (int i = 0; i < 8; i++)
	{
		if (obj[i] == NULL)
			continue;
		OctreeBase<T>* node = obj[i]->FindNearestNode(position);
		if (node == NULL)
			continue;
		double dist = node->Distance(position);
		if (dist < distNear)
		{
			nodeNear = node;
			distNear = dist;
		}
	}

	return nodeNear;
}

//Searches for the nearest leaf to the position point
//	Pass existing objects and selects the closest point from all leafs
template <class T>
OctreeBase<T>* OctreeNode<T>::FindNearestLeaf(glm::vec3 position)
{
	if (obj == NULL)
		return NULL;

	double distNear = DBL_MAX;
	OctreeBase<T>* leafNear = NULL;
	for (int i = 0; i < 8; i++)
	{
		if (obj[i] == NULL)
			continue;
		OctreeBase<T>* leaf = obj[i]->FindNearestLeaf(position);
		if (leaf == NULL)
			continue;
		double dist = obj[i]->Distance(position);
		if (dist < distNear)
		{
			leafNear = leaf;
			distNear = dist;
		}
	}

	return leafNear;
}

//Searches for the node in the position point
// Searches for a node exactly at the position point, if it does not exist returns NULL
template <class T>
OctreeBase<T>* OctreeNode<T>::FindActualNode(glm::vec3 position)
{
	if (position == this->position)
		return this;

	glm::vec3 vector = position - this->position;

	int xv = vector.x > 0;
	int yv = vector.y > 0;
	int zv = vector.z > 0;

	if (obj == NULL || obj[xv * 4 + yv * 2 + zv] == NULL)
		return NULL;
	else
		return obj[xv * 4 + yv * 2 + zv]->FindActualNode(position);
}

//Searches for the leaf in the position point
// Searches for a leaf exactly at the position point, if it does not exist returns NULL
template <class T>
OctreeBase<T>* OctreeNode<T>::FindActualLeaf(glm::vec3 position)
{
	glm::vec3 vector = position - this->position;

	int xv = vector.x > 0;
	int yv = vector.y > 0;
	int zv = vector.z > 0;

	if (obj == NULL || obj[xv * 4 + yv * 2 + zv] == NULL)
		return NULL;
	else
		return obj[xv * 4 + yv * 2 + zv]->FindActualLeaf(position);
}


//Use up-defined methods to find need leaf and return pointer on object
template <class T>
T* OctreeNode<T>::Find(glm::vec3 position, bool nearestSearch)
{
	OctreeBase<T>* base = NULL;

	if (nearestSearch)
	{
		base = FindNearestLeaf(position);
	}
	else
	{
		base = FindActualLeaf(position);
	}

	if (base == NULL)
		return NULL;
	else
	{
		OctreeLeaf<T>* leaf = (OctreeLeaf<T>*)base;
		return leaf->obj;
	}
}

//Use up-defined methods to find need leaf and replaces pointer on object
template <class T>
void OctreeNode<T>::SetLeaf(glm::vec3 position, T *leaf, bool nearest)
{
	OctreeBase<T>* found = NULL;
	if (!nearest)
	{
		found = FindActualLeaf(position);
	}
	else
	{
		found = FindNearestLeaf(position);
	}
	if (found != NULL)
	{
		OctreeLeaf<T>* objLeaf = (OctreeLeaf<T>*)(found);
		objLeaf->obj = new T(*leaf);
	}
	else
		cout << "OctreeNode.setLeaf : position:" + std::to_string(position.x) + '/' + std::to_string(position.y) + '/' + std::to_string(position.z) + " - NULL pointer you enter incorrect position" << endl;
}

//template<class T>
//int OctreeNode<T>::IsNULL()
//{
//	bool del = false;
//	for (int i = 0; i < 8 && !del; i++)
//	{
//		if (obj[i] != NULL)
//			del = true;
//	}
//	return del * 2;
//}
//
//template<class T>
//bool OctreeNode<T>::DeleteNULL()
//{
//	int type = 0;
//
//	for (int i = 0; i < 8; i++)
//	{
//		if (obj[i] != NULL)
//		{
//			type = obj[i]->IsNULL();
//
//			if (type == 0)
//			{
//				OctreeBase<T>* p = obj[i];
//				obj[i] = NULL;
//				delete p;
//			}
//			else
//				if (type == 1)
//				{
//					return 0;
//				}
//				else
//				{
//					if (obj[i]->DeleteNULL())
//					{
//						OctreeBase<T>* p = obj[i];
//						obj[i] = NULL;
//						delete p;
//					}
//					else
//						return 0;
//				}
//
//		}
//	}
//	return 1;
//}

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

//Recursively create Nodes
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
					glm::vec3 locateDiff(1.0f);
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

//Create a the 1st lvl node with leafs 
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

				OctreeLeaf<T>* leaf = new OctreeLeaf<T>();
				leaf->position = position + locateDiff;
				leafs[4 * i + 2 * j + k] = leaf;
			}
	return leafs;
}