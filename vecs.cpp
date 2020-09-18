#include "vecs.h"

RASTER::RASTER(const unsigned long *_rasterWidth, const unsigned long *_rasterHeight)
{
   width = *_rasterWidth;
   height = *_rasterHeight;
   area = width * height;
   surface = new unsigned int[area];
   zBuffer = new std::vector<PIXEL *>[area];
}

void RASTER::AddToZBuffer(unsigned int coord, PIXEL *pixel)
{
   (zBuffer + coord)->push_back(pixel);
}

void RASTER::ClearRaster(unsigned int col)
{
   for (unsigned int i = 0; i < area; i++)
   {
      *(surface + i) = col;
   }
}

void RASTER::EvaluateZ(CAMERA *camera)
{
   for (unsigned int i = 0; i < area; i++)
   {
      float tempZ = camera->GetFarPlane();
      unsigned int col = 0xFF000000;

      for (PIXEL *p : (*(zBuffer + i)))
      {
         if (p->z < tempZ)
         {
            col = p->col;
            tempZ = p->z;
         }
      }
      surface[i] = col;
   }
}

void RASTER::CleanZ()
{
   for (unsigned int i = 0; i < area; i++)
   {
      for (PIXEL *p : (*(zBuffer + i)))
      {
         delete p;
      }
      (zBuffer + i)->clear();
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
