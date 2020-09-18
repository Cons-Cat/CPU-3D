#include "fwd.h"

#include "RasterUtil.h"
#include "shaders.h"
#include <math.h>
#include <vector>

/*
void ShaderUtil::PS_Color(VECTOR_2* v, VECTOR_2* a, VECTOR_2* b, VECTOR_2* c)
{
   for (unsigned int i = 0; i < 3; i++)
   {
      if (v->col >> (i * 16) != 0)
      {
         v->col |= ((unsigned int)(RasterUtil::ImplicitLine(*b, *c, *v) / ImplicitLine(*b, *c, *a) * (float)(255)) << 16);
         v->col |= ((unsigned int)(RasterUtil::ImplicitLine(*a, *c, *v) / ImplicitLine(*a, *c, *b) * (float)(255)) << 8);
         v->col |= ((unsigned int)(RasterUtil::ImplicitLine(*a, *b, *v) / ImplicitLine(*a, *b, *c) * (float)(255)) << 0);
         break;
      }
   }
}
*/

void ShaderUtil::VS_Rotate(MATRIX& _matrix, unsigned int axis, float degrees)
{
   _matrix.DRotate(axis, degrees);
}

void ShaderUtil::MultVertByMatrix(VECTOR_3* vec, MATRIX* matrix)
{
   VECTOR_3 tempVec{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
   float* vals = matrix->GetMatrix();
   tempVec.x = vec->x * vals[0] + vec->y * vals[4] + vec->z * vals[8] + vec->w * vals[12];
   tempVec.y = vec->x * vals[1] + vec->y * vals[5] + vec->z * vals[9] + vec->w * vals[13];
   tempVec.z = vec->x * vals[2] + vec->y * vals[6] + vec->z * vals[10] + vec->w * vals[14];
   tempVec.w = vec->x * vals[3] + vec->y * vals[7] + vec->z * vals[11] + vec->w * vals[15];
   vec->x = tempVec.x;
   vec->y = tempVec.y;
   vec->z = tempVec.z;
   vec->w = tempVec.w;
}

void ShaderUtil::VS_ProjectEdges(MESH& _mesh, CAMERA* camera)
{
   for (unsigned int i = 0; i < _mesh.GetEdges().size(); i++)
   {
      /* ---------------- MultBy by local matrix. --------------- */
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(1), _mesh.GetLocalMatrix());
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(2), _mesh.GetLocalMatrix());

      /* ---------------- MultBy by world matrix. --------------- */
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(1), _mesh.GetWorldMatrix());
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(2), _mesh.GetWorldMatrix());

      /* ---------------- MultBy by view matrix. ---------------- */
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(1), camera->GetViewMatrix());
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(2), camera->GetViewMatrix());

      /* ------------- MultBy by projection matrix. ------------- */
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(1), camera->GetProjMatrix());
      MultVertByMatrix(_mesh.GetEdges()[i]->GetVertex(2), camera->GetProjMatrix());
   }
}

void ShaderUtil::VS_ProjectFaces(MESH& _mesh, CAMERA* camera)
{
   for (unsigned int i = 0; i < _mesh.GetTris().size(); i++)
   {
      /* ---------------- MultBy by local matrix. --------------- */
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(1), _mesh.GetLocalMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(2), _mesh.GetLocalMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(3), _mesh.GetLocalMatrix());

      /* ---------------- MultBy by world matrix. --------------- */
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(1), _mesh.GetWorldMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(2), _mesh.GetWorldMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(3), _mesh.GetWorldMatrix());

      /* ---------------- MultBy by view matrix. ---------------- */
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(1), camera->GetViewMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(2), camera->GetViewMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(3), camera->GetViewMatrix());

      /* ------------- MultBy by projection matrix. ------------- */
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(1), camera->GetProjMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(2), camera->GetProjMatrix());
      MultVertByMatrix(_mesh.GetTris()[i]->GetVertex(3), camera->GetProjMatrix());
   }
}
