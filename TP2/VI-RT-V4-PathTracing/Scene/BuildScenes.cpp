//
//  BuildScenes.cpp
//  VI-RT-InitialVersion
//
//  Created by Luis Paulo Santos on 11/02/2025.
//

#include "BuildScenes.hpp"
#include "DiffuseTexture.hpp"
#include "../utils/common.hpp"
#include "../Matrix/matrix.hpp"

static int AddDiffuseMat (Scene& scene, RGB const color);
static int AddMat (Scene& scene, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta=1.f);
static int AddTextMat (Scene& scene, std::string filename, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta=1.f);
static void AddSphere (Scene& scene, Point const C, float const radius,
                            int const mat_ndx);
static void AddTriangle (Scene& scene,
                        Point const v1, Point const v2, Point const v3,
                        int const mat_ndx);


static int AddDiffuseMat (Scene& scene, RGB const color) {
    BRDF *brdf = new BRDF;
    
    brdf->Ka = color;
    brdf->Kd = color;
    brdf->Ks = RGB(0., 0., 0.);
    brdf->Kt = RGB(0., 0., 0.);
    
    return (scene.AddMaterial(brdf));
}

static int AddTextMat (Scene& scene, std::string filename, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta) {
    DiffuseTexture *brdf = new DiffuseTexture(filename);
    
    brdf->Ka = Ka;
    brdf->Kd = Kd;
    brdf->Ks = Ks;
    brdf->Kt = Kt;
    brdf->eta = eta;
    brdf->textured = true;
    
    return (scene.AddMaterial(brdf));
}


static int AddMat (Scene& scene, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta) {
    BRDF *brdf = new BRDF;
    
    brdf->Ka = Ka;
    brdf->Kd = Kd;
    brdf->Ks = Ks;
    brdf->Kt = Kt;
    brdf->eta = eta;
    
    return (scene.AddMaterial(brdf));
}

static void AddSphere (Scene& scene, Point const C,
                             float const radius, int const mat_ndx) {
    Sphere *sphere = new Sphere(C, radius);
    Primitive *prim = new Primitive;
    prim->g = sphere;
    prim->material_ndx = mat_ndx;
    scene.AddPrimitive(prim);
}

static void AddTriangle (Scene& scene,
                         Point const v1, Point const v2, Point const v3,
                         int const mat_ndx) {
    
    Triangle *tri = new Triangle(v1, v2, v3);
    Primitive *prim = new Primitive;
    prim->g = tri;
    prim->material_ndx = mat_ndx;
    scene.AddPrimitive(prim);
}

static void AddTriangleUV (Scene& scene,
                         Point const v1, Point const v2, Point const v3,
                           Vec2 const uv1, Vec2 const uv2, Vec2 const uv3,
                         int const mat_ndx) {
    
    Triangle *tri = new Triangle(v1, v2, v3);
    tri->set_uv(uv1, uv2, uv3);
    Primitive *prim = new Primitive;
    prim->g = tri;
    prim->material_ndx = mat_ndx;
    scene.AddPrimitive(prim);
}


// Scene with single triangle
void SingleTriScene (Scene& scene){
    int const mat = AddDiffuseMat(scene, RGB (0.99, 0.99, 0.99));
    AddTriangle(scene, Point(-5., 5., 0.), Point(0., -5., 0.), Point(5., 5., 0.), mat);
    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    PointLight *p1 = new PointLight(RGB(0.7,0.7,0.7),Point(0,0,-10));
    scene.lights.push_back(p1);
    scene.numLights++;
    return ;
}

// Scene with  spheres
void SpheresScene (Scene& scene, int const N_spheres){
    int const red_mat = AddDiffuseMat(scene, RGB (0.9, 0.1, 0.1));
    AddSphere(scene, Point(0., 0., 3.), 0.8, red_mat);
    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.5,0.5,0.5));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    PointLight *p1 = new PointLight(RGB(0.7,0.7,0.7),Point(0,2.0,0));
    scene.lights.push_back(p1);
    scene.numLights++;
    return ;
}

