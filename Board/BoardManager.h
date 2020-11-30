#pragma once
#include <glm.hpp>
#include "Board.h"

//stores xy position of bottom left corner and then xy pos of top right corner
extern glm::vec4 g_rectBoard;

class BoardManager
{
public:
   BoardManager();
   ~BoardManager();

   void SetBoardPos(const glm::vec4& pos) { g_rectBoard = pos; }
   
   void OnMouseDown(double posX, double posY);
   void OnKey(int key);

   void DrawBoard();
   void OnRender();

   //This function does the solving 
   void Update();

private:
   void DeleteExtraBoards();
   void SolveStep();

private:
   Board* m_pBoard;
   bool m_bSolving;

   int m_nBoardListCount;
   const int m_nMaxCount = 1000;	//Max boards that can be created in the list... If it exceeds that, then it will stop solving and delete the extra boards
};