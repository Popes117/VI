//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"
#include <random>
#include <omp.h>

void StandardRenderer::Render () {
    int W = 0, H = 0;  // resolução
    int x, y, s;

    std::random_device rdev{};
    std::mt19937 rng{rdev()};
    std::uniform_real_distribution<float> U_dist{0.0, 1.0};  // distribuição uniforme [0,1[

    cam->getResolution(&W, &H);
    float const sppf = 1.f / spp;

    // PARALLEL FOR
    #pragma omp parallel for private(x, s) schedule(dynamic)
    for (y = 0; y < H; y++) {
        std::mt19937 local_rng(rdev()); // rng por thread (não compartilha!)
        std::uniform_real_distribution<float> local_U_dist{0.0, 1.0};

        for (x = 0; x < W; x++) {
            RGB color(0., 0., 0.);

            for (s = 0; s < spp; s++) {
                Ray primary;
                Intersection isect;
                bool intersected;
                float jitterV[2];

                if (jitter) {
                    jitterV[0] = local_U_dist(local_rng);
                    jitterV[1] = local_U_dist(local_rng);
                    cam->GenerateRay(x, y, &primary, jitterV);
                } else {
                    cam->GenerateRay(x, y, &primary);
                }

                intersected = scene->trace(primary, &isect);

                if (EnvironmentShader* dshd = dynamic_cast<EnvironmentShader*>(shd)) {
                    color += dshd->shade(intersected, isect, 0, primary.dir);
                } else {
                    color += shd->shade(intersected, isect, 0);
                }
            }

            img->set(x, y, color * sppf);
        }

        // feedback de progresso (melhor só uma thread)
        fprintf(stderr, "%d\r", y);
        fflush(stderr);
        
    }
}
