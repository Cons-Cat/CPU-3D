#pragma once

struct VECTOR_2
{
   // NDC coords.
   float x, y;
   unsigned int col;
};

struct VECTOR_3
{
   // NDC coords.
   float x, y, z, w;
   unsigned int col;
};

struct RASTER
{
   unsigned int *surface;
   const unsigned int rasterSize;
   const unsigned int width;
   const unsigned int height;
};
