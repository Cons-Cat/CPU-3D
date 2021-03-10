#pragma once
#include "RasterUtil.h"
#include "fwd.h"
#include "geometry.h"
#include "vecs.h"

class ShaderUtil {
 public:
  static void VS_Rotate(MATRIX &_matrix, unsigned int axis, float degrees);
  // static VECTOR_2 RotationMatrix(VECTOR_2 v, float rad);
  // static void PS_Color(VECTOR_2* v, VECTOR_2* a, VECTOR_2* b, VECTOR_2* c);
  static void VS_ProjectEdges(MESH &_mesh, CAMERA *camera);
  static void VS_ProjectFaces(MESH &_mesh, CAMERA *camera);
  static void MultVertByMatrix(VECTOR_3 *vec, MATRIX *matrix);
};
