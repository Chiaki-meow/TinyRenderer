#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
const TGAColor blue   = TGAColor(0, 0,   255,   255);
Model *model = NULL;
const int width = 800;
const int height = 800;
//
//void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color){
//    /*
//    for(float t=0.;t<1.;t+=.01){
//        int x = x0+(x1-x0)*t;
//        int y = y0+(y1-y0)*t;
//        image.set(x,y,color);
//    }
//    */
//    int x0 = t0.x;
//    int y0 = t0.y;
//    int x1 = t1.x;
//    int y1 = t1.y;
//
//    bool steep = false;
//    if(std::abs((x0-x1))<std::abs(y0-y1)){
//        // 检查斜率 如果陡峭的话就交换一下xy
//        std::swap(x0, y0);
//        std::swap(x1, y1);
//        steep = true;
//    }
//    if(x0>x1){
//        std::swap(x0,x1);
//        std::swap(y0,y1);
//    }
//
//    int dx=x1-x0;
//    int dy=y1-y0;
//
//    float derror2 = std::abs(dy)*2;
//    float error2 = 0;
//    int y=y0;
//
//    for(int x=x0; x<=x1; x++){
//        if(steep){
//            image.set(y, x, color); //由于交换了xy，需要在返回结果时交换回去
//        }else {
//            image.set(x, y, color);
//        }
//        error2 += derror2;
//        if(error2>dx){
//            y+=(y1>y0?1:-1);
//            error2-=dx*2;
//        }
//    }
//}
//
////Vec3f barycentric(Vec2i* pts, Vec2i P) {
////    Vec3f u = Vec3f(pts[2].x-pts[0].x, pts[1].x-pts[0].x, pts[0].x-P.x)^Vec3f(pts[2].y-pts[0].y, pts[1].y-pts[0].y, pts[0].y-P.y);
////    /* `pts` and `P` has integer value as coordinates
////       so `abs(u[2])` < 1 means `u[2]` is 0, that means
////       triangle is degenerate, in this case return something with negative coordinates */
////    if (std::abs(u.z)<1) return Vec3f(-1,1,1);
////    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
////}
//
////Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P){
////    Vec3f s[2];
////    for(int i=2;i--;){
////        s[i][0] = C[i]-A[i];
////        s[i][1] = B[i]-A[i];
////        s[i][2] = A[i]-P[i];
////    }
////    Vec3f u = cross(s[0],s[1]);
////    if(std::abs(u[2])>1e-2)
////        return Vec3f (1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
////    return Vec3f (-1,1,1);
////}
//
////void triangle(Vec2i *pts, TGAImage &image, TGAColor color) {
////    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1);
////    Vec2i bboxmax(0, 0);
////    Vec2i clamp(image.get_width()-1, image.get_height()-1);
////    for (int i=0; i<3; i++) {
////        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
////        bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));
////
////        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
////        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
////    }
////    Vec2i P;
////    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
////        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
////            Vec3f bc_screen  = barycentric(pts, P);
////            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
////            image.set(P.x, P.y, color);
////        }
////    }
////}
////
////void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
////    // 对顶点排序，按照y的大小从高到低排序 => y0<y1<y2
////    if(t0.y>t1.y)   std::swap(t0,t1);
////    if(t0.y>t2.y)   std::swap(t0,t2);
////    if(t1.y>t2.y)   std::swap(t1,t2);
////    int total_height = t2.y - t0.y;
////
////    for(int y=t0.y; y<t1.y; y++){
////        int segment_height = t1.y-t0.y+1; // 谨防除0
////        float alpha = (float)(y-t0.y)/total_height;
////        float beta = (float)(y-t0.y)/segment_height;
////        Vec2i A = t0 + (t2-t0)*alpha;
////        Vec2i B = t0 + (t1-t0)*beta;
////        if(A.x>B.x) std::swap(A, B);
////        for(int j=A.x;j<=B.x;j++){
////            image.set(j,y,color);
////        }
////    }
////
////    for(int y=t1.y; y<t2.y; y++){
////        int segment_height = t2.y-t1.y+1; // 谨防除0
////        float alpha = (float)(y-t0.y)/total_height;
////        float beta = (float)(y-t1.y)/segment_height;
////        Vec2i A = t0 + (t2-t0)*alpha;
////        Vec2i B = t1 + (t2-t1)*beta;
////        if(A.x>B.x) std::swap(A, B);
////        for(int j=A.x;j<=B.x;j++){
////            image.set(j,y,color);
////        }
////    }
////
//////    line(t0, t1, image, green);
//////    line(t1, t2, image, green);
//////    line(t2, t0, image, red);
////}
//
//void rasterize(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color, int ybuffer[]){
//    if(p0.x>p1.x){
//        std::swap(p0, p1);
//    }
//    for(int x=p0.x; x<p1.x; x++){
//        float t = (x - p0.x)/(float)(p1.x-p0.x);
//        int y = p0.y*(1.-t)+p1.y*t;
//        if(ybuffer[x]<y){
//            ybuffer[x] = y;
//            image.set(x, 0, color);
//        }
//    }
//}
//
//
////void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) {
////    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
////    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
////    Vec2f clamp(image.get_width()-1, image.get_height()-1);
////    for (int i=0; i<3; i++) {
////        for (int j=0; j<2; j++) {
////            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
////            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
////        }
////    }
////    Vec3f P;
////    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
////        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
////            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
////            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
////            P.z = 0;
////            for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
////            if (zbuffer[int(P.x+P.y*width)]<P.z) {
////                zbuffer[int(P.x+P.y*width)] = P.z;
////                image.set(P.x, P.y, color);
////            }
////        }
////    }
////}
//
//void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i uv0, Vec2i uv1, Vec2i uv2, TGAImage &image, int *zbuffer) {
//    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
//    if (t0.y>t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); }
//    if (t0.y>t2.y) { std::swap(t0, t2); std::swap(uv0, uv2); }
//    if (t1.y>t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); }
//
//    int total_height = t2.y-t0.y;
//    for (int i=0; i<total_height; i++) {
//        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
//        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
//        float alpha = (float)i/total_height;
//        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
//        Vec3i A   =               t0  + Vec3f(t2-t0  )*alpha;
//        Vec3i B   = second_half ? t1  + Vec3f(t2-t1  )*beta : t0  + Vec3f(t1-t0  )*beta;
//        Vec2i uvA =               uv0 +      (uv2-uv0)*alpha;
//        Vec2i uvB = second_half ? uv1 +      (uv2-uv1)*beta : uv0 +      (uv1-uv0)*beta;
//        if (A.x>B.x) { std::swap(A, B); std::swap(uvA, uvB); }
//        for (int j=A.x; j<=B.x; j++) {
//            float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
//            Vec3i   P = Vec3f(A) + Vec3f(B-A)*phi;
//            Vec2i uvP =     uvA +   (uvB-uvA)*phi;
//            int idx = P.x+P.y*width;
//            if (zbuffer[idx]<P.z) {
//                zbuffer[idx] = P.z;
//                TGAColor color = model->diffuse(uvP);
//                image.set(P.x, P.y, TGAColor(color.r, color.g, color.b));
//            }
//        }
//    }
//}
//
//Vec3f world2screen(Vec3f v) {
//    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
//}
//
//int main(int argc, char** argv) {
//    if(2==argc)
//        model = new Model(argv[1]);
//    else
//        model = new Model("obj/african_head.obj");
//
//	TGAImage image(width, height, TGAImage::RGB);
//
//// better way to show triangle
///*
//    TGAImage frame(200, 200, TGAImage::RGB);
//    Vec2i pts[3] = {Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160)};
//    triangle(pts, frame, TGAColor(255, 0, 0, 255));
//    frame.flip_vertically(); // to place the origin i
//
//    frame.flip_vertically(); // i want to have the origin at the left bottom corner of the image
//    frame.write_tga_file("framebuffer.tga");
//*/
//
///*
//    line(13, 20, 80, 40, image, white);
//    line(20, 13, 40, 80, image, red);
//    line(80, 40, 13, 20, image, red);
//
//    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
//    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
//    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
//    triangle(t0[0], t0[1], t0[2], image, red);
//    triangle(t1[0], t1[1], t1[2], image, white);
//    triangle(t2[0], t2[1], t2[2], image, green);
//     */
//// line model renderer
///*
//    for(int i=0; i<model->nfaces(); i++){
//        std::vector<int> face = model->face(i);
//        for(int j=0;j<3;j++){
//            Vec3f v0 = model->vert(face[j]);
//            Vec3f v1 = model->vert(face[(j+1)%3]);
//            int x0 = (v0.x+1.)*width/2.;
//            int y0 = (v0.y+1.)*width/2.;
//            int x1 = (v1.x+1.)*width/2.;
//            int y1 = (v1.y+1.)*width/2.;
//            line(Vec2i(x0,y0),Vec2i(x1,y1),image,white);
//        }
//    }
//    */
//
//// colorful model renderer
///*
//    for(int i=0; i<model->nfaces(); i++){
//        std::vector<int> face = model->face(i);
//        Vec2i screen_coords[3];
//        for(int j=0; j<3; j++){
//            Vec3f world_coords = model->vert(face[j]);
//            screen_coords[j] = Vec2i ((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.);
//        }
//        triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(rand()%255,rand()%255,rand()%255,255));
//    }
//*/
//
//// simple light renderer : the intensity of illumination is equal to the scalar product of the light vector and the normal to the given triangle
//// 光照强度：面的法向量与光照方向的点乘
///*
//    Vec3f light_dir(0,0,-1);
//
//    for(int i=0;i<model->nfaces();i++){
//        std::vector<int> face = model->face(i);
//        Vec2i screen_coords[3];
//        Vec3f world_coords[3];
//        for(int j=0;j<3;j++){
//            Vec3f v = model->vert(face[j]);
//            screen_coords[j] = Vec2i ((v.x+1.)*width/2., (v.y+1.)*height/2.);
//            world_coords[j] = v;
//        }
//        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
//        n.normalize();
//        float intensity = n*light_dir;
//        if(intensity>0){
//            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
//        }
//    }
//    */
//
//// y-buffer
///*
//    {
//        TGAImage scene(width, height, TGAImage::RGB);
//        // scene 2d mesh
//        line(Vec2i(20, 34),   Vec2i(744, 400), scene, red);
//        line(Vec2i(120, 434), Vec2i(444, 400), scene, green);
//        line(Vec2i(330, 463), Vec2i(594, 200), scene, blue);
//
//        // screen line
//        line(Vec2i(10, 10), Vec2i(790, 10), scene, white);
//
//        scene.flip_vertically();
//        scene.write_tga_file("scene.tga");
//    }
//
//    TGAImage render(width, 16, TGAImage::RGB);
//    int ybuffer[width];
//    for(int i=0; i<width; i++){
//        ybuffer[i] = std::numeric_limits<int>::min();
//    }
//
//    rasterize(Vec2i(20, 34),   Vec2i(744, 400), render, red,   ybuffer);
//    rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
//    rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue,  ybuffer);
//
//
//    render.flip_vertically(); // i want to have the origin at the left bottom corner of the image
//    render.write_tga_file("output.tga");
//*/
//
////    triangle(screen_coords, float *zbuffer, image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
//
//    int *zbuffer = new int[height*width];
//    for(int i=width*height; i--; zbuffer[i] = -std::numeric_limits<int>::max());
//
////    for(int i=0;i<model->nfaces();i++){
////        std::vector<int> face = model->face(i);
////        Vec3f pts[3];
////        for(int j=0;j<3;j++)
////            pts[j] = world2screen(model->vert(face[j]));
////
////        TGAColor color = model->diffuse(uvP);
////
////        triangle(pts, zbuffer, image, TGAColor(rand()%255, rand()%255, rand()%255, 255));
////
////    }
//
//    for (int i=0; i<model->nfaces(); i++) {
//        std::vector<int> face = model->face(i);
//        Vec3i pts[3];
//
//        for (int j=0; j<3; j++) {
//            Vec3f v = model->vert(face[j]);
//            pts[j] = world2screen(model->vert(face[j]));
//
//        }
//            Vec2i uv[3];
//            for (int k=0; k<3; k++) {
//                uv[k] = model->uv(i, k);
//            }
//            triangle(pts[0], pts[1], pts[2], uv[0], uv[1], uv[2], image, zbuffer);
//    }
//
//    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
//    image.write_tga_file("output.tga");
//
//	delete model;
//	return 0;
//}
//

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--;){
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if(std::abs(u[2]>1e-2))
        return Vec3f (1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1, 1, 1);
}

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) {
    Vec2f bboxmin( std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax( -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    Vec2f clamp(image.get_width()-1, image.get_height()-1);

    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3f P;
    for(P.x = bboxmin.x; P.x<=bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y<=bboxmax.y; P.y++){
            Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
            if(bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            for(int i=0; i<3; i++)
                P.z += pts[i][2] * bc_screen[i];
            if(zbuffer[int(P.x+P.y*width)]<P.z){
                zbuffer[int(P.x+P.y*width)]=P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}

int main(int argc, char** argv){
    if(2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    float *zbuffer = new float[width*height];
    for(int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage image(width, height, TGAImage::RGB);
    for(int i=0; i<model->nfaces(); i++){
        std::vector<int> face = model->face(i);
        Vec3f pts[3];
        for(int i=0;i<3;i++) pts[i] = world2screen(model->vert(face[i]));
        triangle(pts, zbuffer, image, TGAColor(rand()%255, rand()%255, rand()%255, 255));
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}