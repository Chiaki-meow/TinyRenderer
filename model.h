//
// Created by 程佳雯 on 2023/5/30.
//

#ifndef TINYRENDERER_CHIAKI_MODEL_H
#define TINYRENDERER_CHIAKI_MODEL_H


#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal
    std::vector<Vec3f> norms_;
    std::vector<Vec2f> uv_;
    TGAImage diffusemap_;
    void load_texture(std::string filename, const char *suffix, TGAImage &img);

public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
    TGAColor diffuse(Vec2i uv);
    Vec2i uv(int iface, int nvert);
    Vec3f norm(int iface, int nvert);
};


#endif //TINYRENDERER_CHIAKI_MODEL_H
