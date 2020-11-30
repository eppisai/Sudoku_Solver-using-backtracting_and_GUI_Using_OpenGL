
#include "pch.h"
#include "BoardManager.h"
#include "Renderer/Renderer.h"

glm::vec4 g_rectBoard;

BoardManager::BoardManager() :
   m_bSolving (false),
    m_nBoardListCount(1)
{
   m_pBoard = new Board;
   m_nBoardListCount = 1;
}
BoardManager::~BoardManager()
{
   while (m_pBoard)
   {
      Board* pBoard = m_pBoard->GetNext();
      delete m_pBoard;
      m_pBoard = pBoard;
   }
}

void BoardManager::OnMouseDown(double posX, double posY)
{
   if (m_pBoard)
   {
      m_pBoard->OnMouseDown(posX, posY);
   }
   else { LOG_WARN("Warning: Current board is null"); }
}
void BoardManager::OnKey(int key)
{
    static bool bResetValues = true;

    const int nSolvingKey = 'S';
    const int nClearAllKey = 'C';
    if (m_pBoard)
    {
        m_pBoard->OnKey(key, m_bSolving);
      
        if (key == nSolvingKey && !m_bSolving)
        {
            //Start solving
            m_bSolving = true;
            m_pBoard->ClearSelection();
            m_pBoard->ResetPossibleValues(0x01FF);
        }
        if (key == nClearAllKey)
        {
            DeleteExtraBoards();
            m_pBoard->ClearBoard();
            m_pBoard->SetMiss(0);
            m_bSolving = false;
            bResetValues = true;
        }

#if 1
        if (key == 'Q')
        {
            if (bResetValues)
            {
                m_pBoard->ResetPossibleValues(0x01FF);
                bResetValues = false;
            }
            SolveStep();
        }
#endif
   }
   else { LOG_WARN("Warning: Current board is null"); }
}

void BoardManager::DeleteExtraBoards()
{
   if (!m_pBoard) m_pBoard = new Board();
   else
   {
      Board* pCur = m_pBoard;
      Board* pNext = m_pBoard->GetNext();
      while (pNext)
      {
          delete pCur;
          pCur = pNext;
          pNext = pNext->GetNext();
      }

      m_pBoard = pCur;
   }
   m_nBoardListCount = 1;
}
void BoardManager::DrawBoard()
{
   //properties
   const glm::vec4 colMajor = { 0.0, 0.0, 0.0, 1.0 };
   const glm::vec4 colMinor = { 0.2,0.2,0.2,0.7 };
   const float nThicknessMinor = 3;
   const float nThicknessMajor = 5;

   //helper variables
   const float width  = (g_rectBoard.z - g_rectBoard.x);
   const float height = (g_rectBoard.w - g_rectBoard.y);
   const float startingX = g_rectBoard.x;
   const float startingY = g_rectBoard.y;
   
   float x = g_rectBoard.x + width / 9.0f;
   float y = g_rectBoard.y + height / 9.0f;
   for (int i = 1; i < 10; i++)
   {
      if (i % 3 != 0)
      {
         Renderer::DrawQuad(startingX, y, width, nThicknessMinor, colMinor);//horizontal
         Renderer::DrawQuad(x, startingY, nThicknessMinor, height, colMinor);//vertical
      }

      x += width/9.0f;
      y += height/9.0f;
   }

   //draw major gridlines ON TOP of the minor ones
   x = g_rectBoard.x;
   y = g_rectBoard.y;
   for (int i = 0; i < 4; i++)
   {
      Renderer::DrawQuad(startingX, y, width, nThicknessMajor, colMajor);//horizontal
      Renderer::DrawQuad(x, startingY, nThicknessMajor, height, colMajor);//vertical
      x += width / 3.0f;
      y += height / 3.0f;
   }

#if 1
   //Load board, for debugging purposes
   /*
       0,9,0,0,0,0,4,0,0,
       0,0,8,5,0,0,0,1,0,
       0,0,1,0,0,0,0,6,8,
       0,0,0,1,0,0,0,3,0,
       0,0,0,0,4,5,7,0,0,
       0,5,0,0,0,7,0,0,0,
       0,7,0,0,9,0,2,0,0,
       0,0,3,6,0,0,0,0,0,
       8,0,0,0,0,0,0,0,0
   */
#if 1
   unsigned char board[81] = {
       0,1,0,2,0,5,0,0,7,
       0,0,7,0,1,0,0,0,6,
       2,0,0,0,7,8,0,9,0,
       3,0,0,0,0,0,7,0,4,
       7,2,8,0,6,0,0,0,0,
       9,0,0,7,0,0,0,8,5,
       0,0,0,3,0,0,0,7,0,
       0,7,0,0,5,0,4,1,3,
       0,3,0,0,0,7,0,0,0
   };
#else
   unsigned char board[81] = {
       0,9,0,0,0,0,4,0,0,
       0,0,8,5,0,0,0,1,0,
       0,0,1,0,0,0,0,6,8,
       0,0,0,1,0,0,0,3,0,
       0,0,0,0,4,5,7,0,0,
       0,5,0,0,0,7,0,0,0,
       0,7,0,0,9,0,2,0,0,
       0,0,3,6,0,0,0,0,0,
       8,0,0,0,0,0,0,0,0
       
   };
#endif

   m_pBoard->LoadBoard(board);
#endif
}

