#ifndef SEARCHALG_H
#define SEARCHALG_H


#include "Tools/mesh.h"
#include "Tools/shader.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>

using namespace std;
//
//class SearchAlg
//{
//public:
//	bool MeshNameBiSearchVector(vector<string>& names, string meshName, bool needSort = false)
//	{
//		if (needSort)
//			SortMeshes(names);
//
//		return binary_search(names.begin(), names.end(), meshName, CompareStrings);
//	}
//
//	void SortMeshes(vector<string>& names)
//	{
//		sort(names.begin(), names.end(), CompareStrings);
//	}
//
//	bool CompareStrings(string a, string b)
//	{
//		int i = 0;
//		while (a.size() < i && b.size() < i && a[i] == b[i])
//			i++;
//		if (a.size() > i)
//			return 1;
//		if (b.size() > i)
//			return 0;
//		if (a[i] < b[i])
//			return 1;
//		else
//			return 0;
//	}
//
//};

#endif