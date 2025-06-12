//
//  main.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include <vector>
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include "scene.hpp"
#include "Perspective.hpp"
#include "DummyRenderer.hpp"
#include "StandardRenderer.hpp"
#include "ImagePPM.hpp"
#include "AmbientShader.hpp"
#include "WhittedShader.hpp"
#include "DistributedShader.hpp"
#include "PathTracingShader.hpp"
#include "directLighting.hpp"
#include "AmbientLight.hpp"
#include "Sphere.hpp"
#include "BuildScenes.hpp"
#include <time.h>
#include "../tinyxml2-master/tinyxml2.h"
#include "utils/common.hpp"
#include "Matrix/matrix.hpp"

Group og_group = Group();
std::vector<Matrix> matrixes;

// TODO (maybe) : TALVEZ TENHAS DE VER AQUI A CENA DE METER O TRATAMENTO DE FRAME, TOTALFRAMES E MODELS
// DENTRO DO LOOP FOR ABAIXO 
void processTransformElement(tinyxml2::XMLElement* transformElement, Group& og_group) {
    float tx = 0, ty = 0, tz = 0;
    float rx = 0, ry = 0, rz = 0, angle = 0;
    float sx = 1, sy = 1, sz = 1;
    float time = 0;
    bool align = false;

    int frame = 0, totalFrames = 0;
    std::vector<int> models;

    // Extrair atributos de frame, totalFrames e models
    tinyxml2::XMLElement* frameElem = transformElement->FirstChildElement("frame");
    if (frameElem) frameElem->QueryIntAttribute("n", &frame);

    tinyxml2::XMLElement* totalFramesElem = transformElement->FirstChildElement("totalFrames");
    if (totalFramesElem) totalFramesElem->QueryIntAttribute("n", &totalFrames);

    tinyxml2::XMLElement* modelsElem = transformElement->FirstChildElement("models");
    if (modelsElem) {
        const char* list = modelsElem->Attribute("list");
        if (list) {
            std::stringstream ss(list);
            char ch;
            int modelIndex;
            while (ss >> ch) {
                if (isdigit(ch)) {
                    ss.putback(ch);
                    ss >> modelIndex;
                    models.push_back(modelIndex);
                }
            }
        }
    }

    // Iterar sobre as transformações dentro do elemento <transform>
    for (tinyxml2::XMLElement* child = transformElement->FirstChildElement(); child; child = child->NextSiblingElement()) {
        const char* childName = child->Name();

        if (strcmp(childName, "translate") == 0) {

                child->QueryFloatAttribute("x", &tx);
                child->QueryFloatAttribute("y", &ty);
                child->QueryFloatAttribute("z", &tz);
                Transform transform("translate", 0, tx, ty, tz, frame, totalFrames, models);
                og_group.transforms.push_back(transform);
        }
        else if (strcmp(childName, "rotate") == 0) {

                child->QueryFloatAttribute("angle", &angle);
                child->QueryFloatAttribute("x", &rx);
                child->QueryFloatAttribute("y", &ry);
                child->QueryFloatAttribute("z", &rz);
                Transform transform("rotate", angle, rx, ry, rz, frame, totalFrames, models);
                og_group.transforms.push_back(transform);
            
        }
        else if (strcmp(childName, "scale") == 0) {
            child->QueryFloatAttribute("x", &sx);
            child->QueryFloatAttribute("y", &sy);
            child->QueryFloatAttribute("z", &sz);
            Transform transform("scale", 0, sx, sy, sz, frame, totalFrames, models);
            og_group.transforms.push_back(transform);
        }
    }
}


void processGroupElement(tinyxml2::XMLElement* groupElement, Group& og_group) {

    for (tinyxml2::XMLElement* child = groupElement->FirstChildElement(); child; child = child->NextSiblingElement()) {
        const char* childName = child->Name();
 
        if (strcmp(childName, "transform") == 0) {
            
            processTransformElement(child, og_group);
        }

        if (strcmp(childName, "group") == 0) {

            Group group_bla = Group();
            
            processGroupElement(child, group_bla);
            og_group.groups.push_back(group_bla);
        }
    }

}

void processWorldElement(tinyxml2::XMLElement* worldElement) {
    for (tinyxml2::XMLElement* child = worldElement->FirstChildElement(); child; child = child->NextSiblingElement()) {
        const char* childName = child->Name();
 
        if (strcmp(childName, "group") == 0) {
            processGroupElement(child, og_group);
        }
    }
}

