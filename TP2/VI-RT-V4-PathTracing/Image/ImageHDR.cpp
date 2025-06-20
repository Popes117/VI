#include "ImageHDR.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm> 

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

ImageHDR::ImageHDR(const std::string& filename) {
    Load(filename);
}

ImageHDR::~ImageHDR() {
    if (imagePlane) {
        delete[] imagePlane;
    }
}

bool ImageHDR::Load(const std::string& filename) {
    int w, h, n;

    float* data = stbi_loadf(filename.c_str(), &w, &h, &n, 3);  // força 3 canais RGB

    if (!data) {
        std::cerr << "Can't read HDR file: " << filename << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }

    W = w;
    H = h;
    imagePlane = new RGB[W * H];

    for (int i = 0; i < W * H; ++i) {
        imagePlane[i].R = data[3 * i + 0];
        imagePlane[i].G = data[3 * i + 1];
        imagePlane[i].B = data[3 * i + 2];
    }

    stbi_image_free(data);
    return true;
}


/*
Thus, if we consider the images to be normalized to have coordinates u=[-1,1], v=[-1,1], we have theta=atan2(v,u), phi=pi*sqrt(u*u+v*v). 

The unit vector pointing in the corresponding direction is obtained by rotating (0,0,-1) by phi degrees around the y (up) axis and then theta degrees around the -z (forward) axis.

If for a direction vector in the world (Dx, Dy, Dz), the corresponding (u,v) coordinate in the light probe image is (Dx*r,Dy*r) where r=(1/pi)*acos(Dz)/sqrt(Dx^2 + Dy^2).
*/

RGB ImageHDR::SampleDirection(const Vector& dir) const {
    Vector D = dir;
    D.normalize();

    float Dx = D.X;
    float Dy = D.Y;
    float Dz = D.Z;

    float d = std::sqrt(Dx * Dx + Dy * Dy);

    float r = 0.0f;
    if (d > 1e-6f) {  // EPSILON para evitar divisão por zero
        r = (1.0f / (2.0f * M_PI)) * std::acos(Dz) / d;
    }

    float u = 0.5f + Dx * r;
    float v = 0.5f + Dy * r;

    // Se estiver fora da área visível do mapa, retorna preto
    if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f) {
        return RGB(0.0f, 0.0f, 0.0f);
    }

    // Convertendo para coordenadas da imagem em pixels
    float fx = u * (W - 1);
    float fy = v * (H - 1);

    // Clamp para evitar leitura fora do buffer
    fx = std::min(std::max(fx, 0.0f), (float)(W - 1));
    fy = std::min(std::max(fy, 0.0f), (float)(H - 1));

    int x0 = (int)fx;
    int y0 = (int)fy;
    int x1 = std::min(x0 + 1, W - 1);
    int y1 = std::min(y0 + 1, H - 1);

    float dx = fx - x0;
    float dy = fy - y0;

    auto get_pixel = [&](int x, int y) -> RGB {
        return imagePlane[y * W + x];
    };

    // Interpolação bilinear
    RGB c00 = get_pixel(x0, y0);
    RGB c10 = get_pixel(x1, y0);
    RGB c01 = get_pixel(x0, y1);
    RGB c11 = get_pixel(x1, y1);

    RGB c0 = c00 * (1.0f - dx) + c10 * dx;
    RGB c1 = c01 * (1.0f - dx) + c11 * dx;
    RGB c = c0 * (1.0f - dy) + c1 * dy;

    return c;
}
