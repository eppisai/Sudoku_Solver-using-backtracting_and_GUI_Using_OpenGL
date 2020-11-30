#include "pch.h"
#include "Application.h"

int main()
{
   //////////////////////////////////////////////////////////////////////
   //                           Initialise                             //
   //////////////////////////////////////////////////////////////////////
   Log::Init();

   if (!glfwInit())
   {
      ASSERT(false, "Failed to initialise glfw");
      return 0;
   }

   //////////////////////////////////////////////////////////////////////
   //                       Create Window                              //
   //////////////////////////////////////////////////////////////////////
   int nWidth = 1920, nHeight = 1080;
   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   GLFWwindow* pWindow = glfwCreateWindow(nWidth, nHeight, "Sudoku Solver", nullptr, nullptr);
   if (!pWindow)
   {
      ASSERT(false, "Failed to create glfw window");
      glfwTerminate();
      return 0;
   }
  
   //////////////////////////////////////////////////////////////////////
   //                       GLEW                                       //
   //////////////////////////////////////////////////////////////////////

   glfwMakeContextCurrent(pWindow);
   glfwSwapInterval(1);

   if (glewInit() != GLEW_OK)
   {
      ASSERT(false, "Failed to initialse GLEW");
      glfwTerminate();
      return 0;

   }

   glcall(glEnable(GL_BLEND));
   glcall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

   //LOG_TRACE("OpenGL Version {0}", glGetString(GL_VERSION));

   //Now you can finally start doing stuff...
   {
      Application* pApp = new (std::nothrow) Application(pWindow, nWidth, nHeight);
      ASSERT(pApp, "Memory allocation failed");
      pApp->Run();
      delete pApp;
   }

   glfwTerminate();
   return 0;
}