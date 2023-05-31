#include <vector>
#include <cmath>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
Model *model = NULL;
const int width = 800;
const int height = 800;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color){
    /*
    for(float t=0.;t<1.;t+=.01){
        int x = x0+(x1-x0)*t;
        int y = y0+(y1-y0)*t;
        image.set(x,y,color);
    }
    */
    int x0 = t0.x;
    int y0 = t0.y;
    int x1 = t1.x;
    int y1 = t1.y;

    bool steep = false;
    if(std::abs((x0-x1))<std::abs(y0-y1)){
        // 检查斜率 如果陡峭的话就交换一下xy
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if(x0>x1){
        std::swap(x0,x1);
        std::swap(y0,y1);
    }

    int dx=x1-x0;
    int dy=y1-y0;

    float derror2 = std::abs(dy)*2;
    float error2 = 0;
    int y=y0;

    for(int x=x0; x<=x1; x++){
        if(steep){
            image.set(y, x, color); //由于交换了xy，需要在返回结果时交换回去
        }else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if(error2>dx){
            y+=(y1>y0?1:-1);
            error2-=dx*2;
        }
    }
}

Vec3f barycentric(Vec2i* pts, Vec2i P) {
    Vec3f u = Vec3f(pts[2].x-pts[0].x, pts[1].x-pts[0].x, pts[0].x-P.x)^Vec3f(pts[2].y-pts[0].y, pts[1].y-pts[0].y, pts[0].y-P.y);
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z)<1) return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

void triangle(Vec2i *pts, TGAImage &image, TGAColor color) {
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }
    Vec2i P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts, P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            image.set(P.x, P.y, color);
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    // 对顶点排序，按照y的大小从高到低排序 => y0<y1<y2
    if(t0.y>t1.y)   std::swap(t0,t1);
    if(t0.y>t2.y)   std::swap(t0,t2);
    if(t1.y>t2.y)   std::swap(t1,t2);
    int total_height = t2.y - t0.y;

    for(int y=t0.y; y<t1.y; y++){
        int segment_height = t1.y-t0.y+1; // 谨防除0
        float alpha = (float)(y-t0.y)/total_height;
        float beta = (float)(y-t0.y)/segment_height;
        Vec2i A = t0 + (t2-t0)*alpha;
        Vec2i B = t0 + (t1-t0)*beta;
        if(A.x>B.x) std::swap(A, B);
        for(int j=A.x;j<=B.x;j++){
            image.set(j,y,color);
        }
    }

    for(int y=t1.y; y<t2.y; y++){
        int segment_height = t2.y-t1.y+1; // 谨防除0
        float alpha = (float)(y-t0.y)/total_height;
        float beta = (float)(y-t1.y)/segment_height;
        Vec2i A = t0 + (t2-t0)*alpha;
        Vec2i B = t1 + (t2-t1)*beta;
        if(A.x>B.x) std::swap(A, B);
        for(int j=A.x;j<=B.x;j++){
            image.set(j,y,color);
        }
    }

//    line(t0, t1, image, green);
//    line(t1, t2, image, green);
//    line(t2, t0, image, red);
}

int main(int argc, char** argv) {
    if(2==argc)
        model = new Model(argv[1]);
    else
        model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB);

    TGAImage frame(200, 200, TGAImage::RGB);
    Vec2i pts[3] = {Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160)};
    triangle(pts, frame, TGAColor(255, 0, 0, 255));
    frame.flip_vertically(); // to place the origin i

//    line(13, 20, 80, 40, image, white);
//    line(20, 13, 40, 80, image, red);
//    line(80, 40, 13, 20, image, red);

//    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
//    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
//    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
//    triangle(t0[0], t0[1], t0[2], image, red);
//    triangle(t1[0], t1[1], t1[2], image, white);
//    triangle(t2[0], t2[1], t2[2], image, green);

// line model renderer

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

// colorful model renderer

//    for(int i=0; i<model->nfaces(); i++){
//        std::vector<int> face = model->face(i);
//        Vec2i screen_coords[3];
//        for(int j=0; j<3; j++){
//            Vec3f world_coords = model->vert(face[j]);
//            screen_coords[j] = Vec2i ((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.);
//        }
//        triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(rand()%255,rand()%255,rand()%255,255));
//    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

    frame.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    frame.write_tga_file("framebuffer.tga");
	delete model;
	return 0;
}

