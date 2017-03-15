#pragma once
#include <math.h> // remember to add _USE_MATH_DEFINES to the project settings
#include <memory.h>

#define DEGSTORADS(x)     ((x*M_PI)/180)

/*
  The matrix is assumed to be stored as illustrated below:
  m[0] m[4] m[8]  m[12]
  m[1] m[5] m[9]  m[13]
  m[2] m[6] m[10] m[14]
  m[3] m[7] m[11] m[15]
  */
class Matrix
{
public:
  static void SetIdentity(float* matrix);
  static void Normalise3(float* vector);
  static void Normalise4(float* vector);
  static void MultiplyMatrix(float* result, const float* lhs, const float* rhs);
  static void Translate(float* matrix, const float tx, const float ty, const float tz);
  static void Scale(float* matrix, const float sx, const float sy, const float sz);
  static void RotateX(float* matrix, const float degs);
  static void Rotate3X(float* matrix, const float degs);
  static void RotateY(float* matrix, const float degs);
  static void Rotate3Y(float* matrix, const float degs);
  static void RotateZ(float* matrix, const float degs);
  static void SetFrustum(float* matrix, float left, float right, float bottom, float top, float near, float far);
  static void SetLookAt(float* matrix, float* eye, float* centre, float* up);
  static void subtractMatrix(float* F, float* m1, float* m2);
  static void CrossProduct(float* x, float* a, float* b);
  static float DotProduct(float* a, float* b);


};

// We can inline SetIdentity because it is such a small method
// This method is done for you
inline void Matrix::SetIdentity(float* matrix) {
  memset(matrix, 0, sizeof(float)*16);
  matrix[0]=matrix[5]=matrix[10]=matrix[15]=1;
}

inline void Matrix::Translate(float* matrix, float tx, float ty, float tz) {
	matrix[12] = matrix[0] * tx + matrix[4] * ty + matrix[8] * tz + matrix[12];
	matrix[13] = matrix[1] * tx + matrix[5] * ty + matrix[9] * tz + matrix[13];
	matrix[14] = matrix[2] * tx + matrix[6] * ty + matrix[10] * tz + matrix[14];
	matrix[15] = matrix[3] * tx + matrix[7] * ty + matrix[11] * tz + matrix[15];
}

inline void Matrix::Normalise3(float* vector) {
	float mag = sqrt((vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]));
	vector[0] /= mag;
	vector[1] /= mag;
	vector[2] /= mag;
}


inline void Matrix::Normalise4(float* vector) {
	float mag = sqrt((vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]) + (vector[3] * vector[3]));
	vector[0] /= mag;
	vector[1] /= mag;
	vector[2] /= mag;
	vector[3] /= mag;
}



