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

void RASTER::EvaluateZ(CAMERA *camera)
{
   for (unsigned int i = 0; i < area; i++)
   {
      // TODO: Remove hard coded far plane Z.
      float tempZ = camera->GetFarPlane();
      unsigned int col = 0x00000000;
      //for (unsigned int j = 0; j < (*(zBuffer + i)).size(); j++)
      //{
      //   PIXEL* tempPix = (*(zBuffer + i))[j];

      //   if (tempPix->z < tempZ)
      //   {
      //      col = tempPix->col;
      //      tempZ = tempPix->z;
      //      // TODO: Abstract memory management.
      //      // delete tempPix;
      //   }
      //}

      // if ((*(zBuffer + i)).size() > 0)
      // {
      for (PIXEL *p : (*(zBuffer + i)))
      {
         if (p->z < tempZ)
         {
            col = p->col;
            tempZ = p->z;
         }
      }
      surface[i] = col;
      // }
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
