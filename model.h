//
// Created by 程佳雯 on 2023/5/30.
//

#ifndef TINYRENDERER_CHIAKI_MODEL_H
#define TINYRENDERER_CHIAKI_MODEL_H


#include <vector>
#include "geometry.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int> > faces_;

public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
};


#endif //TINYRENDERER_CHIAKI_MODEL_H
