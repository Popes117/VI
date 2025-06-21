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
#include "EnvironmentShader.hpp"
#include "PathTracingShader.hpp"
#include "directLighting.hpp"
#include "AmbientLight.hpp"
#include "Sphere.hpp"
#include "BuildScenes.hpp"
#include "buildScenesMain.hpp"
#include <time.h>
#include "../tinyxml2-master/tinyxml2.h"
#include "../utils/common.hpp"
#include "../Matrix/matrix.hpp"

void AddPlaneModel(std::vector<Model>& models,Point a, Point b, Point c, Point d, Point e, Point f) {
    Model plane({a, b, c, d, e, f});
    models.push_back(plane);
}

void AddCubeModel(std::vector<Model>& models, std::vector<Point> vertices) {
    Model cube(vertices);
    models.push_back(cube);
}

void AddSphereModel(std::vector<Model>& models,Point center, float radius) {
    Model sphere({Point(center.X, center.Y, center.Z)}, radius);
    models.push_back(sphere);
}

void buildCornellBoxModels(std::vector<Model>& models) {
    // Floor
    AddPlaneModel(models,
        Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 559.2),
        Point(549.6, 0.0, 559.2), Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 559.2)
    );

    // Ceiling
    AddPlaneModel(models,
        Point(556.0, 548.8, 0.0), Point(0.0, 548.8, 0.0), Point(0.0, 548.8, 559.2),
        Point(556.0, 548.8, 559.2), Point(556.0, 548.8, 0.0), Point(0., 548.8, 559.2)
    );

    // Back wall
    AddPlaneModel(models,
        Point(0.0, 0.0, 559.2), Point(549.6, 0.0, 559.2), Point(556.0, 548.8, 559.2),
        Point(0.0, 0.0, 559.2), Point(0.0, 548.8, 559.2), Point(556.0, 548.8, 559.2)
    );

    // Left Wall
    AddPlaneModel(models,
        Point(0.0, 0.0, 0.), Point(0., 0., 559.2), Point(0., 548.8, 559.2),
        Point(0.0, 0.0, 0.), Point(0., 548.8, 0.), Point(0., 548.8, 559.2)
    );

    // Right Wall
    AddPlaneModel(models,
        Point(552.8, 0.0, 0.), Point(549.6, 0., 559.2), Point(549.6, 548.8, 559.2),
        Point(552.8, 0.0, 0.), Point(552.8, 548.8, 0.), Point(549.6, 548.8, 559.2)
    );

    // Right Wall Mirror
    AddPlaneModel(models,
        Point(552, 50.0, 50.), Point(549, 50., 509.2), Point(549, 488.8, 509.2),
        Point(552, 50.0, 50.), Point(552, 488.8, 50.), Point(549, 488.8, 509.2)
    );

    AddCubeModel(models,
        // Short Block - Top
        {Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0),
        Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0),

        // Short Block - Bottom
        Point(130.0, 0.01,  65.0), Point( 82.0, 0.01, 225.0), Point(240.0, 0.01, 272.0),
        Point(130.0, 0.01,  65.0), Point( 290.0, 0.01, 114.0), Point(240.0, 0.01, 272.0),

        // Short Block - Left
        Point(290.0,   0.0, 114.0), Point(290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0),
        Point(290.0,   0.0, 114.0), Point(240.0,  0.0, 272.0), Point(240.0, 165.0, 272.0),

        // Short Block - Back
        Point(240.0, 0.0, 272.0), Point(240.0, 165.0, 272.0), Point(82.0, 165.0, 225.0),
        Point(240.0, 0.0, 272.0), Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0),

        // Short Block - Right
        Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), Point(130.0, 165.0, 65.0),
        Point(82.0, 0.0, 225.0), Point(130.0, 0.0, 65.0), Point(130.0, 165.0, 65.0),

        // Short Block - Front
        Point(130.0,   0.0,  65.0), Point(130.0, 165.0, 65.0), Point(290.0, 165.0, 114.0),
        Point(130.0,   0.0,  65.0), Point(290.0, 0.0, 114.0), Point(290.0, 165.0, 114.0)
        }
    );


    AddCubeModel(models,
        // Tall Block - Top
        {Point(423.0, 330.0, 247.0), Point(265.0, 330.0, 296.0), Point(314.0, 330.0, 456.0),
        Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0),

        // Tall Block - Bottom
        Point(423.0, 0.1, 247.0), Point(265.0, 0.1, 296.0), Point(314.0, 0.1, 456.0),
        Point(423.0, 0.1, 247.0), Point(472.0, 0.1, 406.0), Point(314.0, 0.1, 456.0),

        // Tall Block - Left
        Point(423.,   0., 247.), Point(423., 330., 247.), Point(472., 330., 406.),
        Point(423.,   0., 247.), Point(472.,   0., 406.), Point(472., 330., 406.),

        // Tall Block - Back
        Point(472.,   0., 406.), Point(472., 330., 406.), Point(314., 330., 456.),
        Point(472.,   0., 406.), Point(314.,   0., 456.), Point(314., 330., 456.),

        // Tall Block - Right
        Point(314.,   0., 456.), Point(314., 330., 456.), Point(265., 330., 296.),
        Point(314.,   0., 456.), Point(265.,   0., 296.), Point(265., 330., 296.),

        // Tall Block - Front
        Point(265.,   0.,296.), Point(265.,330.,296.),Point(423.,330.,247.) ,
        Point(265.,   0.,296.),Point(423, 0.,247.) ,Point(423, 330.,247.)}
    );

    // Transparent sphere
    AddSphereModel(models, Point(160., 320., 225.), 90.);
}

void buildEnvSceneModels(std::vector<Model>& models) {
    // Piso (2 triângulos que formam um retângulo)
    AddPlaneModel(models,
        Point(600, 0.0, 0.0), Point(-100.0, 0.0, 0.0), Point(-100.0, 0.0, 800.0),
        Point(600, 0.0, 800.0), Point(600, 0.0, 0.0), Point(-100.0, 0.0, 800.0)
    );

    // Esfera grande de vidro
    AddSphereModel(models, Point(278., 100., 250.), 100.);

    // Esfera vermelha à esquerda
    AddSphereModel(models, Point(90., 60., 380.), 60.);

    // Esfera reflexiva à direita
    AddSphereModel(models, Point(400., 60., 380.), 60.);

    // Esfera difusa ao fundo
    AddSphereModel(models, Point(60., 50., 100.), 50.);
}