// Scene with  sphere and 4 triangles
void SpheresTriScene (Scene& scene) {
    int const red_mat = AddDiffuseMat(scene, RGB (0.9, 0.1, 0.1));
    int const green_mat = AddDiffuseMat(scene, RGB (0.1, 0.9, 0.1));
    AddSphere(scene, Point(0., 0., 3.), 0.8, red_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(-2., 1.5, 4.), Point(-0.5, 1.5, 5.),green_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(0.5, 1.5, 5.), Point(2., 1.5, 4.),green_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(-0.5, -1.5, 5.), Point(-2., -1.5, 4.),green_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(0.5, -1.5, 5.), Point(2., -1.5, 4.), green_mat);
    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.5,0.5,0.5));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    PointLight *p1 = new PointLight(RGB(0.7,0.7,0.7),Point(0,2.0,0));
    scene.lights.push_back(p1);
    scene.numLights++;
    return ;
}

void matrixesCheck(int frame, std::vector<Matrix>& matrixes, std::vector<Model>& models) {

    for (auto& matrix : matrixes) {
        if (frame >= matrix.frame && frame < matrix.frame + matrix.totalFrames) {
            // Apply transformations to the models based on the matrix
            for (int model_index : matrix.models_indexes) {
                Model& model = models[model_index];
                for (auto& vertex : model.vertices) {
                    float vec[4] = {vertex.X, vertex.Y, vertex.Z, 1};
                    matrix.transformPoint(vec);
                    vertex.X = vec[0];
                    vertex.Y = vec[1];
                    vertex.Z = vec[2];
                }
            }
        }
    }

}

