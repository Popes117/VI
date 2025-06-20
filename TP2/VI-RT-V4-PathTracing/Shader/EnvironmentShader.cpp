//
//  WhittedShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "EnvironmentShader.hpp"
#include "BRDF.hpp"
#include "ray.hpp"

#include "Shader_Utils.hpp"

RGB EnvironmentShader::specularReflection (Intersection isect, BRDF *f, int depth) {
    RGB color(0.,0.,0.);

    // generate the specular ray
    // direction R = 2 (N.V) N - V
    Vector Rdir = reflect(isect.wo, isect.sn);
    Ray specular(isect.p, Rdir, SPEC_REFL);
    
    specular.pix_x = isect.pix_x;
    specular.pix_y = isect.pix_y;
    
    specular.FaceID = isect.FaceID;

    specular.adjustOrigin(isect.gn);
    specular.propagating_eta = isect.incident_eta;  // same medium

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection s_isect;
    // trace ray
    intersected = scene->trace(specular, &s_isect);

    // shade this intersection
    color = f->Ks * shade (intersected, s_isect, depth+1, specular.dir);

    return color;
}

RGB EnvironmentShader::specularTransmission (Intersection isect, BRDF *f, int depth) {
    RGB color(0., 0., 0.);

    // generate the transmission ray
    // from https://raytracing.github.io/books/RayTracingInOneWeekend.html#dielectrics
    
    // direction T = IOR * V
    float const IOR = isect.incident_eta / isect.f->eta;
    Vector const V = -1.*isect.wo;
    Vector const N = isect.sn;
    
    auto cos_theta = std::fmin(N.dot(-1.*V), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
    
    // is there total internal reflection ?
    bool const cannot_refract = (IOR*sin_theta>1.);
    
    Vector const dir = (cannot_refract ? reflect(V,N) : refract (V, N, IOR));

    Ray refraction(isect.p, dir, (cannot_refract ? SPEC_REFL : SPEC_TRANS));
    
    refraction.pix_x = isect.pix_x;
    refraction.pix_y = isect.pix_y;
    
    refraction.FaceID = isect.FaceID;

    refraction.adjustOrigin(-1. * isect.gn);
    
    refraction.propagating_eta = (cannot_refract ? isect.incident_eta : isect.f->eta);

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection t_isect;
    // trace ray
    intersected = scene->trace(refraction, &t_isect);

    // shade this intersection
    color = f->Kt * shade (intersected, t_isect, depth+1, refraction.dir);
   
    return color;
}

RGB EnvironmentShader::shade(bool intersected, Intersection isect, int depth) {
    return shade(intersected, isect, depth, -isect.wo); // ou outra direção
}


RGB EnvironmentShader::shade(bool intersected, Intersection isect, int depth, const Vector& ray_dir) {
    RGB color(0.,0.,0.);
    
    // if no intersection, return background
    if (!intersected) {
        //Sample_L  (float* rand, Vector* dir, float& pdf)
        for (Light* l : scene->lights) {
            if (l->type == ENVIRONMENT_LIGHT) {
                EnvironmentLight* env = (EnvironmentLight*)l;
                return env->L(-ray_dir);  // ou: return env->L(-ray.dir);
            }
        }
        return background;
    }
    if (isect.isLight) { // intersection with a light source
        return isect.Le;
    }
    // get the BRDF
    BRDF *f = isect.f;
    
    #define MAX_DEPTH 3
    // if there is a specular component sample it
    if (!f->Ks.isZero()) {
        if (depth < MAX_DEPTH) {
            color += specularReflection(isect, f, depth + 1);
        } else {
            // Fallback: usar IBL diretamente, sem ray tracing
            Vector v = -isect.wo;  // direção da câmara
            Vector n = isect.sn;   // normal
            Vector r = 2.0f * n.dot(v) * n - v;
            r.normalize();

            for (Light* l : scene->lights) {
                if (l->type == ENVIRONMENT_LIGHT) {
                    EnvironmentLight* env = (EnvironmentLight*)l;
                    RGB reflectedColor = env->L(r);
                    color += f->Ks * reflectedColor;
                    break;
                }
            }
        }
    }

    // if there is a specular component sample it
    if (!f->Kt.isZero() && depth<MAX_DEPTH) {
        color += specularTransmission (isect, f, depth+1);
    }
    
    //color += directLighting(scene, isect, f, rng, U_dist, UNIFORM_ONE);
    color += directLighting(scene, isect, f, rng, U_dist, ALL_LIGHTS);

    return color;
};
