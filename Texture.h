#ifndef Texture_H
#define Texture_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Texture {
public:
    unsigned int id;
    string type;
    string path;

    Texture()
    {

    }

    Texture(unsigned int id, string type, string path)
    {
        this->id = id;
        this->type = type;
        this->path = path;
    }
};
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
    vector<Texture> result = TextureIntersection(texts1,texts2);

    for (int i = 0; i < texts1.size(); i++)
    {
        if (find(result.begin(), result.end(), texts1[i]) != result.end())
        {
            result.push_back(texts1[i]);
        }
    }

    for (int i = 0; i < texts2.size(); i++)
    {
        if (find(result.begin(), result.end(), texts2[i]) != result.end())
        {
            result.push_back(texts2[i]);
        }
    }
    return result;
}




#endif