void CornellBox(int frame, Scene& scene, std::vector<Model>& models, std::vector<Matrix>& matrixes) {
    // Definição dos materiais
    int const text_backwall = AddTextMat(scene, "Dog.ppm", RGB(0.3, 0.3, 0.3), RGB(0.9, 0.9, 0.9), RGB(0., 0., 0.), RGB(0., 0., 0.));
    int const uminho_text = AddTextMat(scene, "UMinho.ppm", RGB(0.3, 0.3, 0.3), RGB(0.9, 0.9, 0.9), RGB(0., 0., 0.), RGB(0., 0., 0.));
    int const white_mat = AddMat(scene, RGB(0.2, 0.2, 0.2), RGB(0.4, 0.4, 0.4), RGB(0., 0., 0.), RGB(0., 0., 0.));
    int const red_mat = AddMat(scene, RGB(0.9, 0., 0.), RGB(0.4, 0., 0.), RGB(0., 0., 0.), RGB(0., 0., 0.));
    int const green_mat = AddMat(scene, RGB(0., 0.9, 0.), RGB(0., 0.2, 0.), RGB(0., 0., 0.), RGB(0., 0., 0.));
    int const blue_mat = AddMat(scene, RGB(0., 0., 0.9), RGB(0., 0., 0.4), RGB(0., 0., 0.), RGB(0., 0., 0.));
    int const orange_mat = AddMat(scene, RGB(0.99, 0.65, 0.), RGB(0.37, 0.24, 0.), RGB(0., 0., 0.), RGB(0., 0., 0.));
    int const mirror_mat = AddMat(scene, RGB(0., 0., 0.), RGB(0., 0., 0.), RGB(0.9, 0.9, 0.9), RGB(0., 0., 0.));
    int const glass_mat = AddMat(scene, RGB(0., 0., 0.), RGB(0., 0., 0.), RGB(0.2, 0.2, 0.2), RGB(0.9, 0.9, 0.9), 1.2);

    // Aplicação das transformações
    matrixesCheck(frame, matrixes, models);

    // Floor
    Model& floor_model = models[0];
    AddTriangle(scene, floor_model.vertices[0], floor_model.vertices[1], floor_model.vertices[2], white_mat);
    AddTriangle(scene, floor_model.vertices[3], floor_model.vertices[4], floor_model.vertices[5], white_mat);
    
    // Ceiling
    Model& ceiling_model = models[1];
    AddTriangle(scene, ceiling_model.vertices[0], ceiling_model.vertices[1], ceiling_model.vertices[2], white_mat);
    AddTriangle(scene, ceiling_model.vertices[3], ceiling_model.vertices[4], ceiling_model.vertices[5], white_mat);

    // Back wall
    //AddTriangleUV (scene, Point(0.0, 0.0, 559.2), Point(549.6, 0.0, 559.2), Point(556.0, 548.8, 559.2), Vec2(1.,1.), Vec2(0.,1.), Vec2(0.,0.), text_backwall);
    //AddTriangleUV(scene, Point(0.0, 0.0, 559.2), Point(0.0, 548.8, 559.2), Point(556.0, 548.8, 559.2), Vec2(1.,1.), Vec2(1.,0.), Vec2(0.,0.), text_backwall);
    Model& back_wall_model = models[2];
    AddTriangle(scene, back_wall_model.vertices[0], back_wall_model.vertices[1], back_wall_model.vertices[2], white_mat);
    AddTriangle(scene, back_wall_model.vertices[3], back_wall_model.vertices[4], back_wall_model.vertices[5], white_mat);

    // Left Wall
    Model& left_wall_model = models[3];
    AddTriangle(scene, left_wall_model.vertices[0], left_wall_model.vertices[1], left_wall_model.vertices[2], green_mat);
    AddTriangle(scene, left_wall_model.vertices[3], left_wall_model.vertices[4], left_wall_model.vertices[5], green_mat);

    // Right Wall
    Model& right_wall_model = models[4];
    AddTriangle(scene, right_wall_model.vertices[0], right_wall_model.vertices[1], right_wall_model.vertices[2], red_mat);
    AddTriangle(scene, right_wall_model.vertices[3], right_wall_model.vertices[4], right_wall_model.vertices[5], red_mat);

    // Right Wall Mirror
    Model& right_wall_mirror_model = models[5];
    AddTriangle(scene, right_wall_mirror_model.vertices[0], right_wall_mirror_model.vertices[1], right_wall_mirror_model.vertices[2], mirror_mat);
    AddTriangle(scene, right_wall_mirror_model.vertices[3], right_wall_mirror_model.vertices[4], right_wall_mirror_model.vertices[5], mirror_mat);
    
    // short block
    Model& short_block_model = models[6];
    // top
    //AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(0.,1.), Vec2(1.,1.), uminho_text);
    //AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(1.,0.), Vec2(1.,1.), uminho_text);
    AddTriangle(scene, short_block_model.vertices[0], short_block_model.vertices[1], short_block_model.vertices[2], orange_mat);
    AddTriangle(scene, short_block_model.vertices[3], short_block_model.vertices[4], short_block_model.vertices[5], orange_mat);

    // bottom
    AddTriangle(scene, short_block_model.vertices[6], short_block_model.vertices[7], short_block_model.vertices[8], orange_mat);
    AddTriangle(scene, short_block_model.vertices[9], short_block_model.vertices[10], short_block_model.vertices[11], orange_mat);

    // left
    AddTriangle(scene, short_block_model.vertices[12], short_block_model.vertices[13], short_block_model.vertices[14], orange_mat);
    AddTriangle(scene, short_block_model.vertices[15], short_block_model.vertices[16], short_block_model.vertices[17], orange_mat);

    // back
    AddTriangle(scene, short_block_model.vertices[18], short_block_model.vertices[19], short_block_model.vertices[20], orange_mat);
    AddTriangle(scene, short_block_model.vertices[21], short_block_model.vertices[22], short_block_model.vertices[23], orange_mat);

    // right
    AddTriangle(scene, short_block_model.vertices[24], short_block_model.vertices[25], short_block_model.vertices[26], orange_mat);
    AddTriangle(scene, short_block_model.vertices[27], short_block_model.vertices[28], short_block_model.vertices[29], orange_mat);
    
    // front
    AddTriangle(scene, short_block_model.vertices[30], short_block_model.vertices[31], short_block_model.vertices[32], orange_mat);
    AddTriangle(scene, short_block_model.vertices[33], short_block_model.vertices[34], short_block_model.vertices[35], orange_mat);

    // tall block
    Model& tall_block_model = models[7];
    // top
    AddTriangle(scene, tall_block_model.vertices[0], tall_block_model.vertices[1], tall_block_model.vertices[2], blue_mat);
    AddTriangle(scene, tall_block_model.vertices[3], tall_block_model.vertices[4], tall_block_model.vertices[5], blue_mat);

    // bottom
    AddTriangle(scene, tall_block_model.vertices[6], tall_block_model.vertices[7], tall_block_model.vertices[8], blue_mat);
    AddTriangle(scene, tall_block_model.vertices[9], tall_block_model.vertices[10], tall_block_model.vertices[11], blue_mat);

    // left
    AddTriangle(scene, tall_block_model.vertices[12], tall_block_model.vertices[13], tall_block_model.vertices[14], blue_mat);
    AddTriangle(scene, tall_block_model.vertices[15], tall_block_model.vertices[16], tall_block_model.vertices[17], blue_mat);

    // back
    AddTriangle(scene, tall_block_model.vertices[18], tall_block_model.vertices[19], tall_block_model.vertices[20], blue_mat);
    AddTriangle(scene, tall_block_model.vertices[21], tall_block_model.vertices[22], tall_block_model.vertices[23], blue_mat);

    // right
    AddTriangle(scene, tall_block_model.vertices[24], tall_block_model.vertices[25], tall_block_model.vertices[26], blue_mat);
    AddTriangle(scene, tall_block_model.vertices[27], tall_block_model.vertices[28], tall_block_model.vertices[29], blue_mat);
    
    // front
    AddTriangle(scene, tall_block_model.vertices[30], tall_block_model.vertices[31], tall_block_model.vertices[32], blue_mat);
    AddTriangle(scene, tall_block_model.vertices[33], tall_block_model.vertices[34], tall_block_model.vertices[35], blue_mat);
    
    // transparent sphere

    Model& sphere_model = models[8];
    Point center = sphere_model.vertices[0];
    double radius = sphere_model.radius;
    AddSphere(scene, center, radius, glass_mat);

    //AddSphere(scene, sphere_model.vertices[0], sphere_model.radius, glass_mat);

    #define AREA
    #ifndef AREA
        for (int x=-1 ; x<2 ; x++) {
            for (int z=-1 ; z<2 ; z++) {
                PointLight *p = new PointLight(RGB(30000.,30000.,30000.),Point(278.+x*150.,545.,280.+z*150));
                scene.lights.push_back(p);
                scene.numLights++;
            }
        }
    #else
        for (int lll=-1 ; lll<2 ; lll++) {
            AreaLight *a1 = new AreaLight(RGB(250000.,250000.,250000.), Point(250.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
                scene.lights.push_back(a1);
                scene.numLights++;
            AreaLight *a2 = new AreaLight(RGB(250000.,250000.,250000.), Point(250.+lll*150, 545., 250.+lll*150), Point(250.+lll*150, 545., 300.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
                scene.lights.push_back(a2);
                scene.numLights++;
        }
    #endif
        return ;
        
}

// Diffuse Cornell Box
void DiffuseCornellBox (Scene& scene) {
    int const text_backwall = AddTextMat(scene, "Dog.ppm", RGB (0.3, 0.3, 0.3), RGB (0.8, 0.8, 0.8), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const uminho_text = AddTextMat(scene, "UMinho.ppm", RGB (0.3, 0.3, 0.3), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const white_mat = AddMat(scene, RGB (0.1, 0.1, 0.1), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const red_mat = AddMat(scene, RGB (0.1, 0., 0.), RGB (0.6, 0., 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const green_mat = AddMat(scene, RGB (0., 0.1, 0.), RGB (0., 0.6, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const blue_mat = AddMat(scene, RGB (0., 0., 0.1), RGB (0., 0., 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const orange_mat = AddMat(scene, RGB (0.37, 0.24, 0.), RGB (0.66, 0.44, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    // Floor
    AddTriangle(scene, Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 559.2), white_mat);
    AddTriangle(scene, Point(549.6, 0.0, 559.2), Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 559.2), white_mat);
    // Ceiling
    AddTriangle(scene, Point(556.0, 548.8, 0.0), Point(0.0, 548.8, 0.0), Point(0.0, 548.8, 559.2), white_mat);
    AddTriangle(scene, Point(556.0, 548.8, 559.2), Point(556.0, 548.8, 0.0), Point(0., 548.8, 559.2), white_mat);
    // Back wall
    AddTriangleUV (scene, Point(0.0, 0.0, 559.2), Point(549.6, 0.0, 559.2), Point(556.0, 548.8, 559.2), Vec2(1.,1.), Vec2(0.,1.), Vec2(0.,0.), text_backwall);
    AddTriangleUV(scene, Point(0.0, 0.0, 559.2), Point(0.0, 548.8, 559.2), Point(556.0, 548.8, 559.2), Vec2(1.,1.), Vec2(1.,0.), Vec2(0.,0.), text_backwall);
    //AddTriangle (scene, Point(0.0, 0.0, 559.2), Point(549.6, 0.0, 559.2), Point(556.0, 548.8, 559.2), white_mat);
    //AddTriangle(scene, Point(0.0, 0.0, 559.2), Point(0.0, 548.8, 559.2), Point(556.0, 548.8, 559.2), white_mat);
    // Left Wall
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 0., 559.2), Point(0., 548.8, 559.2), green_mat);
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 548.8, 0.), Point(0., 548.8, 559.2), green_mat);
    // Right Wall
    AddTriangle(scene, Point(552.8, 0.0, 0.), Point(549.6, 0., 559.2), Point(549.6, 548.8, 559.2), red_mat);
    AddTriangle(scene, Point(552.8, 0.0, 0.), Point(552.8, 548.8, 0.), Point(549.6, 548.8, 559.2), red_mat);

    // short block
    // top
    AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(0.,1.), Vec2(1.,1.), uminho_text);
    AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(1.,0.), Vec2(1.,1.), uminho_text);
    //AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), orange_mat);
    //AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    // bottom
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 82.0, 0.01, 225.0), Point(240.0, 0.01, 272.0), orange_mat);
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 290.0, 0.01, 114.0), Point(240.0, 0.01, 272.0), orange_mat);
    // left
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point(  290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point( 240.0,  0.0, 272.0), Point(240.0, 165.0, 272.0), orange_mat);
    // back
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(240.0, 165.0, 272.0), Point(82.0, 165., 225.0), orange_mat);
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), orange_mat);
    // right
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), Point(130.0, 165.0, 65.0), orange_mat);
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(130.0, 0.0, 65.0), Point(130.0, 165.0, 65.0), orange_mat);
    // front
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(130.0, 165.0, 65.0), Point(290.0, 165.0, 114.0), orange_mat);
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(290.0, 0.0, 114.0), Point(290.0, 165.0, 114.0), orange_mat);

    // tall block
    // top
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(265.0, 330.0, 296.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // bottom
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(265.0, 0.1, 296.0), Point(314.0, 0.1, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(472.0, 0.1, 406.0), Point(314.0, 0.1, 456.0), blue_mat);
    // left
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), blue_mat);
    // back
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(314.0, 0.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // right
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(314.0, 330.0, 456.0), Point(265.0, 330.0, 296.0), blue_mat);
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), blue_mat);
    // front
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), Point(423.0, 330.0, 247.0), blue_mat);
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), blue_mat);
    
  
    // add an ambient light to the scene
    //AmbientLight *ambient = new AmbientLight(RGB(0.15,0.15,0.15));
    //AmbientLight *ambient = new AmbientLight(RGB(0.07,0.07,0.07));
    //scene.lights.push_back(ambient);
    //scene.numLights++;
