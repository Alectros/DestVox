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
    // ƒанные модели 
    vector<Texture> textures_loaded; // (оптимизаци€) сохран€ем все загруженные текстуры, чтобы убедитьс€, что они не загружены более одного раза
    vector<Mesh*> meshes;
    std::string directory;
    bool gammaCorrection;
    std::string name;
    glm::vec3 faceColor= glm::vec3(0.6f);
    float shininess = 32.0f;

    Model();

    //  онструктор в качестве аргумента использует путь к 3D-модели
    Model(std::string const& path, bool gamma = false);

    // ќтрисовываем модель, а значит и все еЄ меши
    void Draw(Shader shader);

    void MoveMeshes(glm::vec3 movement);

    ////Methods for get submodel from one(realise at the end)
    vector<std::string> GetMeshNames();
    vector<Texture> GetModelTextures(vector<Mesh*> meshesSet);
    Model GetSubModel(vector<std::string> names, std::string modelName);

private:
    // «агружаем модель с помощью Assimp и сохран€ем полученные меши в векторе meshes
    void loadModel(std::string const& path);

    // –екурсивна€ обработка узла. ќбрабатываем каждый отдельный меш, расположенный в узле, и повтор€ем этот процесс дл€ своих дочерних углов (если таковы вообще имеютс€)
    void processNode(aiNode* node, const aiScene* scene);

    Mesh* processMesh(aiMesh* mesh, const aiScene* scene);

    // ѕровер€ем все текстуры материалов заданного типа и загружам текстуры, если они еще не были загружены.
    // Ќеобходима€ информаци€ возвращаетс€ в виде структуры Texture
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

bool MeshNameBiSearchVector(vector<string>& names, string meshName, bool needSort = false);
void SortMeshes(vector<string>& names);
bool CompareStrings(string a, string b);


#endif