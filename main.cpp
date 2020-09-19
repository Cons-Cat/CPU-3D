/* ---------------------- Includes ---------------------- */
#include "fwd.h"

#include "RasterSurface.h"
#include "RasterUtil.h"
#include "camera.h"
#include "geometry.h"
#include "greendragon.h"
#include "shaders.h"
#include "vecs.h"
#include <vector>

/* ----------------------- Globals ---------------------- */
#pragma region Globals
const unsigned long rasterWidth = 400;
const unsigned long rasterHeight = 200;
RASTER *onlyRaster = new RASTER(&rasterWidth, &rasterHeight);
float ZeroMatrix[4][4]{
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}};
float IdentityMatrix[4][4]{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}};
#pragma endregion

/*************
 * MAIN BODY *
 *************/
int main()
{
   RS_Initialize(rasterWidth, rasterHeight);

   /* ------------------- Initialize Grid ------------------ */
#pragma region Grid
   VECTOR_3 gridVerts[44] = {
       VECTOR_3{0, 0, 0, 0, 0},
   };
   std::vector<EDGE *> gridEdges;
   for (int i = 0; i < 11; i++)
   {
      // Left edge.
      gridVerts[i].x = -0.5f;
      gridVerts[i].z = float(i) / 10.0f - 0.5f;
      gridVerts[i].w = 1;
      gridVerts[i].col = 0xFFFFFFFF;
      // Right edge.
      gridVerts[i + 22].x = 0.5f;
      gridVerts[i + 22].z = float(i) / 10.0f - 0.5f;
      gridVerts[i + 22].w = 1;
      gridEdges.push_back(new EDGE(&gridVerts[i], &gridVerts[i + 22]));
      gridVerts[i + 22].col = 0xFFFFFFFF;
      // Top edge.
      gridVerts[i + 11].x = float(i) / 10.0f - 0.5f;
      gridVerts[i + 11].z = 0.5f;
      gridVerts[i + 11].w = 1;
      gridVerts[i + 11].col = 0xFFFFFFFF;
      // Bottom edge.
      gridVerts[i + 33].x = float(i) / 10.0f - 0.5f;
      gridVerts[i + 33].z = -0.5f;
      gridVerts[i + 33].w = 1;
      gridVerts[i + 33].col = 0xFFFFFFFF;
      gridEdges.push_back(new EDGE(&gridVerts[i + 11], &gridVerts[i + 33]));
   }
   MESH *gridMesh = new MESH(gridEdges, &IdentityMatrix[0][0]);
   gridMesh->SetWorldMatrix(&IdentityMatrix[0][0]);
#pragma endregion;

/* ------------------- Initialize Cube ------------------ */
#pragma region Cube
   VECTOR_3 cubeVerts[8];
   std::vector<EDGE *> cubeEdges;
   unsigned int red = 0xFFFF0000;
   unsigned int green = 0xFF00FF00;
   unsigned int blue = 0xFF0000FF;
   unsigned int yellow = 0xFFFFFF00;
   // Bottom verts.
   cubeVerts[0] = VECTOR_3{-0.25, -0.25, -0.25, 1, green};
   cubeVerts[1] = VECTOR_3{0.25, -0.25, -0.25, 1, green};
   cubeVerts[2] = VECTOR_3{-0.25, -0.25, 0.25, 1, green};
   cubeVerts[3] = VECTOR_3{0.25, -0.25, 0.25, 1, green};
   cubeEdges.push_back(new EDGE(&cubeVerts[0], &cubeVerts[1]));
   cubeEdges.push_back(new EDGE(&cubeVerts[0], &cubeVerts[2]));
   cubeEdges.push_back(new EDGE(&cubeVerts[3], &cubeVerts[1]));
   cubeEdges.push_back(new EDGE(&cubeVerts[3], &cubeVerts[2]));
   // Top verts.
   cubeVerts[4] = VECTOR_3{-0.25, 0.25, -0.25, 1, green};
   cubeVerts[5] = VECTOR_3{0.25, 0.25, -0.25, 1, green};
   cubeVerts[6] = VECTOR_3{-0.25, 0.25, 0.25, 1, green};
   cubeVerts[7] = VECTOR_3{0.25, 0.25, 0.25, 1, green};
   cubeEdges.push_back(new EDGE(&cubeVerts[4], &cubeVerts[5]));
   cubeEdges.push_back(new EDGE(&cubeVerts[4], &cubeVerts[6]));
   cubeEdges.push_back(new EDGE(&cubeVerts[7], &cubeVerts[5]));
   cubeEdges.push_back(new EDGE(&cubeVerts[7], &cubeVerts[6]));
   // Side edges.
   cubeEdges.push_back(new EDGE(&cubeVerts[0], &cubeVerts[4]));
   cubeEdges.push_back(new EDGE(&cubeVerts[1], &cubeVerts[5]));
   cubeEdges.push_back(new EDGE(&cubeVerts[2], &cubeVerts[6]));
   cubeEdges.push_back(new EDGE(&cubeVerts[3], &cubeVerts[7]));
   // Compose mesh.
   MESH *cubeMesh = new MESH(cubeEdges, &IdentityMatrix[0][0]);
   cubeMesh->GetWorldMatrix()->Translate(0, 0.25, 0);
   // Front quad.
   cubeMesh->TrisFromQuad(&cubeVerts[4], &cubeVerts[5], &cubeVerts[0], &cubeVerts[1], green);
   // Back quad.
   cubeMesh->TrisFromQuad(&cubeVerts[2], &cubeVerts[3], &cubeVerts[6], &cubeVerts[7], green);
   // Right quad.
   cubeMesh->TrisFromQuad(&cubeVerts[5], &cubeVerts[7], &cubeVerts[1], &cubeVerts[3], red);
   // Left quad.
   cubeMesh->TrisFromQuad(&cubeVerts[4], &cubeVerts[6], &cubeVerts[0], &cubeVerts[2], blue);
   // Bottom quad.
   cubeMesh->TrisFromQuad(&cubeVerts[0], &cubeVerts[1], &cubeVerts[2], &cubeVerts[3], yellow);
   // Top quad.
   cubeMesh->TrisFromQuad(&cubeVerts[4], &cubeVerts[5], &cubeVerts[6], &cubeVerts[7], yellow);

   cubeMesh->SetTexture(&greendragon_pixels[0], greendragon_width, greendragon_height);
   float cubeAngle = 0.0f;
#pragma endregion

   /* ------------------ Initialize Camera ----------------- */
#pragma region Camera
   CAMERA *camera = new CAMERA();
   camera->SetViewMatrix(&IdentityMatrix[0][0]);
   camera->GetViewMatrix()->DRotate(0, -18);
   camera->GetViewMatrix()->Translate(0, 0.4, -1);
   camera->Invert();
   camera->SetAspectRatio(rasterWidth, rasterHeight);
   camera->SetFOV(90);
   camera->SetProjMatrix(0.1f, 10.0f);
#pragma endregion

   /* ---------------------- Core Loop --------------------- */
   do
   {
      onlyRaster->CleanZ();
      onlyRaster->ClearRaster(0xFF000000);
      // cubeAngle += 0.165f;
      cubeAngle += 10.165f;
      MESH *gridCopy = new MESH(*gridMesh);
      MESH *cubeCopy = new MESH(*cubeMesh);
      cubeCopy->GetLocalMatrix()->DRotate(1, cubeAngle);
      ShaderUtil::VS_ProjectEdges(*gridCopy, camera);
      ShaderUtil::VS_ProjectFaces(*cubeCopy, camera);
      gridCopy->RenderWireframe(onlyRaster);
      cubeCopy->RenderFaces(onlyRaster);
      delete gridCopy;
      delete cubeCopy;
      onlyRaster->EvaluateZ(camera);
   } while (RS_Update(onlyRaster->GetSurface(), onlyRaster->GetArea()));

   RS_Shutdown();
   delete gridMesh;
   delete cubeMesh;
   delete camera;
   delete onlyRaster;
   return 0;
}
