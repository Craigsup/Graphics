	#include "Matrix.h"

// around 4 lines = inline
// 6 or more not inline

void Matrix::Scale(float* matrix, float sx, float sy, float sz) {
	matrix[0] = matrix[0] * sx;
	matrix[1] = matrix[1] * sx;
	matrix[2] = matrix[2] * sx;
	matrix[3] = matrix[3] * sx;
	matrix[4] = matrix[4] * sy;
	matrix[5] = matrix[5] * sy;
	matrix[6] = matrix[6] * sy;
	matrix[7] = matrix[7] * sy;
	matrix[8] = matrix[8] * sz;
	matrix[9] = matrix[9] * sz;
	matrix[10] = matrix[10] * sz;
	matrix[11] = matrix[11] * sz;
}

void Matrix::RotateX(float* matrix, const float degs) {
	float rads = DEGSTORADS(degs);
	float sin = sinf(rads);
	float cos = cosf(rads);
	float t[8];
	t[0] = matrix[4] * cos + matrix[8] * sin;
	t[1] = matrix[5] * cos + matrix[9] * sin;
	t[2] = matrix[6] * cos + matrix[10] * sin;
	t[3] = matrix[7] * cos + matrix[11] * sin;

	t[4] = matrix[4] * -sin + matrix[8] * cos;
	t[5] = matrix[5] * -sin + matrix[9] * cos;
	t[6] = matrix[6] * -sin + matrix[10] * cos;
	t[7] = matrix[7] * -sin + matrix[11] * cos;

	matrix[4] = t[0];
	matrix[5] = t[1];
	matrix[6] = t[2];
	matrix[7] = t[3];
	matrix[8] = t[4];
	matrix[9] = t[5];
	matrix[10] = t[6];
	matrix[11] = t[7];
}

void Matrix::Rotate3X(float* matrix, const float degs) {
	float rads = DEGSTORADS(degs);
	float sin = sinf(rads);
	float cos = cosf(rads);
	float t[4];
	t[0] = matrix[0];
	t[1] = cos*matrix[1] - sin * matrix[2];
	t[2] = sin * matrix[1] + cos * matrix[2];
	t[3] = 1;

	matrix[0] = t[0];
	matrix[1] = t[1];
	matrix[2] = t[2];
	matrix[3] = t[3];
}

void Matrix::Rotate3Y(float* matrix, const float degs) {
	float rads = DEGSTORADS(degs);
	float sin = sinf(rads);
	float cos = cosf(rads);
	float t[4];
	t[0] = cos*matrix[0] + sin * matrix[2];
	t[1] = matrix[1];
	t[2] = -sin * matrix[0] + cos * matrix[2];
	t[3] = 1;

	matrix[0] = t[0];
	matrix[1] = t[1];
	matrix[2] = t[2];
	matrix[3] = t[3];
}

void Matrix::RotateY(float* matrix, const float degs) {
	float rads = DEGSTORADS(degs);
	float sin = sinf(rads);
	float cos = cosf(rads);
	float t[8];
	t[0] = cos*matrix[0] + sin * matrix[2];
	t[1] = cos * matrix[4] + sin * matrix[6];
	t[2] = cos*matrix[8] + sin * matrix[10];
	t[3] = cos*matrix[12] + sin*matrix[14];

	t[4] = -sin*matrix[0] + cos * matrix[2];
	t[5] = -sin * matrix[4] + cos * matrix[6];
	t[6] = -sin*matrix[8] + cos * matrix[10];
	t[7] = -sin*matrix[12] + cos*matrix[14];

	matrix[0] = t[0];
	matrix[1] = t[1];
	matrix[2] = t[2];
	matrix[3] = t[3];
	matrix[8] = t[4];
	matrix[9] = t[5];
	matrix[10] = t[6];
	matrix[11] = t[7];
}


