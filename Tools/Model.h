#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"
#include "texture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Model
{
public:
    // ������ ������ 
    vector<Texture> textures_loaded; // (�����������) ��������� ��� ����������� ��������, ����� ���������, ��� ��� �� ��������� ����� ������ ����
    vector<Mesh*> meshes;
    std::string directory;
    bool gammaCorrection;
    std::string name;
    glm::vec3 faceColor= glm::vec3(0.6f);
    float shininess = 32.0f;

    Model();

    // ����������� � �������� ��������� ���������� ���� � 3D-������
    Model(std::string const& path, bool gamma = false);

    // ������������ ������, � ������ � ��� � ����
    void Draw(Shader shader);

    void MoveMeshes(glm::vec3 movement);

    ////Methods for get submodel from one(realise at the end)
    vector<std::string> GetMeshNames();
    vector<Texture> GetModelTextures(vector<Mesh*> meshesSet);
    Model GetSubModel(vector<std::string> names, std::string modelName);

private:
    // ��������� ������ � ������� Assimp � ��������� ���������� ���� � ������� meshes
    void loadModel(std::string const& path);

    // ����������� ��������� ����. ������������ ������ ��������� ���, ������������� � ����, � ��������� ���� ������� ��� ����� �������� ����� (���� ������ ������ �������)
    void processNode(aiNode* node, const aiScene* scene);

    Mesh* processMesh(aiMesh* mesh, const aiScene* scene);

    // ��������� ��� �������� ���������� ��������� ���� � �������� ��������, ���� ��� ��� �� ���� ���������.
    // ����������� ���������� ������������ � ���� ��������� Texture
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

bool MeshNameBiSearchVector(vector<string>& names, string meshName, bool needSort = false);
void SortMeshes(vector<string>& names);
bool CompareStrings(string a, string b);


#endif