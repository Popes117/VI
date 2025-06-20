#ifndef EnvironmentLight_hpp
#define EnvironmentLight_hpp

#include "light.hpp"
#include "ImageHDR.hpp"

#include <stdlib.h>
#include <math.h>

class EnvironmentLight: public Light {
public:
    ImageHDR hdrImage;

    EnvironmentLight(const std::string& filename): hdrImage(filename){ type = ENVIRONMENT_LIGHT; }

    ~EnvironmentLight () {}

    // return the Light RGB radiance from a direction
    RGB L(const Vector& dir) const { return hdrImage.SampleDirection(dir); }

    // return a point p and RGB radiance for a given probability pair prob[2]
    RGB Sample_L  (float* rand, Vector* dir, float& pdf) const {
        float z = 1.0f - 2.0f * rand[0];
        float phi = 2.0f * M_PI * rand[1];
        float r = std::sqrt(std::max(0.0f, 1.0f - z * z));

        float x = r * std::cos(phi);
        float y = r * std::sin(phi);

        *dir = Vector(x, y, z);
        dir->normalize();
        
        pdf = 1.0f / (4.0f * M_PI);  // PDF uniforme na esfera

        return hdrImage.SampleDirection(*dir);
    }
};

#endif /* EnvironmentLight_hpp */