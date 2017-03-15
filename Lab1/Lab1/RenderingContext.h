#pragma once
#include "Matrix.h"
#include <assert.h>

#define MAX_DEPTH   (32*16)

class RenderingContext
{
public:
	RenderingContext();
	
	// call this every time I render a new frame. OPTERMISE THIS 
	void InitModelMatrix(bool resetdepth);
	void PushModelMatrix();
	void PopModelMatrix();
	
	void UpdateMVPs();
	
	// These all assume the transformation happens to the top-most matrix in the stack (model view)
	void Translate(const float x, const float y, const float z);
	void Scale(const float x, const float y, const float z);
	void RotateX(const float degs);
	void RotateY(const float degs);
	void RotateZ(const float degs);
	
	int mvhandle, mvphandle;

	int lighthandles[5];  // direction; halfplane; ambient; diffuse; specular
	int mathandles[4];    // ambient, diffuse, specular, shininess
	int verthandles[3];   // position, normal, uv

	// set up once
	// rare to do push/pop
	float viewmatrix[16], projectionmatrix[16];
  
	float mvmatrix[16], mvpmatrix[16];

private:
	// how far into the matrix I am (index 0 = first matrix, index 16 = second matrix
	int depth;

	//model matrix = where you build the world to
	float modelmatrix[MAX_DEPTH];
};

// This constructor is done for you
inline RenderingContext::RenderingContext() {
	depth=0;
	Matrix::SetIdentity(viewmatrix);
	Matrix::SetIdentity(projectionmatrix);
}  

inline void RenderingContext::InitModelMatrix(bool resetdepth) {
	if (resetdepth) {
		depth=0;
	}

	Matrix::SetIdentity(modelmatrix + depth);
}

// This method is done for you
inline void RenderingContext::UpdateMVPs() {
	Matrix::MultiplyMatrix(mvmatrix, viewmatrix, modelmatrix+depth);
	Matrix::MultiplyMatrix(mvpmatrix, projectionmatrix, mvmatrix);
}

inline void RenderingContext::PopModelMatrix() {
	// assert does not get called in release mode
	assert(depth > 0);
	depth -= 16;
}

inline void RenderingContext::Translate(const float x, const float y, const float z) {
	Matrix::Translate(modelmatrix + depth, x, y, z);
}

inline void RenderingContext::Scale(const float x, const float y, const float z) {
	Matrix::Scale(modelmatrix + depth, x, y, z);
}

inline void RenderingContext::RotateX(const float degs) {
	Matrix::RotateX(modelmatrix + depth, degs);
}

inline void RenderingContext::RotateY(const float degs) {
	Matrix::RotateY(modelmatrix + depth, degs);
}

inline void RenderingContext::RotateZ(const float degs) {
	Matrix::RotateZ(modelmatrix + depth, degs);
}

inline void RenderingContext::PushModelMatrix() {
	memcpy(modelmatrix+depth+16, modelmatrix+depth, sizeof(float) * 16);
	depth += 16;
}
