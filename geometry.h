#include "vecs.h"
#include <vector>

class EDGE
{
public:
   EDGE(EDGE &_edge);
   ~EDGE();
   EDGE(VECTOR_3 *_v1, VECTOR_3 *_v2);
   void Render(RASTER *raster);
   VECTOR_3 *GetVertex(unsigned int vert);

private:
   VECTOR_3 *v1;
   VECTOR_3 *v2;
};

class MATRIX
{
public:
   MATRIX(float *position);
   ~MATRIX();
   float *MultBy(float *position);
   void Pos(float *position);
   float *GetMatrix();
   void DRotate(unsigned int axis, float degrees);
   void Translate(float xOff, float yOff, float zOff);
   void Negate();
   void SetCoord(unsigned int coord, float val);
   static void TestInvert(MATRIX *viewMatrix);

private:
   float *matrix;
   static void RotateX(MATRIX *_matrix, float rads);
   static void RotateY(MATRIX *_matrix, float rads);
   static void RotateZ(MATRIX *_matrix, float rads);
   void (*rotMatrices[3])(MATRIX *_matrix, float ang) = {MATRIX::RotateX, MATRIX::RotateY, MATRIX::RotateZ};
};

class MESH
{
public:
   MESH(MESH &_mesh);
   MESH(std::vector<EDGE *> &_edges, float *position);
   ~MESH();
   void Render(RASTER raster);
   MATRIX *GetWorldMatrix();
   void SetWorldMatrix(float *position);
   void MultWorldMatrix(MATRIX *_matrix);
   MATRIX *GetLocalMatrix();
   void SetLocalMatrix(float *position);
   void MultLocalMatrix(MATRIX *_matrix);
   std::vector<EDGE *> GetEdges();

private:
   std::vector<EDGE *> edges;
   MATRIX *localMatrix;
   MATRIX *worldMatrix;
};

class CAMERA
{
public:
   CAMERA();
   ~CAMERA();
   void SetAspectRatio(float width, float height);
   void SetFOV(float degrees);
   void SetViewMatrix(float *position);
   MATRIX *GetViewMatrix();
   void SetProjMatrix(float zNear, float zFar);
   MATRIX *GetProjMatrix();
   void Invert();

private:
   MATRIX *viewMatrix;
   MATRIX *projMatrix;
   float aspectRatio;
   float vFOV;
};