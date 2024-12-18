//
// Created by Chiaki_meow on 2024/12/12.
//

#ifndef TINYRENDERER_MY_GL_H
#define TINYRENDERER_MY_GL_H

#include "tgaimage.h"
#include "geometry.h"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;
const float depth = 2000.f;

void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f);
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct IShader {
    virtual ~IShader();
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

void triangle(Vec4f *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer);
void triangle(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer);
void triangle(mat<4, 3, float> &clipc, IShader &shader, TGAImage &image, float *zbuffer);

#endif //TINYRENDERER_MY_GL_H
