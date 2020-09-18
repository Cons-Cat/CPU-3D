#include "geometry.h"
#include "RasterUtil.h"
#include "shaders.h"
#include "vecs.h"
#include <cmath>
#include <new>
#include <vector>

#pragma region Edge
EDGE::EDGE(EDGE &_edge)
{
   v1 = new VECTOR_3();
   v2 = new VECTOR_3();
   *v1 = *(_edge.v1);
   *v2 = *(_edge.v2);
}

EDGE::EDGE(VECTOR_3 *_v1, VECTOR_3 *_v2)
{
   v1 = _v1;
   v2 = _v2;
}

EDGE::~EDGE()
{
   delete v1;
   delete v2;
}

void EDGE::Render(RASTER *raster)
{
   VECTOR_2 *t1 = &RasterUtil::CoordToScreen(v1, raster);
   VECTOR_2 *t2 = &RasterUtil::CoordToScreen(v2, raster);
   // RasterUtil::ParametricAnyDir(raster, t1->x, t1->y, t2->x, t2->y, t1->col, t2->col);
   RasterUtil::BresenhamAnyDir(raster, t1->x, t1->y, t2->x, t2->y, t1->col, t2->col);
   // RasterUtil::ParametricAnyDir(raster, t1->x, t1->y, t2->x, t2->y, t1->col, t2->col);
}

VECTOR_3 *EDGE::GetVertex(unsigned int vert)
{
   return vert == 1 ? v1 : v2;
}
#pragma endregion

#pragma region Matrix
MATRIX::MATRIX(float *position)
{
   matrix = new float[16];
   Pos(position);
}

MATRIX::~MATRIX()
{
   delete matrix;
}

float *MATRIX::MultBy(float *position)
{
   float *arr = new float[16];
   for (unsigned int i = 0; i < 16; i++)
   {
      *(arr + i) = 0;
   }
   for (unsigned int i = 0; i < 4; i++)
   {
      for (unsigned int j = 0; j < 4; j++)
      {
         for (unsigned int k = 0; k < 4; k++)
         {
            *(arr + i + j * 4) += *(position + i + k * 4) * *(matrix + k + j * 4);
         }
      }
   }
   return &arr[0];
}

void MATRIX::Pos(float *position)
{
   for (unsigned int i = 0; i < 16; i++)
   {
      *(matrix + i) = *(position + i);
   }
}

float *MATRIX::GetMatrix()
{
   return matrix;
}

void MATRIX::RotateX(MATRIX *_matrix, float rads)
{
   float arr[4][4] = {
       {1, 0, 0, 0},
       {0, cos(rads), -sin(rads), 0},
       {0, sin(rads), cos(rads), 0},
       {0, 0, 0, 1} };
   float *temp = _matrix->MultBy(&arr[0][0]);
   _matrix->Pos(temp);
}

void MATRIX::RotateY(MATRIX *_matrix, float rads)
{
   float arr[4][4] = {
       {cos(rads), 0, sin(rads), 0},
       {0, 1, 0, 0},
       {-sin(rads), 0, cos(rads), 0},
       {0, 0, 0, 1} };
   float *temp = _matrix->MultBy(&arr[0][0]);
   _matrix->Pos(temp);
}

void MATRIX::RotateZ(MATRIX *_matrix, float rads)
{
   float arr[4][4] = {
       {cos(rads), -sin(rads), 0, 0},
       {sin(rads), cos(rads), 0, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 1} };
   float *temp = _matrix->MultBy(&arr[0][0]);
   _matrix->Pos(temp);
}

void MATRIX::DRotate(unsigned int axis, float degrees)
{
   (*rotMatrices[axis])(this, degrees * 0.01744f);
}

void MATRIX::Translate(float xOff, float yOff, float zOff)
{
   float arr[4][4]{
       {1, 0, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 1, 0},
       {xOff, yOff, zOff, 1} };
   Pos(MultBy(&arr[0][0]));
}

void MATRIX::Negate()
{
   for (unsigned int i = 0; i < 16; i++)
   {
      *(matrix + i) = *(matrix + i) * -1;
   }
}

void MATRIX::SetCoord(unsigned int coord, float val)
{
   *(matrix + 12 + coord) = val;
}
#pragma endregion

#pragma region Mesh
MESH::MESH(MESH &_mesh)
{
   float IdentityMatrix[4][4]{
       {1, 0, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 1} };
   localMatrix = new MATRIX(&IdentityMatrix[0][0]);
   worldMatrix = new MATRIX(&IdentityMatrix[0][0]);
   for (unsigned int i = 0; i < _mesh.edges.size(); i++)
   {
      edges.push_back(new EDGE(*(_mesh.edges[i])));
   }
   for (unsigned int i = 0; i < _mesh.tris.size(); i++)
   {
      tris.push_back(new TRI(*(_mesh.tris[i])));
   }
   SetLocalMatrix(_mesh.GetLocalMatrix()->GetMatrix());
   SetWorldMatrix(_mesh.GetWorldMatrix()->GetMatrix());
}

