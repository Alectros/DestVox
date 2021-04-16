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

    Texture();

    Texture(unsigned int id, string type, string path);
};

bool operator == (Texture t1, Texture t2);

vector<Texture> TextureIntersection(vector<Texture> texts1, vector<Texture> texts2);

vector<Texture> TextureUnion(vector<Texture> texts1, vector<Texture> texts2);

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);


#endif