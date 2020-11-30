#pragma once

#include "GLFW/glfw3.h"
#include "Board/BoardManager.h"

class Application
{
public:
   Application(GLFWwindow* pWindow, int nWidth, int nHeight);
   ~Application();

   void Run();
   BoardManager& GetBoardManager() { return m_boardManager; }

   inline int GetWidth() const { return m_nWidth; }
   inline int GetHeight() const { return m_nHeight; }

private:
   void DrawBackground();
   void OnUpdate();
   void OnRender();

private:
   GLFWwindow* m_pWindow;
   BoardManager m_boardManager;
   int m_nWidth;
   int m_nHeight;

   const int m_nSleepDuration = 0;  //in milliseconds
};