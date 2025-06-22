//
//  Scene.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include "scene.hpp"

#include "primitive.hpp"
#include "BRDF.hpp"
#include "AreaLight.hpp"

#include <iostream>
#include <set>
#include <vector>


bool Scene::trace (Ray r, Intersection *isect) {
    Intersection curr_isect;
    bool intersection = false;    
    
    /*if (r.pix_x==320 && r.pix_y==240) {
        fprintf (stderr, "This is a pixel. There are %d primitives!\n", numPrimitives);
        fflush(stderr);
    }*/
        
    curr_isect.pix_x = isect->pix_x = r.pix_x;
    curr_isect.pix_y = isect->pix_y = r.pix_y;

    if (numPrimitives==0) return false;

#if 1

    // iterate over all primitives
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() ; prim_itr++) {
       /* if (r.pix_x==320 && r.pix_y==240) {
            fprintf (stderr, "Testing intersection\n");
            fflush(stderr);
        }*/
        if ((*prim_itr)->g->intersect(r, &curr_isect)) {
            if (!intersection) { // first intersection
                intersection = true;
                *isect = curr_isect;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
            else if (curr_isect.depth < isect->depth) {
                *isect = curr_isect;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
        }
    }
    isect->isLight = false;

    // now iterate over light sources and intersect with those that have geometry
    for (auto l = lights.begin() ; l != lights.end() ; l++) {
        if ((*l)->type == AREA_LIGHT) {
            AreaLight *al = (AreaLight *)*l;
            if (al->gem->intersect(r, &curr_isect)) {
                if (!intersection) { // first intersection
                    intersection = true;
                    *isect = curr_isect;
                    isect->isLight = true;
                    //isect->Le = RGB(2.,2.,2.);
                    isect->Le = al->L();
                }
                else if (curr_isect.depth < isect->depth) {
                    *isect = curr_isect;
                    isect->isLight = true;
                    isect->Le = al->L();
                    //isect->Le = RGB(2.,2.,2.);
                }
            }
        }
    }
    isect->r_type = r.rtype;

#else 

    Intersection local_isect;
    bool local_hit = false;

    #pragma omp parallel for private(curr_isect) firstprivate(r) shared(BRDFs) schedule(dynamic)
    for (int i = 0; i < prims.size(); i++) {
        Intersection thread_isect;
        if (prims[i]->g->intersect(r, &thread_isect)) {
            #pragma omp critical
            {
                if (!intersection || thread_isect.depth < isect->depth) {
                    intersection = true;
                    *isect = thread_isect;
                    isect->f = BRDFs[prims[i]->material_ndx];
                }
            }
        }
    }

    isect->isLight = false;

    #pragma omp parallel for private(curr_isect) firstprivate(r) schedule(dynamic)
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]->type == AREA_LIGHT) {
            AreaLight* al = static_cast<AreaLight*>(lights[i]);
            Intersection thread_isect;
            if (al->gem->intersect(r, &thread_isect)) {
                //#pragma omp critical
                {
                    if (!intersection || thread_isect.depth < isect->depth) {
                        intersection = true;
                        *isect = thread_isect;
                        isect->isLight = true;
                        isect->Le = al->L();
                    }
                }
            }
        }
    }

    isect->r_type = r.rtype;

#endif

    return intersection;
}

#if 1

// checks whether a point on a light source (distance maxL) is visible
bool Scene::visibility (Ray s, const float maxL) {
    bool visible = true;
    Intersection curr_isect;
    
    if (numPrimitives==0) return true;
    
    // iterate over all primitives while visible
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() && visible ; prim_itr++) {
        if ((*prim_itr)->g->intersect(s, &curr_isect)) {
            if (curr_isect.depth < maxL) {
                visible = false;
            }
        }
    }
    return visible;
}

#else 

bool Scene::visibility(Ray s, const float maxL) {
    bool visible = true;
    Intersection curr_isect;

    if (numPrimitives == 0) return true;

    #pragma omp parallel shared(visible)
    {
        #pragma omp for
        for (int i = 0; i < prims.size(); ++i) {
            if (!visible) {
                #pragma omp cancel for
            }

            Intersection local_isect;
            if (prims[i]->g->intersect(s, &local_isect)) {
                if (local_isect.depth < maxL) {
                    #pragma omp critical
                    visible = false;

                    #pragma omp cancel for
                }
            }
        }
    }

    return visible;
}

#endif

void Scene::clear() {
    // Deleta as primitivas
    for (auto prim : prims) {
        delete prim;
    }
    prims.clear();
    numPrimitives = 0;

    // Deleta os materiais
    for (auto brdf : BRDFs) {
        delete brdf;
    }
    BRDFs.clear();
    numBRDFs = 0;

    // Deleta as luzes
    for (auto light : lights) {
        delete light;
    }
    lights.clear();
    numLights = 0;
}
