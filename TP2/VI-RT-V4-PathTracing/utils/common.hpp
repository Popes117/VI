#include <vector>
#include <cmath>
#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>
#include <string>

struct Coordenadas{
	double x, y, z;

    Coordenadas(float x, float y, float z) : x(x), y(y), z(z) {
    }
    Coordenadas(){

    }
    void addCoords(Coordenadas w){
        x += w.x;
        y += w.y;
        z += w.z;
    }
    void multByFloat(float w){
        x *= w;
        y *= w;
        z *= w;
    }
    void normalize(){
        float vLen = sqrtf(x*x+y*y+z*z);
        x = x / vLen;
        y = y / vLen;
        z = z / vLen;
    };
    Coordenadas get_cross_product(Coordenadas& v){
        this->normalize();
        v.normalize();
        return Coordenadas((this->y*v.z) - (this->z*v.y), (this->z*v.x) - (this->x*v.z), (this->x*v.y) - (this->y*v.x));
    };	
};

struct Transform {
    std::string type;
    float angle, x, y, z;
    int frame, totalFrames;
    std::vector<int> models_indexes;

    Transform(std::string type, float angle, float x, float y, float z, int frame, int totalFrames, std::vector<int> models_indexes) :
    type(type), angle(angle), x(x), y(y), z(z), frame(frame), totalFrames(totalFrames), models_indexes(models_indexes) {}

};

#endif

class Group{
	
	public:

	std::vector<Transform> transforms;
	std::vector<Group> groups;

	Group():transforms(), groups(){}

};