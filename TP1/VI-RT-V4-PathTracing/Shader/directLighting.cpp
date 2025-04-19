//
//  directLighting.cpp
//  VI-RT-3rdVersion
//
//  Created by Luis Paulo Santos on 05/03/2025.
//

#include "directLighting.hpp"
#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "AreaLight.hpp"

static RGB direct_AmbientLight (AmbientLight * l, BRDF  *  f);
static RGB direct_PointLight (PointLight  *  l, Scene *scene, Intersection isect, BRDF  *  f);
static RGB direct_AreaLight (AreaLight * l, Scene *scene, Intersection isect, BRDF* f, float *r);

static float *baseP;
static float **areaP;

void memoryAllocator(int numLights){
     baseP = (float*)malloc(sizeof(float) * numLights);
     areaP = (float**)malloc(sizeof(float*) * numLights); 
     for(int i = 0; i < numLights; i++){
        areaP[i] = (float*)malloc(sizeof(float)<<1);
     }
}

void memoryDeallocator(int numLights){
    free(baseP);
    for(int i = 0; i < numLights; i++){
        free(areaP[i]);
    }
    free(areaP);
}

RGB directLighting (Scene *scene, Intersection isect, BRDF *f, std::mt19937& rng, std::uniform_real_distribution<float>U_dist, DIRECT_SAMPLE_MODE mode) {
    RGB color (0.,0.,0.);
    //only works with areaLights
    if(mode==UNIFORM_ONE){
       // versão com cálculo aproximado da contribuição da luz
        int i = 0;
        float sum = 0;
        for (Light* l : scene->lights) {
            AreaLight *al = (AreaLight*)l;
            areaP[i][0] = U_dist(rng);
            areaP[i][1] = U_dist(rng);
            float pdf, Ldistance;
            RGB L;
            Point Lpos;
            L = al->Sample_L(areaP[i], &Lpos, pdf);
            // the pdf computed above is just 1/Area
            Vector Ldir=isect.p.vec2point(Lpos);
#if 1
            //Com cossenos
            Ldir.normalize();
            float cosL = Ldir.dot(isect.sn);
            float cosLN_l = -1.f * Ldir.dot(al->gem->normal);
            
            if (cosL > 0 && cosLN_l >0){
                Ldistance = Ldir.norm();
                baseP[i] = ((al->power.R + al->power.G + al->power.B) * cosL * cosLN_l * al->gem->area())/(Ldistance*Ldistance);
                sum += baseP[i];
            }else{
                baseP[i] = 0;
            }
#else
        //sem cossenos 
       Ldistance = Ldir.norm();
       baseP[i] = ((al->power.R + al->power.G + al->power.B) * al->gem->area())/(Ldistance*Ldistance);
       sum += baseP[i];
       //
#endif
       i++;
        }

        float rand = U_dist(rng);
        float accP =0;
        int k = 0;
        for(; k<i-1; k++){
            baseP[k] /= sum;
            accP += baseP[k] < 0 ? 0 : baseP[k];
            if(rand < accP) break;
        }
        color = direct_AreaLight((AreaLight*)scene->lights[k], scene, isect, f, areaP[k]) / baseP[k];
        /* Modo completamente aleatório (comentar tudo o que está para cima)
        float rand = U_dist(rng);
        float accP =0;
        float increment = 1 / scene->numLights;
        for (Light* l : scene->lights) {
            accP += increment;
            if (rand < increment){
                AreaLight *al = (AreaLight*)l;
                float u[2];
                u[0] = U_dist(rng);
                u[1] = U_dist(rng);
                color = direct_AreaLight(al, scene, isect, f, u) * scene->numLights;
                break;
            }
        }*/
    }else{
    // Loop over scene's light sources
    for (Light* l : scene->lights) {
        if (l->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            color += direct_AmbientLight ((AmbientLight *)l, f);
            continue;
        }
        if (l->type == POINT_LIGHT) {  // is it a point light ?
            color += direct_PointLight ((PointLight *)l, scene, isect, f);
            continue;
        } // is POINT_LIGHT
        if (l->type == AREA_LIGHT) {  // is it a area light ?
            float r[2];
            RGB color_temp(0.,0.,0.);
            r[0] = U_dist(rng);
            r[1] = U_dist(rng);
            color_temp = direct_AreaLight ((AreaLight *)l, scene, isect, f, r);
            color += color_temp;
        } // is AREA_LIGHT
    }  // loop over all light sources
}
    return color;
}

static RGB direct_AmbientLight (AmbientLight * l, BRDF * f) {
    RGB color (0., 0., 0.);
    if (!f->Ka.isZero()) {
        RGB Ka = f->Ka;
        color += Ka * l->L();
    }
    return (color);
}

static RGB direct_PointLight (PointLight* l, Scene *scene, Intersection isect, BRDF * f) {
    RGB color (0., 0., 0.);
    RGB Kd;
    
    if (f->textured) {
        DiffuseTexture * df = (DiffuseTexture *)f;
        Kd = df->GetKd(isect.TexCoord);
    }
    else {
        Kd = f->Kd;
    }

    if (!Kd.isZero()) {
        Point Lpos;
        RGB L = l->Sample_L(NULL, &Lpos);
        Vector Ldir=isect.p.vec2point(Lpos);
        float Ldistance = Ldir.norm();
        Ldir.normalize();
        float cosL = Ldir.dot(isect.sn);
        if (cosL>0) {
            
            Ray shadow = Ray(isect.p, Ldir, SHADOW);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;
            
            shadow.adjustOrigin(isect.gn);
            
            if (scene->visibility(shadow, Ldistance-EPSILON)) {
                color += L * Kd * cosL;
                if (Ldistance>0.f) color /= (Ldistance*Ldistance);
            }
        }
    } // Kd is zero
    
    return (color);
}

static RGB direct_AreaLight (AreaLight* l, Scene *scene, Intersection isect, BRDF* f, float *r) {
    RGB color (0., 0., 0.);
    RGB Kd;
    float pdf, cosL,  cosLN_l, Ldistance;
    RGB L;
    Point Lpos;

    
    if (f->textured) {
        DiffuseTexture * df = (DiffuseTexture *)f;
        Kd = df->GetKd(isect.TexCoord);
    }
    else {
        Kd = f->Kd;
    }

    pdf = 0.;
    if (!Kd.isZero()) {
        
        L = l->Sample_L(r, &Lpos, pdf);
        // the pdf computed above is just 1/Area
        Vector Ldir=isect.p.vec2point(Lpos);
        Ldistance = Ldir.norm();
        Ldir.normalize();
        cosL = Ldir.dot(isect.sn);
        // Ldir points into the light: * -1 to get the correct sign
        cosLN_l = -1.f * Ldir.dot(l->gem->normal);
        // The light source will only contribute if the above cosine is positive
        if (cosL>1.e-4 && cosLN_l>1.e-4) {
            
            Ray shadow = Ray(isect.p, Ldir, SHADOW);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;
            
            shadow.adjustOrigin(isect.gn);
            
            if (scene->visibility(shadow, Ldistance-EPSILON)) {
                color = L * Kd * cosL;
                if (pdf >0.) color /= pdf;
                if (Ldistance>0.f) color /= (Ldistance*Ldistance);
                color *= cosLN_l;
            }
        }
    } // Kd is zero
        
    return (color);
}
