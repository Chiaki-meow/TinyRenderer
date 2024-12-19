#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "my_gl.h"

const int width = 800;
const int height = 800;

Model *model = nullptr;
float *shadowbuffer = nullptr;

Vec3f light_dir(1, 1, 0);

Vec3f eye(1.2, -.8, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

TGAImage total(1024, 1024, TGAImage::GRAYSCALE);
TGAImage occl(1024, 1024, TGAImage::GRAYSCALE);

struct ZShader : public IShader {
    mat<4, 3, float> varying_tri;

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f gl_Vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, gl_Vertex);
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f gl_FragCoord, Vec3f bar, TGAColor &color) {
        color = TGAColor(255, 255, 255) * ((gl_FragCoord.z + 1.f) / 2.f);
        return false;
    }
};

struct Shader : public IShader {
    mat<2, 3, float> varying_uv;        // v2f params
    mat<4, 3, float> varying_tri;       // triangle coordinates

    virtual Vec4f vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, (gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f gl_FragCoord, Vec3f bar, TGAColor &color) {
        Vec2f uv = varying_uv * bar;
        if (std::abs(shadowbuffer[int(gl_FragCoord.x + gl_FragCoord.y * width)] - gl_FragCoord.z < 1e-2)) {
            occl.set(uv.x * 1024, uv.y * 1024, TGAColor(255));
        }
        return false;
    }
};

struct AOShader : public IShader {
    mat<2, 3, float> varying_uv;        // v2f params
    mat<4, 3, float> varying_tri;       // triangle coordinates
    TGAImage aoimage;

    virtual Vec4f vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, (gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f gl_FragCoord, Vec3f bar, TGAColor &color) {
        Vec2f uv = varying_uv * bar;
        int t = aoimage.get(uv.x * 1024, uv.y * 1024)[0];
        color = TGAColor(t, t, t);
        return false;
    }
};

struct DepthShader : public IShader {
    mat<3, 3, float> ndc_tri;

    DepthShader() : ndc_tri() {}

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
        ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec3f p = ndc_tri * bar;
        color = TGAColor(255, 255, 255) * (p.z / depth);
        return false;
    }
};

Vec3f rand_point_on_unit_sphere() {
    float u = (float) rand() / (float) RAND_MAX;
    float v = (float) rand() / (float) RAND_MAX;
    float theta = 2.f * M_PI * u;
    float phi = acos(2.f * v - 1.f);
    return Vec3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}


int main(int argc, char **argv) {
    if (2 > argc) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    float *zbuffer = new float[width * height];
    shadowbuffer = new float[width * height];

    TGAImage frame(width, height, TGAImage::RGB);
    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());

//    AOShader aoshader;
//    aoshader.aoimage.read_tga_file("occlusion.tga");
//    aoshader.aoimage.flip_vertically();
//    for (int i = 0; i < model->nfaces(); i++) {
//        for (int j = 0; j < 3; j++) {
//            aoshader.vertex(i, j);
//        }
//        triangle(aoshader.varying_tri, aoshader, frame, zbuffer);
//    }
//    frame.flip_vertically();
//    frame.write_tga_file("framebuffer.tga");
//    return 0;


    const int nrenders = 1;
    for (int iter = 0; iter <= nrenders; iter++) {
        std::cerr << iter << " from " << nrenders << std::endl;

        for (int i = 0; i < 3; i++) up[i] = (float) rand() / (float) RAND_MAX;
        eye = rand_point_on_unit_sphere();
        eye.y = std::abs(eye.y);

        for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

        lookat(eye, center, up);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        projection(0);

        for (int m = 1; m < argc; m++) {
            model = new Model(argv[m]);
            ZShader zShader;
            for (int i = 0; i < model->nfaces(); i++) {
                for (int j = 0; j < 3; j++) {
                    zShader.vertex(i, j);
                }
                triangle(zShader.varying_tri, zShader, frame, shadowbuffer);
            }

            frame.flip_vertically();
            frame.write_tga_file("framebuffer.tga");

            Shader shader;
            occl.clear();
            for (int i = 0; i < model->nfaces(); i++) {
                for (int j = 0; j < 3; j++) {
                    shader.vertex(i, j);
                }
                triangle(shader.varying_tri, shader, frame, zbuffer);
            }
            for (int i = 0; i < 1024; i++) {
                for (int j = 0; j < 1024; j++) {
                    float tmp = total.get(i, j)[0];
                    total.set(i, j, TGAColor((tmp * (iter - 1) + occl.get(i, j)[0]) / (float) iter + .5f));
                }
            }

            delete model;
        }
        total.flip_vertically(); // to place the origin in the bottom left corner of the image
        total.write_tga_file("occlusion.tga");
        occl.flip_vertically(); // to place the origin in the bottom left corner of the image
        occl.write_tga_file("occl.tga");
    }

    delete[]zbuffer;
    delete[]shadowbuffer;
    return 0;
}
