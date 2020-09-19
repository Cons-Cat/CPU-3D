#include "camera.h"

CAMERA::CAMERA()
{
   float IdentityMatrix[4][4]{
       {1, 0, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 1}};
   viewMatrix = new MATRIX(&IdentityMatrix[0][0]);
   projMatrix = new MATRIX(&IdentityMatrix[0][0]);
}

CAMERA::~CAMERA()
{
   delete viewMatrix;
   delete projMatrix;
}

void CAMERA::SetAspectRatio(float width, float height)
{
   aspectRatio = height / width;
}

void CAMERA::SetFOV(float degrees)
{
   vFOV = degrees;
}

void CAMERA::SetViewMatrix(float *position)
{
   viewMatrix = new MATRIX(position);
}

MATRIX *CAMERA::GetViewMatrix()
{
   return viewMatrix;
}

void CAMERA::SetProjMatrix(float zNear, float zFar)
{
   float rad = vFOV * 0.01744f;
   float yScale = 1.0f / tan(rad / 2);
   float xScale = yScale * aspectRatio;
   nearPlane = zNear;
   farPlane = zFar;

   float projMat[4][4]{
       {yScale * aspectRatio, 0, 0, 0},
       {0, yScale, 0, 0},
       {0, 0, zFar / (zFar - zNear), 1},
       {0, 0, -(zFar * zNear) / (zFar - zNear), 0}};
   projMatrix->Pos(&projMat[0][0]);
}

MATRIX *CAMERA::GetProjMatrix()
{
   return projMatrix;
}

void CAMERA::Invert()
{
   VECTOR_3 tempVert{
       *(viewMatrix->GetMatrix() + 12), *(viewMatrix->GetMatrix() + 13), *(viewMatrix->GetMatrix() + 14)};
   float subMatrix[3][3]{
       {*(viewMatrix->GetMatrix() + 0), *(viewMatrix->GetMatrix() + 1), *(viewMatrix->GetMatrix() + 2)},
       {*(viewMatrix->GetMatrix() + 4), *(viewMatrix->GetMatrix() + 5), *(viewMatrix->GetMatrix() + 6)},
       {*(viewMatrix->GetMatrix() + 8), *(viewMatrix->GetMatrix() + 9), *(viewMatrix->GetMatrix() + 10)},
   };
   // Transpose the matrix.
   float transMatrix[4][4]{
       {1, 0, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 1}};
   for (int j = 0; j < 3; j++)
   {
      for (int i = 0; i < 3; i++)
      {
         transMatrix[i][j] = subMatrix[j][i];
      }
   }
   MATRIX *tempMatrix = new MATRIX(&transMatrix[0][0]);
   ShaderUtil::MultVertByMatrix(&tempVert, tempMatrix);
   // Negate the vector.
   tempVert.x = -tempVert.x;
   tempVert.y = -tempVert.y;
   tempVert.z = -tempVert.z;
   tempMatrix->SetCoord(0, tempVert.x);
   tempMatrix->SetCoord(1, tempVert.y);
   tempMatrix->SetCoord(2, tempVert.z);
   viewMatrix = tempMatrix;
}

float CAMERA::GetNearPlane()
{
   return nearPlane;
}

float CAMERA::GetFarPlane()
{
   return farPlane;
}
