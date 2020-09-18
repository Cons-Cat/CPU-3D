#pragma once
#include <vector>

struct VECTOR_2
{
   // NDC coords.
   float x, y, z;
   unsigned int col;
};

struct VECTOR_3
{
   // NDC coords.
   float x, y, z, w;
   unsigned int col;
};

struct PIXEL
{
   float z;
   unsigned int col;
};

// struct RASTER
class RASTER
{
public:
   RASTER(const unsigned long *_rasterWidth, const unsigned long *_rasterHeight);
   void AddToZBuffer(unsigned int coord, PIXEL *pixel);
   void ClearZBuffer();
   void ClearRaster(unsigned int col);
   void EvaluateZ();
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