#define AREA
#ifndef AREA
    for (int x=-1 ; x<2 ; x++) {
        for (int z=-1 ; z<2 ; z++) {
            PointLight *p = new PointLight(RGB(0.16,0.16,0.16),Point(278.+x*150.,545.,280.+z*150));
            scene.lights.push_back(p);
            scene.numLights++;
        }
    }
#else
    for (int lll=-1 ; lll<2 ; lll++) {
        AreaLight *a1 = new AreaLight(RGB(.2,.2,.2), Point(250.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(.2,.2,.2), Point(250.+lll*150, 545., 250.+lll*150), Point(250.+lll*150, 545., 300.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
#endif
    return ;
}

// DLight Challenge
void DLightChallenge (Scene& scene) {
    int const text_backwall = AddTextMat(scene, "Dog.ppm", RGB (0.3, 0.3, 0.3), RGB (0.8, 0.8, 0.8), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const uminho_text = AddTextMat(scene, "UMinho.ppm", RGB (0.3, 0.3, 0.3), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const white_mat = AddMat(scene, RGB (0.1, 0.1, 0.1), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const red_mat = AddMat(scene, RGB (0.1, 0., 0.), RGB (0.5, 0.1, 0.1), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const green_mat = AddMat(scene, RGB (0., 0.1, 0.), RGB (0., 0.6, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const blue_mat = AddMat(scene, RGB (0., 0., 0.1), RGB (0., 0., 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const orange_mat = AddMat(scene, RGB (0.37, 0.24, 0.), RGB (0.66, 0.44, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));

    // Floor
    AddTriangle(scene, Point(552.8, 0.0, 0.0), Point(-100.0, 0.0, 0.0), Point(-100.0, 0.0, 859.2), white_mat);
    AddTriangle(scene, Point(549.6, 0.0, 859.2), Point(552.8, 0.0, 0.0), Point(-100.0, 0.0, 859.2), white_mat);
    // Ceiling
    AddTriangle(scene, Point(556.0, 548.8, 0.0), Point(-100.0, 548.8, 0.0), Point(-100.0, 548.8, 859.2), white_mat);
    AddTriangle(scene, Point(556.0, 548.8, 859.2), Point(556.0, 548.8, 0.0), Point(-100., 548.8, 859.2), white_mat);
    // Back wall
    AddTriangleUV (scene, Point(-100.0, 0.0, 859.2), Point(549.6, 0.0, 859.2), Point(556.0, 548.8, 859.2), Vec2(1.,1.), Vec2(0.,1.), Vec2(0.,0.), text_backwall);
    AddTriangleUV(scene, Point(-100.0, 0.0, 859.2), Point(-100.0, 548.8, 859.2), Point(556.0, 548.8, 859.2), Vec2(1.,1.), Vec2(1.,0.), Vec2(0.,0.), text_backwall);
    // Left Wall
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 0., 459.2), Point(0., 548.8, 459.2), green_mat);
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 548.8, 0.), Point(0., 548.8, 459.2), green_mat);
    // L walls
    AddTriangle(scene, Point(-100.0, 0.0, 459.2), Point(-100., 0., 859.2), Point(-100., 548.8, 859.2), white_mat);
    AddTriangle(scene, Point(-100.0, 0.0, 459.2), Point(-100., 548.8, 459.2), Point(-100., 548.8, 859.2), white_mat);
    AddTriangle (scene, Point(-100.0, 0.0, 459.2), Point(0., 0.0, 459.2), Point(0., 548.8, 459.2),  white_mat);
    AddTriangle(scene, Point(-100.0, 0.0, 459.2), Point(-100.0, 548.8, 459.2), Point(0., 548.8, 459.2), white_mat);
    // Right Wall
    
    AddTriangle(scene, Point(552.8, 0.0, 0.0), Point(549.6, 0., 859.2), Point(549.6, 548.8, 859.2), red_mat);
    AddTriangle(scene, Point(552.8, 0.0, 0.0), Point(552.8, 548.8, 0.0), Point(549.6, 548.8, 859.2), red_mat);

    // short block
    // top
    AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(0.,1.), Vec2(1.,1.), uminho_text);
    AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(1.,0.), Vec2(1.,1.), uminho_text);
    //AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), orange_mat);
    //AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    // bottom
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 82.0, 0.01, 225.0), Point(240.0, 0.01, 272.0), orange_mat);
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 290.0, 0.01, 114.0), Point(240.0, 0.01, 272.0), orange_mat);
    // left
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point(  290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point( 240.0,  0.0, 272.0), Point(240.0, 165.0, 272.0), orange_mat);
    // back
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(240.0, 165.0, 272.0), Point(82.0, 165., 225.0), orange_mat);
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), orange_mat);
    // right
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), Point(130.0, 165.0, 65.0), orange_mat);
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(130.0, 0.0, 65.0), Point(130.0, 165.0, 65.0), orange_mat);
    // front
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(130.0, 165.0, 65.0), Point(290.0, 165.0, 114.0), orange_mat);
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(290.0, 0.0, 114.0), Point(290.0, 165.0, 114.0), orange_mat);

    // tall block
    // top
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(265.0, 330.0, 296.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // bottom
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(265.0, 0.1, 296.0), Point(314.0, 0.1, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(472.0, 0.1, 406.0), Point(314.0, 0.1, 456.0), blue_mat);
    // left
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), blue_mat);
    // back
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(314.0, 0.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // right
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(314.0, 330.0, 456.0), Point(265.0, 330.0, 296.0), blue_mat);
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), blue_mat);
    // front
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), Point(423.0, 330.0, 247.0), blue_mat);
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), blue_mat);
    
  
    for (int llz=-1 ; llz<2 ; llz++) {
        for (int llx=-1 ; llx<2 ; llx++) {
            AreaLight *a1 = new AreaLight(RGB(5000.-(llx+llz)*2000.,5000. -(llx+llz)*2000.,5000.-(llx+llz)*2000.), Point(250.+llx*150, 545., 250.+llz*150), Point(300.+llx*150, 545., 250.+llz*150), Point(300.+llx*150, 545., 300.+llz*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
            AreaLight *a2 = new AreaLight(RGB(5000.-(llx+llz)*2000.,5000.-(llx+llz)*2000.,5000.-(llx+llz)*2000.), Point(250.+llx*150, 545., 250.+llz*150), Point(250.+llx*150, 545., 300.+llz*150), Point(300.+llx*150, 545., 300.+llz*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
        }
    }
    for (int lll=0 ; lll<2 ; lll++) {
        AreaLight *a1 = new AreaLight(RGB(15000.+lll*4000,15000.+lll*4000,15000.+lll*4000), Point(-10., 20.+250*lll, 459.3), Point(-10., 90.+250*lll, 459.3), Point(-90, 90.+250*lll, 459.3), Vector (0.,0.,1.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(15000.+lll*4000,15000.+lll*4000,15000.+lll*4000), Point(-10., 20.+250*lll, 459.3), Point(-90., 20.+250*lll, 459.3), Point(-90, 90.+250*lll, 459.3), Vector (0.,0.,1.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
    for (int lll=0 ; lll<2 ; lll++) {
        AreaLight *a1 = new AreaLight(RGB(2000.-lll*500,2000.-lll*500.,1000. -lll*500), Point(0.01, 20., 20.+lll*200.), Point(0.01, 20., 100.+lll*200.), Point(0.01, 30., 100.+lll*200.), Vector (1.,0.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(2000.-lll*500,2000.-lll*500,1000. -lll*500), Point(0.01, 20., 20.+lll*200.), Point(0.01, 30., 20.+lll*200.), Point(0.01, 30., 100.+lll*200.), Vector (1.,0.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
    for (int lll=0 ; lll<4 ; lll++) {
        AreaLight *a1 = new AreaLight(RGB(2000.-lll*450,2000.-lll*450.,1000. -lll*300), Point(549.59, 20., 20.+lll*200.), Point(549.59, 20., 100.+lll*200.), Point(549.59, 30., 100.+lll*200.), Vector (-1.,0.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(2000.-lll*450,2000.-lll*450,1000. -lll*300), Point(549.59, 20., 20.+lll*200.), Point(549.59, 30., 20.+lll*200.), Point(549.59, 30., 100.+lll*200.), Vector (-1.,0.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
    { // blue block light
        AreaLight *a1 = new AreaLight(RGB(4000.,4000.0,10000.), Point(340.0, 0.01, 220.0), Point(340.0, 0.01, 230.0), Point(350.0, 0.01, 230.0), Vector (0.,1.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(4000.,4000.0,10000.), Point(340.0, 0.01, 220.0), Point(350.0, 0.01, 220.0), Point(350.0, 0.01, 230.0), Vector (0.,1.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
    { // orange block light
        AreaLight *a1 = new AreaLight(RGB(4000.,4000.0,10000.), Point(210.0, 0.01, 60.0), Point(210., 0.01, 70.0), Point(220., 0.01, 70.0), Vector (0.,1.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(4000.,4000.0,10000.), Point(210., 0.01, 60.0), Point(220., 0.01, 60.0), Point(220., 0.01, 70.0), Vector (0.,1.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
    return ;
}


// Scene for testing defocus blur 5 triangles
void DeFocusTriScene (Scene& scene) {
    int const red_mat = AddDiffuseMat(scene, RGB (0.9, 0.1, 0.1));
    int const green_mat = AddDiffuseMat(scene, RGB (0.1, 0.9, 0.1));
    int const brown_mat = AddDiffuseMat(scene, RGB (210./256.,105./256.,30./256.));
    // floor
    AddTriangle(scene, Point(-20., -0.1, -20.), Point(-20., -0.1, 20.), Point(20., -0.1, 20.),brown_mat);
    AddTriangle(scene, Point(-20., -0.1, -20.), Point(20., -0.1, -20.), Point(20., -0.1, 20.),brown_mat);

    float const Xbase=0.;
    float const Zbase=10.;
    // central triangle
    AddTriangle(scene, Point(Xbase-0.5, 1., Zbase), Point(Xbase+0.5, 1., Zbase), Point(Xbase, 0.1, Zbase),green_mat);
    //AddTriangle(scene, Point(-0.5, 1., 10.), Point(0.5, 1., 10.), Point(0., 0.1, 10.),green_mat);
    AddTriangle(scene, Point(Xbase+0.5, 1., Zbase+1.), Point(Xbase+1.5, 1., Zbase+1.), Point(Xbase+1., 0.1, Zbase+1.),red_mat);
    AddTriangle(scene, Point(Xbase+1.5, 1., Zbase+2.), Point(Xbase+2.5, 1., Zbase+2.), Point(Xbase+2., 0.1, Zbase+2.),green_mat);
    AddTriangle(scene, Point(Xbase-1.0, 1., Zbase-1.), Point(Xbase, 1., Zbase-1.), Point(Xbase-0.5, 0.1, Zbase-1.),red_mat);
    AddTriangle(scene, Point(Xbase-1.5, 1., Zbase-2.), Point(Xbase-0.5, 1., Zbase-2.), Point(Xbase-1., 0.1, Zbase-2.),green_mat);

    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.5,0.5,0.5));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    return ;
}

void EnvScene(Scene &scene){
    int const white_mat = AddMat(scene, RGB (0.1, 0.1, 0.1), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const red_mat = AddMat(scene, RGB (0.9, 0., 0.), RGB (0.4, 0., 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const glass_mat = AddMat(scene, RGB (0., 0., 0.), RGB (0., 0., 0.), RGB (0.2, 0.2, 0.2), RGB (0.9, 0.9, 0.9), 1.2);
    int const mirror_mat = AddMat(scene, RGB (0., 0., 0.), RGB (0., 0., 0.), RGB (0.9, 0.9, 0.9), RGB (0., 0., 0.));
    int const green_mat = AddDiffuseMat(scene, RGB (0.1, 0.9, 0.1));
    int const orange_mat = AddMat(scene, RGB (0.37, 0.24, 0.), RGB (0.66, 0.44, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));

    // Piso
    AddTriangle(scene, Point(600, 0.0, 0.0), Point(-100.0, 0.0, 0.0), Point(-100.0, 0.0, 800.0), white_mat);
    AddTriangle(scene, Point(600, 0.0, 800.0), Point(600, 0.0, 0.0), Point(-100.0, 0.0, 800.0), white_mat);

    // Esfera grande de vidro
    AddSphere(scene, Point(278., 100., 250.), 100., glass_mat);

    // Esfera vermelha à esquerda
    AddSphere(scene, Point(90., 60., 380.), 60., red_mat);

    // Esfera reflexiva à direita
    AddSphere(scene, Point(400., 60., 380.), 60., mirror_mat); 

    // Esfera difusa ao fundo
    AddSphere(scene, Point(60., 50., 100.), 50., green_mat);


    //#define AREANOENV

    #ifndef AREANOENV
        EnvironmentLight *envLight = new EnvironmentLight("rnl_probe.hdr");
        scene.lights.push_back(envLight);
        scene.numLights++;
    #else
        for (int lll=-1 ; lll<1 ; lll++) {
            AreaLight *a1 = new AreaLight(RGB(250000.,250000.,250000.), Point(250.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
                scene.lights.push_back(a1);
                scene.numLights++;
            AreaLight *a2 = new AreaLight(RGB(250000.,250000.,250000.), Point(250.+lll*150, 545., 250.+lll*150), Point(250.+lll*150, 545., 300.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
                scene.lights.push_back(a2);
                scene.numLights++;
        }
    #endif

    return;
}
