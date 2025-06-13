#ifndef ImageHDR_hpp
#define ImageHDR_hpp
#include "image.hpp"
#include "vector.hpp"


class ImageHDR : public Image {
public:
    ImageHDR(): Image() {}
    ImageHDR(const std::string& filename);
    ~ImageHDR();

    bool Load(const std::string& filename);

    // Sample a direction on the unit sphere and return the color from imagePlane
    RGB SampleDirection(const Vector& dir) const;
};

#endif // ImageHDR_hpp