int parsexml(const char *filename) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Erro ao carregar o arquivo XML." << std::endl;
        return 1;
    }
 
    tinyxml2::XMLElement* worldElement = doc.FirstChildElement("world");
    if (!worldElement) {
        std::cerr << "Elemento 'world' não encontrado." << std::endl;
        return 1;
    }
 
    processWorldElement(worldElement);
 
    return 0;
}

// TODO : VER COMO TRATAR DOS FRAME, TOTALFRAMES E MODELS
void handle_groups(const Group& group) {
    
    Matrix m = Matrix();

    for (const auto& transform : group.transforms) {
        if (transform.type == "translate") {
            m.addTranslation(transform.x, transform.y, transform.z);
            matrixes.push_back(m);
        } else if (transform.type == "rotate") {
            m.addRotation(transform.x, transform.y, transform.z, transform.angle);
            matrixes.push_back(m);
        } else if (transform.type == "scale") {
            m.addScale(transform.x, transform.y, transform.z);
            matrixes.push_back(m);
        }
    }

    for (const auto& sub_group : group.groups) {
        handle_groups(sub_group);
    }

}

int main(int argc, const char * argv[]) {
    Scene scene;
    ImagePPM *img;    // Image
    Shader *shd;      // Shader
    clock_t start, end;
    double cpu_time_used;
    
    // Image resolution
    const int W= 640;
    const int H= 640;

    img = new ImagePPM(W,H);
    
    /* Scenes*/
    
    /* Single Sphere */
    //SpheresScene(scene, 1);
    /* Single Sphere and Triangles */
    //SpheresTriScene(scene);
    // Camera parameters for the simple scenes
    //const Point Eye ={0,0,0}, At={0,0,1};
    /* Cornell Box */
    //CornellBox(scene);
    //DiffuseCornellBox(scene);
    DLightChallenge(scene);
    // Camera parameters for the Cornell Box
    const Point Eye ={280,265,-500}, At={280,260,0};
    const float deFocusRad = 0*3.14f/180.f;    // to radians
    const float FocusDist = 1.;
    //const float deFocusRad = 2.5*3.14f/180.f;    // to radians
    //const float FocusDist = 800.;
    //const Point Eye ={0,325,0}, At={560,345,350};

    //SingleTriScene(scene);
    /* Camera parameters for the Cornell Box*/
    /*const Point Eye ={0,0,-5}, At={0,0,0};
    const float deFocusRad = 0.*3.14f/180.f;    // to radians
    const float FocusDist = 1.;*/

    /*DeFocusTriScene (scene);
    float const Z=5.f;
    const Point Eye ={0.,1.0,Z}, At={0.1,1.0,Z+1.f};
    const float deFocusRad = 5.*3.14f/180.f;    // to radians
    const float FocusDist = 5.;*/

    const Vector Up={0,1,0};
    const float fovH = 60.f;
    const float fovHrad = fovH*3.14f/180.f;    // to radians
    //Perspective *cam = new Perspective(Eye, At, Up, W, H, fovHrad);
    Perspective *cam = new Perspective(Eye, At, Up, W, H, fovHrad, deFocusRad, FocusDist);

    /*   Dummy */
    // create the shader
    //shd = new DummyShader(&scene, W, H);
    // declare the renderer
    //DummyRenderer myRender (cam, &scene, img, shd);

    /*   Standard */
    // create the shader
    //shd = new AmbientShader(&scene, RGB(0.1,0.1,0.8));
    //shd = new WhittedShader(&scene, RGB(0.1,0.1,0.8));
    //shd = new DistributedShader(&scene, RGB(0.1,0.1,0.8));
    memoryAllocator(scene.numLights);
    shd = new PathTracing(&scene, RGB(0.,0.,0.2));
    // declare the renderer
    int const spp=16;
    
    bool const jitter=true;
    StandardRenderer myRender (cam, &scene, img, shd, spp, jitter);
    // render
    start = clock();
    
    myRender.Render();
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    memoryDeallocator(scene.numLights);
    // save the image
    img->Save("MyImage.ppm");
    
    fprintf (stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);
    
    std::cout << "That's all, folks!" << std::endl;
    return 0;
}


