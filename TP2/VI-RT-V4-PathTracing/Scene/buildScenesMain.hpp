#ifndef buildScenesMain_hpp
#define buildScenesMain_hpp

#include "scene.hpp"
#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "AreaLight.hpp"
#include "EnvironmentLight.hpp"
#include "Sphere.hpp"
#include "triangle.hpp"
#include "BRDF.hpp"
#include "../utils/common.hpp"


void AddPlaneModel(std::vector<Model>& models,Point a, Point b, Point c, Point d, Point e, Point f);
void AddCubeModel(std::vector<Model>& models, std::vector<Point> vertices);
void AddSphereModel(std::vector<Model>& models,Point center, float radius);
void buildCornellBoxModels(std::vector<Model>& models);
void buildEnvSceneModels(std::vector<Model>& models);


#endif /* buildScenesMain_hpp */