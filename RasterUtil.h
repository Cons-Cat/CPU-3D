#pragma once
#include "fwd.h"

#include "RasterUtil.h"
#include "geometry.h"
#include "vecs.h"

class RasterUtil
{
public:
   static unsigned int ToOneDimension(unsigned int _width, unsigned int _x, unsigned int _y);
   static void DrawPixel(RASTER *_raster, unsigned int _x, unsigned int _y, float _z, unsigned int col);
   static unsigned int Lerp(float A, float B, float R);
   static unsigned int ColLerp(unsigned int col1, unsigned int col2, float R);
   static unsigned int rgbaTOargb(unsigned int colIn);
   static void ParametricAnyDir(RASTER *_raster, int x1, int y1, int x2, int y2, unsigned int col1, unsigned int col2, float z1, float z2);
   static float ImplicitLine(VECTOR_2 a, VECTOR_2 b, VECTOR_2 p);
   // static VECTOR_2 FindBarycentric(VECTOR_2 a, VECTOR_2 b, VECTOR_2 c, VECTOR_2 p);
   static bool InTriangleTwo(VECTOR_2 a, VECTOR_2 b, VECTOR_2 c, VECTOR_2 p);
   static void DrawTriangleCol(VECTOR_2 a, VECTOR_2 b, VECTOR_2 c, RASTER *_raster, unsigned int col);
   static void DrawTriangleTex(VECTOR_2 a, VECTOR_2 b, VECTOR_2 c, RASTER *_raster, const unsigned int* texture, unsigned int texWidth, unsigned int texHeight);
   static VECTOR_2 CoordToScreen(VECTOR_3 *v, RASTER *_raster);
   static void BresenhamAnyDir(RASTER *_raster, int x1, int y1, int x2, int y2, unsigned int col, unsigned int col2, float z1, float z2);
};
