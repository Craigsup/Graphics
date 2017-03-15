#pragma once
#include "GLSetup.h"
#include "RenderingContext.h"

class Object3D
{
public:
  Object3D();
  ~Object3D();

  void SetName(const char* name);

  void SetVertexData(byte* buffer, int noofverts, int bufferlen);
  void SetTriangles(byte* buffer, int noofpolys);

  void Draw(RenderingContext rcontext);

  void SetMaterial(byte* buffer);
  void SetDiffuse(float r, float g, float b, float a);
  const float* GetDiffuse();
  void SetTextureMap(int id);
  
  void SetTranslation(byte* buffer);
  void SetTranslation(float x, float y, float z);


private:
  char* name;
  
  float translation[3];
  
  bool incuv;
  int elementtype, elementcount;
  int noofverts;
  float* vertexdata;
  unsigned short* polygons;
  
  float glossiness, speclevel;
  float ambient[4], diffuse[4], specular[4];

  unsigned int* vbos;
  
  int texturemap;

  void InitVBOs();

};

inline void Object3D::SetTextureMap(int id)
{
  texturemap=id;
}

inline void Object3D::SetDiffuse(float r, float g, float b, float a)
{
  diffuse[0]=r;
  diffuse[1]=g;
  diffuse[2]=b;
  diffuse[3]=a;
}
  
inline const float* Object3D::GetDiffuse()
{
  return diffuse;
}

inline void Object3D::SetName(const char* name)
{
  const int len=strlen(name)+1;
  this->name=(char*) realloc(this->name, len);
  strcpy_s(this->name, len, name);

  int noofitems=0;
  int* data=(int*) malloc(sizeof(int)*noofitems);

  data=(int*) realloc(data, sizeof(int)*(noofitems+10));

  free(data);

}
