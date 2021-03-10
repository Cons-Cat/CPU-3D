#pragma once
#include <vector>

#include "camera.h"
#include "fwd.h"

struct VECTOR_2 {
  float x, y, z, u, v;
  unsigned int col;
};

struct VECTOR_3 {
  float x, y, z, w, u, v;
  unsigned int col;
};

struct PIXEL {
  float z;
  unsigned int col;
};

class RASTER {
 public:
  RASTER(const unsigned long *_rasterWidth, const unsigned long *_rasterHeight);
  void AddToZBuffer(unsigned int coord, PIXEL *pixel);
  void ClearRaster(unsigned int col);
  void EvaluateZ(CAMERA *camera);
  void CleanZ();
  unsigned int *GetSurface();
  unsigned int GetWidth();
  unsigned int GetHeight();
  unsigned int GetArea();

 private:
  unsigned int *surface;
  std::vector<PIXEL *> *zBuffer;
  unsigned long width;
  unsigned long height;
  unsigned long area;
};
