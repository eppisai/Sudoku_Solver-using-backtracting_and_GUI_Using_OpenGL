#pragma once
#include "glm.hpp"

struct Vertex
{
   glm::vec2 m_vPosition;
   glm::vec4 m_vColor;
   float m_nTextureId;
   glm::vec2 m_vTextureCoord;
};
struct Quad
{
   Vertex m_vertices[4];
};
class Renderer
{
private:
   Renderer() {}

public:
   static void BegineScene();
   static void EndScene();
   static void Init();
   static void ResetBackground();
   static void SetBackground();

   static void SetMvpMatrix(float left, float right, float bottom, float top);

   //Draw stuff
   static void DrawQuad(const Quad& quad);

   static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color);
   static void DrawQuad(float x, float y, float w, float h, const glm::vec4& color);
   
private:
   static void RendBatch();
private:
   static unsigned int m_nVertexArray;
   static unsigned int m_nVertexBuffer;
   static unsigned int m_nIndexBuffer;
   static unsigned int m_nShader;

   static unsigned int m_nMaxVertices;
   static unsigned int m_nMaxIndices;

   static unsigned int m_nCurrVertex;
   static unsigned int m_nCurrIndex;
   static unsigned int m_nStartingVertex;
   static unsigned int m_nStartingIndex;
   

   static int m_nDrawCalls;

   static glm::mat4 m_matMvp;

   static int m_nUniformMvp;
   static int m_nUniformSampler;
};