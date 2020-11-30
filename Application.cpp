#include "pch.h"
#include "Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/AssetManager.h"
#include <thread>   //for sleep

Application::Application(GLFWwindow* pWindow, int nWidth, int nHeight) :
   m_boardManager(),
   m_pWindow (pWindow),
   m_nWidth (nWidth),
   m_nHeight (nHeight)
{
   ASSERT(m_pWindow, "Window was null");
}

Application::~Application()
{
   ASSERT(m_pWindow, "Window was null");
   if (m_pWindow) {
      glfwDestroyWindow(m_pWindow);
   }
}

//DELETE THIS LATER
GLFWwindow* g_TempWindow;

void Application::Run()
{
   Renderer::Init();
   Renderer::SetMvpMatrix(0.0f, (float)m_nWidth, 0.0f, (float)m_nHeight);
   
   g_TempWindow = m_pWindow;

   {
      unsigned char buf[] = { 255, 255, 255, 255 };
      Texture* whiteId = AssetManager::LoadTexture(1, 1, buf, 0);
      Texture* texId = AssetManager::LoadTexture("Assets/Image/number.png", 1);
   }

   glcall(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));


   //Setup window callback events
  
   glfwSetWindowUserPointer(m_pWindow, this);

   glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods)
      {
         Application* pApp = static_cast<Application*>(glfwGetWindowUserPointer(window));
         if (pApp)
         {
            //LOG_INFO("button {0}, action {1}", button, action);
            if (button == 0 && action == GLFW_PRESS)
            {
               double x, y;
               glfwGetCursorPos(window, &x, &y);
               y = pApp->GetHeight() - y;
               pApp->GetBoardManager().OnMouseDown(x, y);
            }
         }
         else { LOG_ERROR("Error: Application was null"); }
      });

   glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
      {
         Application* pApp = static_cast<Application*>(glfwGetWindowUserPointer(window));
         if (pApp)
         {
            //LOG_INFO("Key {0}, scancode {1}, action {2}, mods {3}", key, scancode, action, mods);
            if (action == GLFW_PRESS || action == GLFW_REPEAT)
            {
               pApp->GetBoardManager().OnKey(key);
            }

         }
         else { LOG_ERROR("Error: Application was null"); }
      });

   DrawBackground();
   while (!glfwWindowShouldClose(m_pWindow))
   {
      glcall(glClear(GL_COLOR_BUFFER_BIT));
      Renderer::BegineScene();

      m_boardManager.Update();
      m_boardManager.OnRender();

      Renderer::EndScene();
      glfwSwapBuffers(m_pWindow);
      glfwPollEvents();

      if (m_nSleepDuration)
      {
          std::this_thread::sleep_for(std::chrono::milliseconds(m_nSleepDuration));
      }
   }

   AssetManager::OnExit();
}

void Application::DrawBackground()
{
   Renderer::ResetBackground();

   //bottom left corner, counter clockwise
   {
      Quad quadBackground;

      glm::vec4 colTopLeft =  { 35,35,35,255 };
      glm::vec4 colTopRight = { 35,35,35,255 };
      glm::vec4 colBotLeft =  { 35,35,35,255 };
      glm::vec4 colBotRight = { 35,35,35,255 };

      colTopLeft *= 1.0f / 255.0f;
      colTopRight *= 1.0f / 255.0f;
      colBotLeft *= 1.0f / 255.0f;
      colBotRight *= 1.0f / 255.0f;

      quadBackground.m_vertices[0] = Vertex{ {0, 0}, colBotLeft, 0, {0.0, 0.0} };

      quadBackground.m_vertices[1] = Vertex{ {m_nWidth, 0},colBotRight, 0, {1.0, 0.0} };
      
      quadBackground.m_vertices[2] = Vertex{ {m_nWidth, m_nHeight}, colTopRight, 0, {1.0, 1.0} };
      
      quadBackground.m_vertices[3] = Vertex{ {0, m_nHeight}, colTopLeft, 0, {0.0, 1.0} };
      Renderer::DrawQuad(quadBackground);
   }

   const int nPaddingBot = 4;
   const int nPaddingTop = 4;
   m_boardManager.SetBoardPos(glm::vec4{ nPaddingBot,nPaddingBot, m_nHeight- nPaddingTop, m_nHeight- nPaddingTop });
   m_boardManager.DrawBoard();
   Renderer::SetBackground();
}

void Application::OnUpdate()
{

}
void Application::OnRender()
{

}