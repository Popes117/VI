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

// direction deve ser normalizado
RGB ImageHDR::SampleDirection(const Vector& dir) const {
    // 1. Normalizar direção
    Vector D = dir;
    D.normalize();

    float Dx = D.X;
    float Dy = D.Y;
    float Dz = D.Z;

    // 2. Evitar divisão por zero
    float r = std::sqrt(Dx * Dx + Dy * Dy);
    if (r < EPSILON) r = EPSILON;

    // 3. Calcular coordenadas normalizadas [-1, 1]
    float radius = (1.0f / M_PI) * std::acos(Dz) / r;
    float u = Dx * radius;
    float v = Dy * radius;

    // 4. Verificar se estamos fora do círculo visível
    if ((u * u + v * v) > 1.0f) {
        return RGB(0.0f, 0.0f, 0.0f); // fora da área visível
    }

    // 5. Converter para coordenadas da imagem
    float fx = (u * 0.5f + 0.5f) * W;
    float fy = (v * 0.5f + 0.5f) * H;

    // Clamp para evitar acesso fora de limites
    fx = std::min(std::max(fx, 0.0f), (float)(W - 1));
    fy = std::min(std::max(fy, 0.0f), (float)(H - 1));

    // 6. Interpolação bilinear
    int x0 = (int)fx;
    int y0 = (int)fy;
    int x1 = std::min(x0 + 1, W - 1);
    int y1 = std::min(y0 + 1, H - 1);
    float dx = fx - x0;
    float dy = fy - y0;

    auto get_pixel = [&](int x, int y) -> RGB {
        return imagePlane[y * W + x];
    };

    RGB c00 = get_pixel(x0, y0);
    RGB c10 = get_pixel(x1, y0);
    RGB c01 = get_pixel(x0, y1);
    RGB c11 = get_pixel(x1, y1);

    RGB c0 = c00 * (1.0f - dx) + c10 * dx;
    RGB c1 = c01 * (1.0f - dx) + c11 * dx;
    RGB c = c0 * (1.0f - dy) + c1 * dy;

    return c;
}

/*
RGB ImageHDR::SampleDirection(const Vector& dir) const {
    // Normalizar a direção só por segurança (podes remover se garantires isso)
    Vector d = dir;
    d.normalize();

    // Convert Vector → spherical coords
    float theta = acosf(std::clamp(d.Y, -1.f, 1.f)); // [0, pi] (latitude)
    float phi   = atan2f(d.Z, d.X);                  // [-pi, pi] (longitude)

    // Mapear para coords de textura [0,1]
    float u = (phi + M_PI) / (2.0f * M_PI);  // longitude
    float v = theta / M_PI;                  // latitude

    // Converter para coordenadas de pixel
    int i = std::min(int(u * W), W - 1);
    int j = std::min(int(v * H), H - 1);

    return imagePlane[j * W + i];
}*/