void BoardManager::OnRender()
{
   if (m_pBoard)
   {
      m_pBoard->OnRender();
   }
   else { LOG_WARN("Warning: Current board is null"); }
}

void BoardManager::Update()
{
   if (!m_bSolving || !m_pBoard) return;
   //Start solving here
   SolveStep();
}

void BoardManager::SolveStep()
{
    SolveState state = m_pBoard->SolveStep();

    switch (state)
    {
        case SolveState::Contradiction:
        {
            Board* current = m_pBoard;
            Board* next = m_pBoard->GetNext();
            if (!next)
            {
                //Contradiction... No solution exists
                LOG_WARN("Contradiction... Sudoku is impossible to solve");
                m_bSolving = false;
            }
            else
            {
                BoardAssumption assumption = current->GetAssumption();
                if (assumption.m_ndigit >= 0)
                {
                    int index = assumption.m_nBoardX + assumption.m_nBoardY * 9;
                    next->GetCells()[index].RemovePossibleL(assumption.m_nAssumption);

                    m_pBoard = next;
                    delete current;
                }
                else
                {
                    ASSERT(false, "Invalid digit stored in BoardAssumption");
                }
            }
            break;
        }
        case SolveState::CreateGuess:
        {
            if (m_nBoardListCount >= m_nMaxCount)
            {
                DeleteExtraBoards();
                m_pBoard->ResetPossibleValues(0);
                LOG_WARN("Memory overflow... Too many assumptions were made. Cannot solve this puzzle.");
                m_bSolving = false;
            }
            else
            {
                m_nBoardListCount++;

                BoardAssumption assumption = m_pBoard->CreateAssumption();
                //LOG_TRACE("Made an assumption... Cell {0}, {1}: {2}", (int)assumption.m_nBoardX, (int)assumption.m_nBoardY, (int)assumption.m_ndigit);

                if (assumption.m_ndigit >= 0)
                {
                    Board* newBoard = new Board(*m_pBoard);
                    newBoard->SetNext(m_pBoard);
                    newBoard->SetAssumption(assumption);

                    int index = assumption.m_nBoardX + assumption.m_nBoardY * 9;
                    newBoard->GetCells()[index].SetValue(assumption.m_ndigit);  //Set the value of the assumption that was made

                    m_pBoard = newBoard;
                }
            }
            break;
        }
        case SolveState::Solved:
        {
            LOG_INFO("Sudoku has been solved !!");
            m_bSolving = false;
            break;
        }
    }
}
