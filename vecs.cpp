#include "vecs.h"

RASTER::RASTER(const unsigned long *_rasterWidth, const unsigned long *_rasterHeight)
{
   width = *_rasterWidth;
   height = *_rasterHeight;
   area = width * height;

   // unsigned int Raster[area] = {
   //  0,
   // };
   surface = new unsigned int[area];
}

void RASTER::AddToZBuffer(unsigned int coord, unsigned int col, float z)
{
   (zBuffer + coord)->push_back(col);
   (zBuffer + coord)->push_back(z);
   surface[coord] = col;
}

void RASTER::ClearZBuffer()
{
   // delete zBuffer;
}

void RASTER::ClearRaster(unsigned int col)
{
   {
      for (unsigned int i = 0; i < area; i++)
      {
         *(surface + i) = col;
      }
   }
}

void RASTER::EvaluateZ()
{
   for (unsigned int i = 0; i < area; i++)
   {
      // *(surface + i) = col;
   }
}

unsigned int *RASTER::GetSurface()
{
   return surface;
}

unsigned int RASTER::GetWidth()
{
   return width;
}

unsigned int RASTER::GetHeight()
{
   return height;
}

unsigned int RASTER::GetArea()
{
   return area;
}
