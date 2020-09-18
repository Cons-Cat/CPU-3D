#include "vecs.h"
#include "RasterUtil.h"

RASTER::RASTER(const unsigned long *_rasterWidth, const unsigned long *_rasterHeight)
{
   width = *_rasterWidth;
   height = *_rasterHeight;
   area = width * height;

   // unsigned int Raster[area] = {
   //  0,
   // };
   surface = new unsigned int[area];
   zBuffer = new std::vector<PIXEL *>[area];
}

void RASTER::AddToZBuffer(unsigned int coord, PIXEL *pixel)
{
   (zBuffer + coord)->push_back(pixel);
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
      // TODO: Remove hard coded far plane Z.
      float tempZ = 10.0f;
      unsigned int col = 0x00000000;
      for (unsigned int j = 0; j < (*(zBuffer + i)).size(); j++)
      {
         PIXEL *tempPix = (*(zBuffer + i))[j];

         if (tempPix->z < tempZ)
         {
            col = tempPix->col;
            tempZ = tempPix->z;
            // TODO: Abstract memory management.
            // delete tempPix;
         }
      }
      surface[i] = col;
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
