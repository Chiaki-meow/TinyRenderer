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
Vec3f eye(1, 1, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

struct Shader : public IShader {
    mat<4, 4, float> uniform_M;
    mat<4, 4, float> uniform_MIT;
    mat<4, 4, float> uniform_MShadow;
    mat<2, 3, float> varying_uv;        // v2f params
    mat<3, 3, float> varying_tri;       // triangle coordinates
    mat<3, 3, float> varying_nrm;       // normal per vertex

    Shader(Matrix M, Matrix MIT, Matrix MS) : uniform_M(M), uniform_MIT(MIT), uniform_MShadow(MS) {}

    virtual Vec4f vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        varying_nrm.set_col(nthvert, proj<3>((Projection * ModelView).invert_transpose() *
                                             embed<4>(model->normal(iface, nthvert), 0.f)));
        Vec4f gl_Vertex = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec4f sb_p = uniform_MShadow * embed<4>(varying_tri * bar);
        sb_p = sb_p / sb_p[3];
        int idx = int(sb_p[0]) + int(sb_p[1] * width);
        float shadow = .3 + .7 * (shadowbuffer[idx] < sb_p[2]);
        Vec2f uv = varying_uv * bar;
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
        Vec3f r = (n * (n * l * 2.f) - l).normalize();   // reflected light

        float spec = pow(std::max(r.z, 0.f), model->specular(uv));
        float diff = std::max(0.f, n * l);
        TGAColor c = model->diffuse(uv);
        for (int i = 0; i < 3; i++)
            color[i] = std::min<float>(20 + c[i] * shadow * (1.2 * diff + .6 * spec), 255);

        return false;
    }
};

struct DepthShader : public IShader {
    mat<4, 3, float> varying_tri;
    mat<3, 3, float> ndc_tri;

    DepthShader() : varying_tri() {}

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
        varying_tri.set_col(nthvert, gl_Vertex);
        ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec3f p = ndc_tri * bar;
        color = TGAColor(255, 255, 255) * (p.z / depth);
        return false;
    }
};

int main(int argc, char **argv) {
    if (2 > argc) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    float *zbuffer = new float[width * height];
    shadowbuffer = new float[width * height];

    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    light_dir = proj<3>((Projection * ModelView * embed<4>(light_dir, 0.f))).normalize();

    Matrix M = Viewport * Projection * ModelView;

    {   // rendering frame
        TGAImage frame(width, height, TGAImage::RGB);
        TGAImage depth(width, height, TGAImage::RGB);

        for (int m = 1; m < argc; m++) {
            model = new Model(argv[m]);
            Shader shader(ModelView, (Projection * ModelView).invert_transpose(),
                          M * (Viewport * Projection * ModelView).invert());
            DepthShader depthShader;
            Vec4f screen_coord[3];
            for (int i = 0; i < model->nfaces(); i++) {
                for (int j = 0; j < 3; j++) {
                    screen_coord[j] = shader.vertex(i, j);
                }
//                triangle(screen_coord, depthShader, depth, shadowbuffer);
                triangle(screen_coord, shader, frame, zbuffer);
//            triangle(shader.varying_tri, shader, frame, zbuffer);
            }
            delete model;
        }
        frame.flip_vertically(); // to place the origin in the bottom left corner of the image
        frame.write_tga_file("framebuffer.tga");
        depth.flip_vertically(); // to place the origin in the bottom left corner of the image
        depth.write_tga_file("depth.tga");
    }
    delete[]zbuffer;
    return 0;
}
