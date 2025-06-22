#pragma once
#include <iostream>
#include <vector>

class Matrix {
   public:
    Matrix();
    Matrix(float(*values)[4]);
    void print();
    void addRotation(float x, float y, float z, float angle, int frame, int totalFrames, std::vector<int> models_indexes);
    void addScale(float x, float y, float z, int frame, int totalFrames, std::vector<int> models_indexes);
    void addTranslation(float x, float y, float z, int frame, int totalFrames, std::vector<int> models_indexes);
    void transformPoint(float* vector, int isPoint = 1);
    void deleteMatrix();
    float(*data)[4];
    int frame;
    int totalFrames;
    std::vector<int> models_indexes;



private:
    float(*generateIdentityMatrix())[4];
    void combineMatrices(float values[4][4]);
};

inline std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    // Verificar se a matriz está inicializada
    if (!m.data) {
        os << "[Matriz não inicializada]\n";
        return os;
    }

    // Imprimir a matriz 4x4
    os << "Matriz 4x4:\n";
    for (int i = 0; i < 4; ++i) {
        os << "[ ";
        for (int j = 0; j < 4; ++j) {
            os << m.data[i][j];
            if (j < 3) os << ", ";
        }
        os << " ]\n";
    }

    // Imprimir frame e totalFrames
    os << "Frame atual: " << m.frame << "\n";
    os << "Total de frames: " << m.totalFrames << "\n";

    // Imprimir lista de modelos
    os << "Modelos associados: ";
    if (m.models_indexes.empty()) {
        os << "nenhum";
    } else {
        os << "[ ";
        for (size_t i = 0; i < m.models_indexes.size(); ++i) {
            os << m.models_indexes[i];
            if (i < m.models_indexes.size() - 1) os << ", ";
        }
        os << " ]";
    }
    os << "\n";

    return os;
}