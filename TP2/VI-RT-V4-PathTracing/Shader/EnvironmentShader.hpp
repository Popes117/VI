#ifndef EnvironmentShader_hpp
#define EnvironmentShader_hpp

#include "shader.hpp"
#include "BRDF.hpp"
#include "directLighting.hpp"
#include "EnvironmentLight.hpp"
#include <random>

class EnvironmentShader: public Shader {
    RGB background;
    RGB specularReflection (Intersection isect, BRDF *f, int depth);
    RGB specularTransmission (Intersection isect, BRDF *f, int depth);
    /****************************************
     
     Our Random Number Generator (rng) */
    std::random_device rdev{};
    std::mt19937 rng{rdev()};
    std::uniform_real_distribution<float>U_dist{0.0,1.0};  // uniform distribution in[0,1[


public:
    EnvironmentShader (Scene *scene, RGB bg): background(bg), Shader(scene) {}
    RGB shade (bool intersected, Intersection isect, int depth) override;
    RGB shade (bool intersected, Intersection isect, int depth, const Vector& ray_dir);
};

#endif /* EnvironmentShader_hpp */