/*void Matrix::RotateY(float* matrix, const float degs) {
float rads = DEGSTORADS(degs);
float sin = sinf(rads);
float cos = cosf(rads);
float t[16];
if (degs > 0) {
int test = 1;
}
t[0] = cos*matrix[0] + sin * matrix[2];
t[1] = cos * matrix[4] + sin * matrix[6];
t[2] = cos*matrix[8] + sin * matrix[10];
t[3] = cos*matrix[12] + sin*matrix[14];

t[4] = -sin*matrix[0] + cos * matrix[2];
t[5] = -sin * matrix[4] + cos * matrix[6];
t[6] = -sin*matrix[8] + cos * matrix[10];
t[7] = -sin*matrix[12] + cos*matrix[14];

/*t[0] = matrix[0] * cos + matrix[2] * -sin;
t[1] = matrix[1];
t[2] = matrix[0] * sin + matrix[2] * cos;
t[3] = 0;
t[4] = matrix[4] * cos + matrix[6] * -sin;
t[5] = matrix[5];
t[6] = matrix[4] * sin + matrix[6] * cos;
t[7] = 0;
t[8] = matrix[8] * cos + matrix[10] * -sin;
t[9] = matrix[9];
t[10] = matrix[8] * sin + matrix[10] * cos;
t[11] = 0;
t[12] = matrix[12] * cos + matrix[14] * -sin;
t[13] = matrix[13];
t[14] = matrix[12] * sin + matrix[14] * cos;
t[15] = 0;

/*t[0] = matrix[0] * cos + matrix[8] * sin;
t[1] = matrix[1] * cos + matrix[9] * sin;
t[2] = matrix[2] * cos + matrix[10] * sin;
t[3] = matrix[3] * cos + matrix[11] * sin;
t[4] = matrix[4];
t[5] = matrix[5];
t[6] = matrix[6];
t[7] = matrix[7];
t[8] = matrix[0] * -sin + matrix[8] * cos;
t[9] = matrix[1] * -sin + matrix[9] * cos;
t[10] = matrix[2] * -sin + matrix[10] * cos;
t[11] = matrix[3] * -sin + matrix[11] * cos;
t[12] = 0;
t[13] = 0;
t[14] = 0;
t[15] = 0;


matrix[0] = t[0];
matrix[1] = t[1];
matrix[2] = t[2];
matrix[3] = t[3];
matrix[4] = t[4];
matrix[5] = t[5];
matrix[6] = t[6];
matrix[7] = t[7];
matrix[8] = t[8];
matrix[9] = t[9];
matrix[10] = t[10];
matrix[11] = t[11];
matrix[12] = t[12];
matrix[13] = t[13];
matrix[14] = t[14];
matrix[15] = t[15];

/*matrix[0] = t[0];
matrix[1] = t[4];
matrix[2] = t[8];
matrix[3] = t[12];
matrix[4] = t[1];
matrix[5] = t[5];
matrix[6] = t[9];
matrix[7] = t[13];
matrix[8] = t[2];
matrix[9] = t[6];
matrix[10] = t[10];
matrix[11] = t[14];
matrix[12] = t[3];
matrix[13] = t[7];
matrix[14] = t[11];
matrix[15] = t[15];
}*/

void Matrix::RotateZ(float* matrix, const float degs) {
	float rads = DEGSTORADS(degs);
	float sin = sinf(rads);
	float cos = cosf(rads);
	float t[8];
	t[0] = matrix[0] * cos + matrix[4] * sin;
	t[1] = matrix[1] * cos + matrix[5] * sin;
	t[2] = matrix[2] * cos + matrix[6] * sin;
	t[3] = matrix[3] * cos + matrix[7] * sin;

	t[4] = matrix[0] * -sin + matrix[4] * cos;
	t[5] = matrix[1] * -sin + matrix[5] * cos;
	t[6] = matrix[2] * -sin + matrix[6] * cos;
	t[7] = matrix[3] * -sin + matrix[7] * cos;

	matrix[0] = t[0];
	matrix[1] = t[1];
	matrix[2] = t[2]; 
	matrix[3] = t[3];
	matrix[4] = t[4];
	matrix[5] = t[5];
	matrix[6] = t[6];
	matrix[7] = t[7];
}

