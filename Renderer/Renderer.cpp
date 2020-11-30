#include "pch.h"
#include "Renderer.h"
#include "File/FileManager.h"

//store the count and then store the 
static const unsigned int l_indexQuad[] = { 0,1,2,2,3,0 };

//member variables
unsigned int Renderer::m_nVertexArray        = 0;
unsigned int Renderer::m_nVertexBuffer       = 0;
unsigned int Renderer::m_nIndexBuffer        = 0;
unsigned int Renderer::m_nShader             = 0;

unsigned int Renderer::m_nMaxVertices        = 0;
unsigned int Renderer::m_nMaxIndices         = 0;

unsigned int Renderer::m_nCurrVertex         = 0;
unsigned int Renderer::m_nCurrIndex          = 0;
unsigned int Renderer::m_nStartingVertex     = 0;
unsigned int Renderer::m_nStartingIndex      = 0;

int Renderer::m_nDrawCalls = 0;

glm::mat4 Renderer::m_matMvp;
int Renderer::m_nUniformMvp;
int Renderer::m_nUniformSampler;



unsigned int CompileShader(unsigned int type, const std::string& strShader)
{
   unsigned int id = glCreateShader(type);
   const char* const path = strShader.c_str();
   glcall(glShaderSource(id, 1, &path, 0));
   glcall(glCompileShader(id));

   int status;
   glcall(glGetShaderiv(id, GL_COMPILE_STATUS, &status));

   if (status == GL_FALSE)
   {
      int size;
      glcall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &size));
      char* buff = new char[size];

      glcall(glGetShaderInfoLog(id, size, 0, buff));
      std::string str = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
      ASSERT(false, "Failed to compile {0} shader: {1}", str.c_str(), buff);
      delete[] buff;
   }

   return id;
}

void Renderer::Init()
{
   //Initialise values
   {
      m_nMaxVertices = 800;
      m_nMaxIndices = m_nMaxVertices * 6 / 4;
      m_nCurrVertex = 0;
      m_nCurrIndex = 0;
      m_nStartingVertex = 0;
      m_nStartingIndex = 0;
   }

   //create vao
   glcall(glGenVertexArrays(1, &m_nVertexArray));
   glcall(glBindVertexArray(m_nVertexArray));

   //create vertex buffer
   glcall(glGenBuffers(1, &m_nVertexBuffer));
   glcall(glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer));
   glcall(glBufferData(GL_ARRAY_BUFFER, m_nMaxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

   //layout
   {
      //position
      glcall(glEnableVertexAttribArray(0));
      glcall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, m_vPosition)));

      //color
      glcall(glEnableVertexAttribArray(1));
      glcall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, m_vColor)));

      //texture id
      glcall(glEnableVertexAttribArray(2));
      glcall(glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, m_nTextureId)));
      
      //texture coords
      glcall(glEnableVertexAttribArray(3));
      glcall(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, m_vTextureCoord)));
   }

   //create index buffer
   glcall(glGenBuffers(1, &m_nIndexBuffer));
   glcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIndexBuffer));
   glcall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nMaxIndices * sizeof(int), nullptr, GL_DYNAMIC_DRAW));

   std::string strShaders[2];
   FileManager::ParseShader("Assets/Shader/Generic.shader", strShaders);
   unsigned int nVertex = CompileShader(GL_VERTEX_SHADER, strShaders[e_VertexShaderIndex]);
   unsigned int nFrag = CompileShader(GL_FRAGMENT_SHADER, strShaders[e_FragmentShaderIndex]);

   m_nShader = glCreateProgram();
   glcall(glAttachShader(m_nShader, nVertex));
   glcall(glAttachShader(m_nShader, nFrag));
   glcall(glLinkProgram(m_nShader));
   glcall(glValidateProgram(m_nShader));

#ifdef _DEBUG
   {
      int success;
      glGetProgramiv(m_nShader, GL_LINK_STATUS, &success);
      ASSERT(success == GL_TRUE, "Error: Shader did not link properly")
         glGetProgramiv(m_nShader, GL_VALIDATE_STATUS, &success);
      ASSERT(success == GL_TRUE, "Error: Shader did not validate properly")
   }
