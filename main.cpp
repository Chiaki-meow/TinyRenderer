#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "my_gl.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const int width = 800;
const int height = 800;
Model *model = nullptr;

Vec3f light_dir(1, 1, 1);
Vec3f eye(0, -1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

struct GouraudShader : public IShader {
    Vec3f varying_intensity;

    virtual Vec4f vertex(int iface, int nthvert) {
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir);
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        float intensity = varying_intensity * bar;
        color = TGAColor(255, 255, 255) * intensity;
        return false;
    }
};

struct Shader : public IShader {
    Vec3f varying_intensity;        // v2f params
    mat<2, 3, float> varying_uv;    // v2f params

    virtual Vec4f vertex(int iface, int nthvert) {
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir);
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        return Viewport * Projection * ModelView * gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        float intensity = varying_intensity * bar;
        Vec2f uv = varying_uv * bar;
        color = model->diffuse(uv) * intensity;
        return false;
    }
};

int main(int argc, char **argv) {
    if (2 == argc)
        model = new Model(argv[1]);
    else
        model = new Model("obj/african_head.obj");


    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    light_dir.normalize();

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    GouraudShader shader;
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, shader, image, zbuffer);

    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    zbuffer.flip_vertically();
    image.write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");

    delete model;
    return 0;
}
