#include "RasterUtil.h"
#include <algorithm>
#include <stdlib.h>

#pragma region Core
unsigned int RasterUtil::ToOneDimension(unsigned int _width, unsigned int _x, unsigned int _y)
{
   return (_width * _y) + _x;
}

void RasterUtil::DrawPixel(RASTER *_raster, unsigned int _x, unsigned int _y, float _z, unsigned int col)
{
   if (_x < _raster->GetWidth() && _y < _raster->GetHeight())
   {
      // _raster->AddToZBuffer(ToOneDimension(_raster->GetWidth(), _x, _y), col);
      PIXEL *newPix = new PIXEL{_z, col};
      _raster->AddToZBuffer(ToOneDimension(_raster->GetWidth(), _x, _y), newPix);
   }
}

unsigned int RasterUtil::Lerp(float A, float B, float R)
{
   return (B - A) * R + A;
}

VECTOR_2 RasterUtil::CoordToScreen(VECTOR_3 *v, RASTER *_raster)
{
   VECTOR_3 tempV{v->x / v->w, v->y / v->w, v->z / v->w, 1, v->col};
   return VECTOR_2{
       (tempV.x + 1.0f) / 2.0f * (_raster->GetWidth()),
       (1.0f - tempV.y) / 2.0f * (_raster->GetHeight()),
       tempV.z,
       v->col};
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
void RasterUtil::BresenhamAnyDir(RASTER *_raster, int x1, int y1, int x2, int y2, unsigned int col, unsigned int col2)
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
      leftward = 1;
   }
   else if (x1 < x2)
   {
      leftward = 0;
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
      if (x1 >= 0 && y1 >= 0 && x1 < _raster->GetWidth() && y1 < _raster->GetHeight())
      {
         // TODO: Proper z.
         DrawPixel(_raster, x, y, 10, col);
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

void RasterUtil::ParametricAnyDir(RASTER *_raster, int x1, int y1, int x2, int y2, unsigned int col1, unsigned int col2)
{
   if (x1 == x2)
   {
      for (int i = y1; abs(i - y2) > 0; i += (y2 > y1) ? 1 : -1)
      {
         float R = abs(i - y1) / (float)abs(y2 - y1);
         // TODO: Proper z.
         DrawPixel(_raster, x1, i, 10, RasterUtil::ColLerp(col1, col2, R));
      }
   }
   else
   {
      for (int i = x1; abs(i - x2) > 0; i += (x2 > x1) ? 1 : -1)
      {
         float R = abs(i - x1) / (float)abs(x2 - x1);
         int y = Lerp(y1, y2, R);
         // TODO: Proper z.
         DrawPixel(_raster, i, y, 10, RasterUtil::ColLerp(col1, col2, R));
      }
   }
   return;
}
#pragma endregion

#pragma region Triangles
float RasterUtil::ImplicitLine(VECTOR_2 a, VECTOR_2 b, VECTOR_2 p)
{
   return (a.y - b.y) * p.x + (b.x - a.x) * p.y + a.x * b.y - a.y * b.x;
}

// VECTOR_3 RasterUtil::FindBarycentric(VECTOR_2 a, VECTOR_2 b, VECTOR_2 c, VECTOR_2 p)
// {
// }

// bool RasterUtil::InTriangleThree(VECTOR_3 a, VECTOR_3 b, VECTOR_3 c, VECTOR_3 p)
// {
//    VECTOR_3 v1 = FindBarycentric(a, b, c, p);
//    return v1.x <= 1 && v1.y <= 1 && v1.z <= 1 && v1.x >= 0 && v1.y >= 0 && v1.z >= 0;
// }

bool RasterUtil::InTriangleTwo(VECTOR_2 a, VECTOR_2 b, VECTOR_2 c, VECTOR_2 p)
{
   // Find barycentric coordinates.
   float constraintA = ImplicitLine(b, c, a);
   float constraintB = ImplicitLine(a, c, b);
   float constraintC = ImplicitLine(a, b, c);

   float subA = ImplicitLine(b, c, p);
   float subB = ImplicitLine(a, c, p);
   float subC = ImplicitLine(a, b, p);

   float barycentricA = subA / constraintA;
   float barycentricB = subB / constraintB;
   float barycentricC = subC / constraintC;

   return barycentricA <= 1 && barycentricB <= 1 && barycentricC <= 1 && barycentricA >= 0 && barycentricB >= 0 && barycentricC >= 0;
}

void RasterUtil::DrawTriangle(VECTOR_2 a, VECTOR_2 b, VECTOR_2 c, RASTER *_raster, unsigned int col)
{
   unsigned int minX = std::min(a.x, std::min(b.x, c.x));
   unsigned int maxX = std::max(a.x, std::max(b.x, c.x));
   unsigned int minY = std::min(a.y, std::min(b.y, c.y));
   unsigned int maxY = std::max(a.y, std::max(b.y, c.y));

   for (int i = minX; i <= maxX; i++)
   {
      for (int j = minY; j <= maxY; j++)
      {
         VECTOR_2 IJ{
             i, j};
         if (InTriangleTwo(a, b, c, IJ))
         {
            // TODO: Dry this up.
            float constraintA = ImplicitLine(b, c, a);
            float constraintB = ImplicitLine(a, c, b);
            float constraintC = ImplicitLine(a, b, c);

            float subA = ImplicitLine(b, c, IJ);
            float subB = ImplicitLine(a, c, IJ);
            float subC = ImplicitLine(a, b, IJ);

            float barycentricA = subA / constraintA;
            float barycentricB = subB / constraintB;
            float barycentricC = subC / constraintC;

            // float z = Lerp(a.z, b.z, );
            float zA = barycentricA * a.z;
            float zB = barycentricB * b.z;
            float zC = barycentricC * c.z;
            float zAvg = (barycentricA + barycentricB + barycentricC) / 3.0f;

            // v->col |= ((unsigned int)(ImplicitLine(*b, *c, *v) / ImplicitLine(*b, *c, *a) * (float)(255)) << 16);
            // v->col |= ((unsigned int)(ImplicitLine(*a, *c, *v) / ImplicitLine(*a, *c, *b) * (float)(255)) << 8);
            // v->col |= ((unsigned int)(ImplicitLine(*a, *b, *v) / ImplicitLine(*a, *b, *c) * (float)(255)) << 0);

            DrawPixel(_raster, i, j, zAvg, col);
         }
      }
   }
}
#pragma endregion