#endif

   glcall(glUseProgram(m_nShader));

   GlClearError();
   m_nUniformMvp = glGetUniformLocation(m_nShader, "u_Mvp");
   GlCheckError();

   if (m_nUniformMvp < 0) { LOG_WARN("Warning: Uniform u_Mvp was not found"); }

   GlClearError();
   m_nUniformSampler = glGetUniformLocation(m_nShader, "u_textureIds");
   GlCheckError();

   if (m_nUniformSampler < 0) { LOG_WARN("Warning: Uniform u_textureIds was not found"); }

   
}

void Renderer::BegineScene()
{
   m_nCurrVertex = m_nStartingVertex;
   m_nCurrIndex = m_nStartingIndex;
}
void Renderer::EndScene()
{
   if (!m_nDrawCalls || (m_nCurrIndex != m_nStartingIndex || m_nCurrVertex != m_nStartingVertex))
   {
      Renderer::RendBatch();
   }
   m_nDrawCalls = 0;
}
void Renderer::RendBatch()
{
   int nCount = (!m_nDrawCalls) ? m_nCurrIndex : m_nCurrIndex - m_nStartingIndex;
   int nOffset = (!m_nDrawCalls) ? 0 : m_nStartingIndex * sizeof(int);

   int samplers[2] = { 0, 1 };
   glcall(glUniform1iv(m_nUniformSampler, 2, samplers));
   glcall(glUniformMatrix4fv(m_nUniformMvp, 1, GL_FALSE, &m_matMvp[0][0]));
   glcall(glDrawElements(GL_TRIANGLES, nCount, GL_UNSIGNED_INT, (const void*) nOffset));

   m_nCurrVertex = m_nStartingVertex;
   m_nCurrIndex = m_nStartingIndex;
   m_nDrawCalls++;
}
void Renderer::DrawQuad(const Quad& quad)
{
   //check if there is enough room in the batch for drawing a quad
   if (m_nCurrVertex + 4 > m_nMaxVertices || m_nCurrIndex + 6 > m_nMaxIndices)
   {
      //there isnt room so render the batch
      RendBatch();
   }
   glcall(glBufferSubData(GL_ARRAY_BUFFER, m_nCurrVertex * sizeof(Vertex), 4*sizeof(Vertex), &quad.m_vertices[0]));
   
   unsigned int indices[6];
   for (int i = 0; i < 6; i++)
   {
      indices[i] = l_indexQuad[i] + m_nCurrVertex;
   }
   
   glcall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_nCurrIndex * sizeof(int), 6*sizeof(int), indices));
   
   m_nCurrVertex += 4;
   m_nCurrIndex += 6;
}

//Position is the coordinates of bottom left point and scale is the length and breadth
void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color)
{
   Quad quad;
   //anticlockwise, starting from bottom left point
   quad.m_vertices[0] = { position, color, 0, {0,0} };
   quad.m_vertices[1] = { {position.x + scale.x, position.y}, color, 0, {0,0} };
   quad.m_vertices[2] = { {position.x + scale.x, position.y + scale.y}, color, 0, {0,0} };
   quad.m_vertices[3] = { {position.x, position.y + scale.y}, color, 0, {0,0} };
   DrawQuad(quad);
}

void Renderer::DrawQuad(float x, float y, float w, float h, const glm::vec4& color)
{
   DrawQuad(glm::vec2{ x,y }, glm::vec2{ w,h }, color);
}

void Renderer::ResetBackground()
{
   m_nStartingIndex = 0;
   m_nStartingVertex = 0;
}
//call this function once all the static elements have been 'rendered'
void Renderer::SetBackground()
{
   m_nStartingIndex = m_nCurrIndex;
   m_nStartingVertex = m_nCurrVertex;
}

void Renderer::SetMvpMatrix(float left, float right, float bottom, float top)
{
   m_matMvp = glm::ortho<float>(left, right, bottom, top, -1.0f, 1.0f);
   glm::vec4 pos{ left, top, 0 ,0 };
   
   glm::vec4 pos2 = m_matMvp * pos;
   glm::vec4 pos3 = pos * m_matMvp;

   int jim = 1;
}