#pragma once

class Matrix {
   public:
    Matrix();
    Matrix(float(*values)[4]);
    void print();
    void addRotation(float x, float y, float z, float angle);
    void addScale(float x, float y, float z);
    void addTranslation(float x, float y, float z);
    void transformPoint(float* vector, int isPoint = 1);
    void deleteMatrix();
    float(*data)[4];



private:
    float(*generateIdentityMatrix())[4];
    void combineMatrices(float values[4][4]);
};