void Matrix::MultiplyMatrix(float* result, const float* lhs, const float* rhs) {
	result[0] = rhs[0] * lhs[0] + rhs[1] * lhs[4] + rhs[2] * lhs[8] + rhs[3] * lhs[12];
	result[1] = rhs[0] * lhs[1] + rhs[1] * lhs[5] + rhs[2] * lhs[9] + rhs[3] * lhs[13];
	result[2] = rhs[0] * lhs[2] + rhs[1] * lhs[6] + rhs[2] * lhs[10] + rhs[3] * lhs[14];
	result[3] = rhs[0] * lhs[3] + rhs[1] * lhs[7] + rhs[2] * lhs[11] + rhs[3] * lhs[15];

	result[4] = rhs[4] * lhs[0] + rhs[5] * lhs[4] + rhs[6] * lhs[8] + rhs[7] * lhs[12];
	result[5] = rhs[4] * lhs[1] + rhs[5] * lhs[5] + rhs[6] * lhs[9] + rhs[7] * lhs[13];
	result[6] = rhs[4] * lhs[2] + rhs[5] * lhs[6] + rhs[6] * lhs[10] + rhs[7] * lhs[14];
	result[7] = rhs[4] * lhs[3] + rhs[5] * lhs[7] + rhs[6] * lhs[11] + rhs[7] * lhs[15];


	result[8] = rhs[8] * lhs[0] + rhs[9] * lhs[4] + rhs[10] * lhs[8] + rhs[11] * lhs[12];
	result[9] = rhs[8] * lhs[1] + rhs[9] * lhs[5] + rhs[10] * lhs[9] + rhs[11] * lhs[13];
	result[10] = rhs[8] * lhs[2] + rhs[9] * lhs[6] + rhs[10] * lhs[10] + rhs[11] * lhs[14];
	result[11] = rhs[8] * lhs[3] + rhs[9] * lhs[7] + rhs[10] * lhs[11] + rhs[11] * lhs[15];

	result[12] = rhs[12] * lhs[0] + rhs[13] * lhs[4] + rhs[14] * lhs[8] + rhs[15] * lhs[12];
	result[13] = rhs[12] * lhs[1] + rhs[13] * lhs[5] + rhs[14] * lhs[9] + rhs[15] * lhs[13];
	result[14] = rhs[12] * lhs[2] + rhs[13] * lhs[6] + rhs[14] * lhs[10] + rhs[15] * lhs[14];
	result[15] = rhs[12] * lhs[3] + rhs[13] * lhs[7] + rhs[14] * lhs[11] + rhs[15] * lhs[15];
}

void Matrix::SetFrustum(float* matrix, float left, float right, float bottom, float top, float near, float far) {
	matrix[0] = (2 * near) / (right - left);
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 0;
	matrix[5] = (2 * near) / (top - bottom);
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = (right+left)/(right-left);
	matrix[9] = (top+bottom)/(top-bottom);
	matrix[10] = -((far + near) / (far - near));
	matrix[11] = -1;
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = -((2 * far*near) / (far - near));
	matrix[15] = 0;
}

void Matrix::SetLookAt(float* matrix, float* eye, float* centre, float* up) {
	float F[3] = { 0, 0, 0 };
	subtractMatrix(F, centre, eye);
	Normalise3(F);

	float T[3] = { up[0], up[1], up[2]};
	Normalise3(T);

	float S[3] = { 0,0,0 };
	CrossProduct(S, F, T);
	Normalise3(S);

	float U[3] = { 0,0,0 };
	CrossProduct(U, S, F);
	Normalise3(U);

	matrix[0] = S[0];
	matrix[1] = U[0];
	matrix[2] = -F[0];
	matrix[3] = 0;
	matrix[4] = S[1];
	matrix[5] = U[1];
	matrix[6] = -F[1];
	matrix[7] = 0;
	matrix[8] = S[2];
	matrix[9] = U[2];
	matrix[10] = -F[2];
	matrix[11] = 0;
	matrix[12] = -(DotProduct(S, eye));
	matrix[13] = -(DotProduct(U, eye));
	matrix[14] = DotProduct(F, eye);
	matrix[15] = 1;
}

float Matrix::DotProduct(float* a, float* b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void Matrix::CrossProduct(float* x, float* a, float* b) {
	x[0] = (a[1] * b[2]) - (a[2] * b[1]);
	x[1] = (a[2] * b[0]) - (a[0] * b[2]);
	x[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

void Matrix::subtractMatrix(float* F, float* m1, float* m2) {
	F[0] = m1[0] - m2[0];
	F[1] = m1[1] - m2[1];
	F[2] = m1[2] - m2[2];
}
