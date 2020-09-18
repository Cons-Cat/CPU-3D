#pragma once
#include "fwd.h"

#include "geometry.h"
#include "shaders.h"

class CAMERA
{
   public:
   CAMERA();
   ~CAMERA();
   void SetAspectRatio(float width, float height);
   void SetFOV(float degrees);
   void SetViewMatrix(float* position);
   MATRIX* GetViewMatrix();
   void SetProjMatrix(float zNear, float zFar);
   MATRIX* GetProjMatrix();
   void Invert();
   float GetNearPlane();
   float GetFarPlane();

   private:
   MATRIX* viewMatrix;
   MATRIX* projMatrix;
   float aspectRatio;
   float vFOV;
   float nearPlane;
   float farPlane;
};
