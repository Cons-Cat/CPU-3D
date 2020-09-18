#include "vecs.h"

RASTER::RASTER::RASTER(const unsigned long *_rasterWidth, const unsigned long *_rasterHeight)
{
   width = *_rasterWidth;
   height = *_rasterHeight;
   area = width * height;

   // unsigned int Raster[area] = {
   //  0,
   // };
   surface = new unsigned int[area];
}

void RASTER::RASTER::AddToZBuffer(unsigned int coord, unsigned int color)
{
}

void RASTER::RASTER::ClearZBuffer()
{
   // delete zBuffer;
}

void RASTER::RASTER::ClearRaster(unsigned int col)
{
   {
      for (unsigned int i = 0; i < area; i++)
      {
         *(surface + i) = col;
      }
   }
}

unsigned int *RASTER::RASTER::GetSurface()
{
   return surface;
}

unsigned int RASTER::RASTER::GetWidth()
{
   return width;
}

unsigned int RASTER::RASTER::GetHeight()
{
   return height;
}

unsigned int RASTER::RASTER::GetArea()
{
   return area;
}
