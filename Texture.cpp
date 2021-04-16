

#include <string>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Texture.h"
#include "Tools/stb_image.h"

using namespace std;

Texture::Texture()
{
}

Texture::Texture(unsigned int id, string type, string path)
{
    this->id = id;
    this->type = type;
    this->path = path;
}

bool operator == (Texture t1, Texture t2)
{
    if (t1.id == t2.id && t1.path == t2.path && t1.type == t1.type)
        return 1;
    else
        return 0;
}

vector<Texture> TextureIntersection(vector<Texture> texts1, vector<Texture> texts2)
{
    vector<Texture> intersect;

    for (int i = 0; i < texts1.size(); i++)
        for (int j = 0; j < texts2.size(); i++)
        {
            if (texts1[i] == texts2[j])
            {
                intersect.push_back(texts1[i]);
                continue;
            }
        }
    return intersect;
}

vector<Texture> TextureUnion(vector<Texture> texts1, vector<Texture> texts2)
{
    vector<Texture> result = TextureIntersection(texts1, texts2);

    for (int i = 0; i < texts1.size(); i++)
    {
        if (find(result.begin(), result.end(), texts1[i]) == result.end())
        {
            result.push_back(texts1[i]);
        }
    }

    for (int i = 0; i < texts2.size(); i++)
    {
        if (find(result.begin(), result.end(), texts2[i]) == result.end())
        {
            result.push_back(texts2[i]);
        }
    }
    return result;
}

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
