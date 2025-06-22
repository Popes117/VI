//
//  BuildScenes.hpp
//  VI-RT-InitialVersion
//
//  Created by Luis Paulo Santos on 11/02/2025.
//

#ifndef BuildScenes_hpp
#define BuildScenes_hpp

#include "scene.hpp"
#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "AreaLight.hpp"
#include "EnvironmentLight.hpp"
#include "Sphere.hpp"
#include "triangle.hpp"
#include "BRDF.hpp"
#include "../utils/common.hpp"
#include "../Matrix/matrix.hpp"

 void SpheresScene (Scene& scene, int const N_spheres);
 void SpheresTriScene (Scene& scene);
void SingleTriScene (Scene& scene);
void DeFocusTriScene (Scene& scene);
void CornellBox (int frame, Scene& scene, std::vector<Model> &models, 
                 std::vector<Matrix> &matrixes);
void DiffuseCornellBox (Scene& scene);
void DLightChallenge (Scene& scene);
void EnvScene(int frame, Scene& scene, std::vector<Model>& models, std::vector<Matrix>& matrixes);

#endif /* BuildScenes_hpp */
