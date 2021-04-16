
#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Tools/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Tools/mesh.h"
#include "Tools/shader.h"
#include "texture.h"
#include "Tools/Model.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

    Model::Model()
    {

    }

    // ����������� � �������� ��������� ���������� ���� � 3D-������
    Model::Model(string const& path, bool gamma)
    {
        gammaCorrection = gamma;
        loadModel(path);
    }

    // ������������ ������, � ������ � ��� � ����
    void Model::Draw(Shader shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            (*meshes[i]).Draw(shader);
    }

    void Model::MoveMeshes(glm::vec3 movement)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            (*meshes[i]).Move(movement);
    }

    //Methods for get submodel from one(realise at the end)
    vector<string> Model::GetMeshNames()
    {
        vector<string> names;

        for (int i = 0; i < meshes.size(); i++)
            names.push_back((*meshes[i]).name);
        return names;
    }

    vector<Texture> Model::GetModelTextures(vector<Mesh*> meshesSet)
    {
        vector<Texture> allTx;

        for (int i = 0; i < meshesSet.size(); i++)
        {
            allTx = TextureUnion(allTx, (*meshesSet[i]).textures);
        }
        return allTx;
    }

    Model Model::GetSubModel(vector<string> names, string modelName)
    {
        Model rModel;
        vector<Mesh*> outMesh;


        for (int i = 0; i < meshes.size(); i++)
        {
            if (MeshNameBiSearchVector(names, (*meshes[i]).name))
                outMesh.push_back(meshes[i]);
        }
        rModel.meshes = outMesh;

        rModel.textures_loaded = GetModelTextures(outMesh);

        rModel.directory = this->directory;
        rModel.gammaCorrection = this->gammaCorrection;
        rModel.name = modelName;
        rModel.shininess = this->shininess;
        rModel.faceColor = this->faceColor;

        return rModel;
    }

    // ��������� ������ � ������� Assimp � ��������� ���������� ���� � ������� meshes
    void Model::loadModel(string const& path)
    {
        // ������ ����� � ������� Assimp
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        // �������� �� ������
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // ���� �� 0
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        // ��������� ���� � �����
        directory = path.substr(0, path.find_last_of('/'));

        // ����������� ��������� ��������� ���� Assimp
        processNode(scene->mRootNode, scene);
    }

    // ����������� ��������� ����. ������������ ������ ��������� ���, ������������� � ����, � ��������� ���� ������� ��� ����� �������� ����� (���� ������ ������ �������)
    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        // ������������ ������ ��� �������� ����
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // ���� �������� ������ ������� �������� � �����.
            // ����� �� �������� ��� ������; ���� - ��� ���� ������ ����������� ������
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // ����� ����, ��� �� ���������� ��� ���� (���� ������� �������), �� �������� ���������� ������������ ������ �� �������� �����
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // ������ ��� ����������
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // ���� �� ���� �������� ����
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // �� ��������� ������������� ������, �.�. Assimp ���������� ���� ����������� ��������� �����, ������� �� ������������� �������� � ��� glm::vec3, ������� ������� �� �������� ������ � ���� ������������� ������ ���� glm::vec3

            // ����������
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // �������
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            // ���������� ����������
            if (mesh->mTextureCoords[0]) // ���� ��� �������� ���������� ����������
            {
                glm::vec2 vec;

                // ������� ����� ��������� �� 8 ��������� ���������� ���������. �� ������������, ��� �� �� ����� ������������ ������,
                // � ������� ������� ����� ��������� ��������� ���������� ���������, ������� �� ������ ����� ������ ����� (0)
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            // ����������� ������
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            // ������ ���������
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }
        // ������ ���������� �� ������ ����� ���� (����� - ��� ����������� ����) � ��������� ��������������� ������� ������
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            // �������� ��� ������� ������ � ��������� �� � ������� indices
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // ������������ ���������
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // �� ������ ���������� �� ������ ��������� � ��������. ������ ��������� �������� ����� ���������� 'texture_diffuseN',
        // ��� N - ���������� ����� �� 1 �� MAX_SAMPLER_NUMBER. 
        // ���� ����� ��������� � � ������ ���������:
        // ��������� - texture_diffuseN
        // ��������� - texture_specularN
        // ������� - texture_normalN

        // 1. ��������� �����
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. ����� ���������
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // 3. ����� ��������
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 4. ����� �����
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // ���������� ���-������, ��������� �� ������ ���������� ������
        return new Mesh(vertices, indices, textures, mesh->mName.C_Str());
    }

    // ��������� ��� �������� ���������� ��������� ���� � �������� ��������, ���� ��� ��� �� ���� ���������.
    // ����������� ���������� ������������ � ���� ��������� Texture
    vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // ���������, �� ���� �� �������� ��������� �����, � ���� - ��, �� ���������� �������� ����� �������� � ��������� � ��������� ��������
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // �������� � ��� �� ����� � ����� ��� ���������, ��������� � ��������� (�����������)
                    break;
                }
            }
            if (!skip)
            {   // ���� �������� ��� �� ���� ���������, �� ��������� �
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory, false);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // ��������� �������� � ������� � ��� ������������ ����������, ��� ����� ����������, ��� � ��� �� �������� ��� ������������� ��������� �������
            }
        }
        return textures;
    }


    bool MeshNameBiSearchVector(vector<string>& names, string meshName, bool needSort)
    {
        if (needSort)
            SortMeshes(names);

        return binary_search(names.begin(), names.end(), meshName, CompareStrings);
    }

    void SortMeshes(vector<string>& names)
    {
        sort(names.begin(), names.end(), CompareStrings);
    }

    bool CompareStrings(string a, string b)
    {
        int i = 0;
        while (a.size() > i && b.size() > i && a[i] == b[i])
            i++;
        if (a.size() > i)
            return 1;
        if (b.size() > i)
            return 0;
        if (a[i] < b[i])
            return 1;
        else
            return 0;
    }
