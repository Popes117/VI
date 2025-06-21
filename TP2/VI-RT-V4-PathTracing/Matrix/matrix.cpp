#include <cmath>
#include <iostream>
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884L
#endif
#include "matrix.hpp"
#include <vector>
using namespace std;

Matrix::Matrix() {
  // Initialize identity matrix
  this->data = generateIdentityMatrix();
}

Matrix::Matrix(float (*values)[4]) {
  this->data = new float[4][4];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      this->data[i][j] = values[i][j];
    }
  }
}

Matrix::Matrix(int index){
  // Cria matriz com indíce de ordem entre as outras matrizes
  this->index = index;
  this->data = generateIdentityMatrix();
}

void Matrix::print() {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      cout << data[i][j] << " ";
    }
    cout << endl;
  }
}

float (*Matrix::generateIdentityMatrix())[4] {
  float(*idMatrix)[4] = new float[4][4];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (i == j)
        idMatrix[i][j] = 1.0;
      else
        idMatrix[i][j] = 0.0;
    }
  }
  return idMatrix;
}

void Matrix::addRotation(float x, float y, float z, float angle, int frame, int totalFrames, std::vector<int> models_indexes) {
  float angleRad = angle * M_PI / 180.0f;
  const float c = cos(angleRad);
  const float s = sin(angleRad);
  const float t = 1 - c;
  const float xSquared = x * x;
  const float ySquared = y * y;
  const float zSquared = z * z;
  const float yz = y * z;
  const float xy = x * y;
  const float xz = x * z;
  if (xSquared + ySquared + zSquared == 0) {
    cerr << "Invalid axis: Length is zero." << endl;
    return;
  }
  float(*result)[4] = generateIdentityMatrix();

  result[0][0] = xSquared + (1 - xSquared) * c;
  result[0][1] = xy * t - z * s;
  result[0][2] = xz * t + y * s;
  result[1][0] = xy * t + z * s;
  result[1][1] = ySquared + (1 - ySquared) * c;
  result[1][2] = yz * t - x * s;
  result[2][0] = xz * t - y * s;
  result[2][1] = yz * t + x * s;
  result[2][2] = zSquared + (1 - zSquared) * c;

  combineMatrices(result);
  this->frame = frame;
  this->totalFrames = totalFrames;
  this->models_indexes = models_indexes;
  delete[] result;
}

void Matrix::addScale(float x, float y, float z, int frame, int totalFrames, std::vector<int> models_indexes) {
  float(*scaleMatrix)[4] = generateIdentityMatrix();
  scaleMatrix[0][0] = x;
  scaleMatrix[1][1] = y;
  scaleMatrix[2][2] = z;
  combineMatrices(scaleMatrix);
  this->frame = frame;
  this->totalFrames = totalFrames;
  this->models_indexes = models_indexes;
  delete[] scaleMatrix;
}

void Matrix::addTranslation(float x, float y, float z, int frame, int totalFrames, std::vector<int> models_indexes) {
  float(*scaleMatrix)[4] = generateIdentityMatrix();
  scaleMatrix[0][3] = x;
  scaleMatrix[1][3] = y;
  scaleMatrix[2][3] = z;
  combineMatrices(scaleMatrix);
  this->frame = frame;
  this->totalFrames = totalFrames;
  this->models_indexes = models_indexes;
  delete[] scaleMatrix;
}

// modifies the original
void Matrix::transformPoint(float *vector, int isPoint) {
  float(*result) = new float[4];
  result[0] = vector[0];
  result[1] = vector[1];
  result[2] = vector[2];
  result[3] = isPoint;
  for (int i = 0; i < 3; i++) {
    vector[i] = 0;
    for (int j = 0; j < 4; j++) {
      vector[i] += data[i][j] * result[j];
    }
  }

  delete[] result;
}

void Matrix::combineMatrices(float values[4][4]) {
  float(*result)[4] = new float[4][4];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result[i][j] = 0;
      for (int k = 0; k < 4; k++) {
        result[i][j] += this->data[i][k] * values[k][j];
      }
    }
  }
  delete[] this->data;
  this->data = result;
}

void Matrix::deleteMatrix() {
  delete[] this->data;
  delete this;
}
