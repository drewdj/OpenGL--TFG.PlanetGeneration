#pragma once
#include "common.h"
#include <vector>

class Texture{

public:
    int w=0;
    int h=0;
    int textType;
    unsigned int glId=-1;
    std::vector<GLuint> glIds;

    Texture(std::string filename);
    Texture(int textType, std::string folder);
    Texture(const std::vector<std::string>& filenames);
    Texture();
    Texture(int textType);
    void bind(int textureunitIdx);
    void bindMultiple(int startingTextureUnitIdx);

};