MESH::MESH(std::vector<EDGE *> &_edges, float *position)
{
   edges = _edges;
   float IdentityMatrix[4][4]{
       {1, 0, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 1} };
   localMatrix = new MATRIX(&IdentityMatrix[0][0]);
   worldMatrix = new MATRIX(position);
}

MESH::~MESH()
{
   for (int i = 0; i < edges.size(); i++)
   {
      delete edges[i];
   }
   delete localMatrix;
   delete worldMatrix;
}

void MESH::RenderWireframe(RASTER raster)
{
   for (unsigned int i = 0; i < edges.size(); i++)
   {
      edges[i]->Render(&raster);
   }
}

void MESH::RenderFaces(RASTER raster)
{
   for (unsigned int i = 0; i < tris.size(); i++)
   {
      tris[i]->Render(&raster);
   }
}

void MESH::MultLocalMatrix(MATRIX *_matrix)
{
   localMatrix->MultBy(_matrix->GetMatrix());
}

void MESH::SetLocalMatrix(float *position)
{
   localMatrix->Pos(position);
}

void MESH::SetWorldMatrix(float *position)
{
   worldMatrix->Pos(position);
}

void MESH::MultWorldMatrix(MATRIX *_matrix)
{
   worldMatrix->MultBy(_matrix->GetMatrix());
}

MATRIX *MESH::GetWorldMatrix()
{
   return worldMatrix;
}

MATRIX *MESH::GetLocalMatrix()
{
   return localMatrix;
}

std::vector<EDGE *> MESH::GetEdges()
{
   return edges;
}

std::vector<TRI *> MESH::GetTris()
{
   return tris;
}

void MESH::TrisFromQuad(VECTOR_3 *_v1, VECTOR_3 *_v2, VECTOR_3 *_v3, VECTOR_3 *_v4, unsigned int col)
{
   // _v1 and _v4 represent non-adjacent vertices.
   TRI *tri1 = new TRI(_v1, _v2, _v3, col);
   TRI *tri2 = new TRI(_v2, _v3, _v4, col);
   tris.push_back(tri1);
   tris.push_back(tri2);
}
#pragma endregion

#pragma region Camera
CAMERA::CAMERA()
{
   float IdentityMatrix[4][4]{
       {1, 0, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 1, 0},
       {0, 0, 0, 1} };
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

   float projMat[4][4]{
       {yScale * aspectRatio, 0, 0, 0},
       {0, yScale, 0, 0},
       {0, 0, zFar / (zFar - zNear), 1},
       {0, 0, -(zFar * zNear) / (zFar - zNear), 0} };
   projMatrix->Pos(&projMat[0][0]);
}

MATRIX *CAMERA::GetProjMatrix()
{
   return projMatrix;
}

void CAMERA::Invert()
{
   VECTOR_3 tempVert{
       *(viewMatrix->GetMatrix() + 12), *(viewMatrix->GetMatrix() + 13), *(viewMatrix->GetMatrix() + 14) };
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
       {0, 0, 0, 1} };
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
#pragma endregion

#pragma region Triangle
TRI::TRI(VECTOR_3 *_v1, VECTOR_3 *_v2, VECTOR_3 *_v3, unsigned int _col)
{
   v1 = _v1;
   v2 = _v2;
   v3 = _v3;
   col = _col;
}

TRI::TRI(TRI &_tri)
{
   v1 = new VECTOR_3();
   v2 = new VECTOR_3();
   v3 = new VECTOR_3();
   *v1 = *(_tri.v1);
   *v2 = *(_tri.v2);
   *v3 = *(_tri.v3);
   col = _tri.col;
}

TRI::~TRI()
{
}

void TRI::Render(RASTER *raster)
{
   VECTOR_2 a = RasterUtil::CoordToScreen(v1, raster);
   VECTOR_2 b = RasterUtil::CoordToScreen(v2, raster);
   VECTOR_2 c = RasterUtil::CoordToScreen(v3, raster);
   RasterUtil::DrawTriangle(a, b, c, raster, col);
}

VECTOR_3 *TRI::GetVertex(unsigned int vert)
{
   if (vert == 1)
   {
      return v1;
   }
   if (vert == 2)
   {
      return v2;
   }
   if (vert == 3)
   {
      return v3;
   }
}
#pragma endregion
