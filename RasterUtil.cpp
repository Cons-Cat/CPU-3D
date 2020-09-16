#include "RasterUtil.h"
#include <stdlib.h>

#pragma region Core
void RasterUtil::ClearRaster(RASTER* _raster, unsigned int col)
{
   for (unsigned int i = 0; i < _raster->rasterSize; i++)
   {
      *(_raster->surface + i) = col;
   }
}

unsigned int RasterUtil::ToOneDimension(unsigned int _width, unsigned int _x, unsigned int _y)
{
   return (_width * _y) + _x;
}

void RasterUtil::DrawPixel(RASTER* _raster, unsigned int _x, unsigned int _y, unsigned int col)
{
   if (_x < _raster->width && _y < _raster->height)
   {
      *(_raster->surface + ToOneDimension(_raster->width, _x, _y)) = col;
   }
}

unsigned int RasterUtil::Lerp(float A, float B, float R)
{
   return (B - A) * R + A;
}

VECTOR_2 RasterUtil::CoordToScreen(VECTOR_3* v, RASTER* _raster)
{
   // float div = v->z;
   float div = 1;
   return VECTOR_2{
       (v->x / div + 1.0f) / 2.0f * (_raster->width),
       (1.0f - v->y / div) / 2.0f * (_raster->height),
       v->col };
}

unsigned int RasterUtil::ColLerp(unsigned int col1, unsigned int col2, float R)
{
   unsigned int returnCol = 0;
   returnCol |= (Lerp((col1 & 0xFF0000) >> 16, (col2 & 0xFF0000) >> 16, R) << 16);
   returnCol |= (Lerp((col1 & 0x00FF00) >> 8, (col2 & 0x00FF00) >> 8, R) << 8);
   returnCol |= Lerp(col1 & 0x0000FF, col2 & 0x0000FF, R);
   returnCol |= 0xFF000000;
   return returnCol;
}
#pragma endregion

#pragma region Lines
void RasterUtil::BresenhamAnyDir(RASTER* _raster, int x1, int y1, int x2, int y2, unsigned int col)
{
   int sx = (x1 < x2) ? 1 : -1;
   int sy = (y1 < y2) ? 1 : -1;
   int dX = abs(x2 - x1);
   int dY = -abs(y2 - y1);
   int err = dX + dY;
   int e2;
   int x = x1;
   int y = y1;

   int leftward;
   if (x1 > x2)
   {
      leftward = 0;
   }
   else if (x1 < x2)
   {
      leftward = 1;
   }
   else
   {
      leftward = 2;
   }

   int upward;
   if (y1 > y2)
   {
      upward = 1;
   }
   else if (y1 < y2)
   {
      upward = 0;
   }
   else
   {
      upward = 2;
   }

   while (true)
   {
      if (x1 >= 0 && y1 >= 0 && x1 < _raster->width && y1 < _raster->height)
      {
         DrawPixel(_raster, x, y, col);
      }

      if (((x >= x2 && leftward == 0) || (x <= x2 && leftward == 1) || (x == x2 && leftward == 2)) && ((y >= y2 && upward == 0) || (y <= y2 && upward == 1) || (y == y2 && upward == 2)))
      {
         break;
      }

      e2 = 2 * err;

      if (e2 >= dY)
      {
         err += dY;
         x += sx;
      }

      if (e2 <= dX)
      {
         err += dX;
         y += sy;
      }
   }
}

void RasterUtil::ParametricAnyDir(RASTER* _raster, int x1, int y1, int x2, int y2, unsigned int col1, unsigned int col2)
{
   if (x1 == x2)
   {
      for (int i = y1; abs(i - y2) > 0; i += (y2 > y1) ? 1 : -1)
      {
         float R = abs(i - y1) / (float)abs(y2 - y1);
         DrawPixel(_raster, x1, i, RasterUtil::ColLerp(col1, col2, R));
      }
   }
   else
   {
      for (int i = x1; abs(i - x2) > 0; i += (x2 > x1) ? 1 : -1)
      {
         float R = abs(i - x1) / (float)abs(x2 - x1);
         int y = Lerp(y1, y2, R);
         DrawPixel(_raster, i, y, RasterUtil::ColLerp(col1, col2, R));
      }
   }
   return;
}
#pragma endregion

#pragma region Triangles
float RasterUtil::ImplicitLine(VECTOR_3 a, VECTOR_3 b, VECTOR_3 p)
{
   return (a.y - b.y) * p.x + (b.x - a.x) * p.y + a.x * b.y - a.y * b.x;
}

VECTOR_3 RasterUtil::RasterUtil::FindBarycentric(VECTOR_3 a, VECTOR_3 b, VECTOR_3 c, VECTOR_3 p)
{
   float constraintA = ImplicitLine(b, c, a);
   float constraintB = ImplicitLine(a, c, b);
   float constraintC = ImplicitLine(a, b, c);

   float subA = ImplicitLine(b, c, p);
   float subB = ImplicitLine(a, c, p);
   float subC = ImplicitLine(a, b, p);

   return {
       subA / constraintA, subB / constraintB, subC / constraintC };
}

bool RasterUtil::InTriangle(VECTOR_3 a, VECTOR_3 b, VECTOR_3 c, VECTOR_3 p)
{
   VECTOR_3 v1 = FindBarycentric(a, b, c, p);
   return v1.x <= 1 && v1.y <= 1 && v1.z <= 1 && v1.x >= 0 && v1.y >= 0 && v1.z >= 0;
}

void RasterUtil::DrawTriangle(VECTOR_3 a, VECTOR_3 b, VECTOR_3 c, RASTER* _raster, float rad)
{
   // shader->VS_Rotate(&a, &b, &c, shader, rad);
   CoordToScreen(&a, _raster);
   CoordToScreen(&b, _raster);
   CoordToScreen(&c, _raster);
   for (int i = 0; i < _raster->width; i++)
   {
      for (int j = 0; j < _raster->height; j++)
      {
         VECTOR_3 IJ{
             i, j, 0xFF000000 };
         if (InTriangle(a, b, c, IJ))
         {
            // shader->PS_Color(&IJ, &a, &b, &c);
            DrawPixel(_raster, i, j, IJ.col);
         }
      }
   }
   BresenhamAnyDir(_raster, a.x, a.y, b.x, b.y, 0xFFFFFFFF);
   BresenhamAnyDir(_raster, a.x, a.y, c.x, c.y, 0xFFFFFFFF);
   BresenhamAnyDir(_raster, c.x, c.y, b.x, b.y, 0xFFFFFFFF);
}
#pragma endregion