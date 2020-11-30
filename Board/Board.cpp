#include "pch.h"

#include "Board.h"

Board::Board() :
    m_nextBoard (nullptr),
    m_nMiss (0),
    m_nSelectedX(-1),
    m_nSelectedY(-1),
    m_Assumption { 0xFF,0xFF, 0xFFFF, (char)(-1) }
{
}

Board::Board(const Board& board) :
    m_nextBoard(nullptr),
    m_nMiss(0),
    m_nSelectedX(-1),
    m_nSelectedY(-1),
    m_Assumption{ 0xFF,0xFF, 0xFFFF, (char)(-1) }
{
    for (int i = 0; i < 81; i++)
    {
        m_Cells[i] = board.m_Cells[i];
    }
}

void Board::OnRender()
{
   if (m_pCurrentCell)
   {
      m_pCurrentCell->OnRenderSelected(m_nSelectedX, m_nSelectedY);
   }
   for (int y = 0; y < 9; y++)
   {
      for (int x = 0; x < 9; x++)
      {
         m_Cells[x + 9 * y].OnRender(x, y);
      }
   }
}

void Board::OnMouseDown(double posX, double posY)
{
   double dPercentX = MathR::GetPercent(g_rectBoard.x, g_rectBoard.z, posX);
   double dPercentY = MathR::GetPercent(g_rectBoard.y, g_rectBoard.w, posY);

   if (dPercentX >= 0 && dPercentX <= 1 && dPercentY >= 0 && dPercentY <= 1)
   {

      int nGridX = static_cast<int>(dPercentX * 9);
      int nGridY = static_cast<int>(dPercentY * 9);
      //LOG_INFO("Grid Coord: x: {0}, y: {1}", nGridX, nGridY);
      m_nSelectedX = nGridX;
      m_nSelectedY = nGridY;
      m_pCurrentCell = &m_Cells[nGridX + nGridY * 9];
   }
   else
   {
       ClearSelection();
   }
}

void Board::OnKey(int key, bool bSolving)
{

    if (bSolving)
    {
        return;
    }

    //keyboard key values
    const int nLeft = 263;
    const int nRight = 262;
    const int nUp = 265;
    const int nDown = 264;

    if (m_pCurrentCell)
    {
        //move the current selection if the arrow keys weer pressed
        //move the currnet selection with the arrow keys
        switch (key)
        {
            case nLeft:
            {
                m_nSelectedX--;
                if (m_nSelectedX < 0) m_nSelectedX = 8;
                break;
            }
            case nRight:
            {
                m_nSelectedX++;
                if (m_nSelectedX > 8) m_nSelectedX = 0;
                break;

            }
            case nUp:
            {
                m_nSelectedY++;
                if (m_nSelectedY > 8) m_nSelectedY = 0;
                break;

            }
            case nDown:
            {
                m_nSelectedY--;
                if (m_nSelectedY < 0) m_nSelectedY = 8;
                break;

            }
        }

        //insert a number into the currrently selected block if a number key was pressed
        const int keyNumPad0 = 320;
        const int keyNum0 = '0';
        
        unsigned char val = 20;
        if (key >= keyNum0 && key <= keyNum0 + 9)
        {
            val = static_cast<unsigned char> (key - keyNum0);
        }
        else if (key >= keyNumPad0 && key <= keyNumPad0 + 9)
        {
            val = static_cast<unsigned char> (key - keyNumPad0);
        }

        if (val >= 0 && val <= 9)
        {
            m_pCurrentCell->SetValue(val);
            m_pCurrentCell->SetPossibleL(0);

            //Move the selected tile to the left to make it easier to fill
            m_nSelectedX++;
            if (m_nSelectedX >= 9)
            {
                m_nSelectedX = 0;
                m_nSelectedY++;
                m_nSelectedY %= 9;
            }
            m_pCurrentCell = &m_Cells[m_nSelectedX + 9 * m_nSelectedY];
        }
    }
    else if (key >= nRight && key <= nUp)
    {
        //select the 0,0 tile by default
        m_nSelectedX = m_nSelectedY = 0;
    }

    m_pCurrentCell = &m_Cells[m_nSelectedX + m_nSelectedY * 9];

}

void Board::ClearBoard()
{
   for (int i = 0; i < 81; i++)
   {
      m_Cells[i].Clear();
   }
}
void Board::ResetPossibleValues(unsigned short val)
{
    
    for (int i = 0; i < 81; i++)
    {
        if (!m_Cells[i].GetValue())
        {
            m_Cells[i].SetPossibleL(val);
        }
    }
}
void Board::LoadBoard(unsigned char data[81])
{
    for (int x = 0; x < 81; x++)
    {
        m_Cells[x].Clear();
        char c = data[x];
        if (c >= 0 || c <= 9)
        {
            m_Cells[x].SetValue(c);
        }
    }
}
bool Board::IsSolved() const
{
    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            unsigned char val = m_Cells[x + 9 * y].GetValue();
            if (val < 1 || val > 9) return false;
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////
//                   Check Contradiction                               //
/////////////////////////////////////////////////////////////////////////
bool Board::CheckContradiction() const
{
    auto ResetArray = [](bool arr[])
    {
#if 0
        for (int i = 0; i < 9; i++)
        {
            arr[i] = 0;
        }
#else
        memset(arr, 0, 10);
#endif
    };

    bool seen[10];
    
    //check contradictions along columns
    for (int x = 0; x < 9; x++)
    {
        ResetArray(seen);
        for (int y = 0; y < 9; y++)
        {
            unsigned char c = m_Cells[x + y * 9].GetValue();
            if (c > 0)
            {
                ASSERT(c <= 9, "Invalid value of cell");
                if (seen[c])    return true;
                seen[c] = true;
            }
        }
    }

    //check contradictions along rows
    for (int y = 0; y < 9; y++)
    {
        ResetArray(seen);
        for (int x = 0; x < 9; x++)
        {
            unsigned char c = m_Cells[x + y * 9].GetValue();
            if (c > 0)
            {
                ASSERT(c <= 9, "Invalid value of cell");
                if (seen[c])    return true;
                seen[c] = true;
            }
        }
    }

    //check contraditions along 3X3 block

    for (int outerY = 0; outerY <= 6; outerY += 3)
    {
        for (int outerX = 0; outerX <= 6; outerX += 3)
        {
            ResetArray(seen);
            for (int innerY = 0; innerY < 3; innerY++)
            {
                const int y = outerY + innerY;
                for (int innerX = 0; innerX < 3; innerX++)
                {
                    const int x = outerX + innerX;
                    unsigned char c = m_Cells[x + y * 9].GetValue();
                    if (c > 0)
                    {
                        ASSERT(c <= 9, "Invalid value of cell");
                        if (seen[c])    return true;
                        seen[c] = true;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 81; i++)
    {
        if (!m_Cells[i].GetPossitbleL() && !m_Cells[i].GetValue())
        {
            //LOG_WARN("Warning: Empty cell has no possibilities");
            return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//                                                                     //
//                   Eliminate Possible Values                         //
//                                                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
void Board::EliminatePossibleValues()
{
    auto CalculateSeen = [this](int x, int y, short& seen)
    {
        unsigned char c = m_Cells[x + 9 * y].GetValue();
        if (c)
        {
            ASSERT(c < 10 && c >= 0, "Value of the cell is out of range.");
            seen |= (1 << (c - 1));
        }
    };

    auto RemoveSeen = [this](int x, int y, short seen)
    {
        if (!m_Cells[x + y * 9].GetValue())
        {
            m_Cells[x + 9 * y].RemovePossibleL(seen);
        }
    };

    //go through each COLUMN, Store a list of numbers that are known, and eliminate those numbers
    for (int x = 0; x < 9; x++)
    {
        short nSeen = 0;
        //Calculate the seen numbers
        for (int y = 0; y < 9; y++)
        {
            CalculateSeen(x, y, nSeen);
        }

        //Since we have calculated the seen numbers, remove it from the list of all possible numbers for that column
        for (int y = 0; y < 9; y++)
        {
            RemoveSeen(x, y, nSeen);
        }
    }

    //go through each ROW, Store a list of numbers that are fixed for that row (we know its value), and eliminate those 'seen' numbers from the possible values in that row
    for (int y = 0; y < 9; y++)
    {
        short nSeen = 0;
        //calculate the seen numbers
        for (int x = 0; x < 9; x++)
        {
            CalculateSeen(x, y, nSeen);
        }

        //Since we have calculated the seen numbers, remove it from the list of all possible numbers for that row
        for (int x = 0; x < 9; x++)
        {
            RemoveSeen(x, y, nSeen);
        }
    }

    //go through each 3X3 BLOCK to remove possible values
    for (int outerY = 0; outerY <= 6; outerY += 3)
    {
        for (int outerX = 0; outerX <= 6; outerX += 3)
        {
            short nSeen = 0;
            //calculate the seen numbers in this 3x3 block
            for (int innerY = 0; innerY < 3; innerY++)
            {
                const int y = outerY + innerY;
                for (int innerX = 0; innerX < 3; innerX++)
                {
                    const int x = outerX + innerX;
                    CalculateSeen(x, y, nSeen);
                }
            }

            //Go through each element in this 3x3 block and remove the 'seen' numbers from the posssible values
            for (int innerY = 0; innerY < 3; innerY++)
            {
                const int y = outerY + innerY;
                for (int innerX = 0; innerX < 3; innerX++)
                {
                    const int x = outerX + innerX;
                    RemoveSeen(x, y, nSeen);

                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//                                                                     //
//                       Set Unique values                             //
//                                                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

//DELETE THIS LATER
extern GLFWwindow* g_TempWindow;
#include "Renderer/Renderer.h"

void Board::SetUniqueCellValue()
{
    Pair pairs[9];

    auto CalculatePossibilityCount = [&pairs, this](int x, int y)
    {
        int index = x + y * 9;
        unsigned char nVal = m_Cells[index].GetValue();
        if (nVal < 1 || nVal > 9)
        {
            //calculate the pairs 
            unsigned short nPossible = m_Cells[index].GetPossitbleL();
            unsigned short bitMask = 1;
            for (int i = 0; i < 9; i++, bitMask <<= 1)
            {
                if (nPossible & bitMask)
                {
                    const Pair pair = { pairs[i].nCount + 1, x, y };
                    pairs[i] = pair;
                }
            }
        }
        else
        {
            //the number mVal already exists in this section, so mark the pair as invalid
            pairs[nVal - 1].nCount = 30;    //30 is an arbitrary high number... The pair's value is used only if nCount = 1, so any number higher than 1 would work.
        }
    };


    auto SetValuesFromPossibilityCount = [&pairs, this]() -> int
    {
        int num = 0;
        //Pairs have been calculated, now check if any of the occurences if 1
       //check if theres any digit which can only exist in one cell, if so then assign it
        for (unsigned char i = 0; i < 9; i++)
        {
            if (pairs[i].nCount == 1)
            {
                const int nIndex = pairs[i].nx + pairs[i].ny * 9;
                m_Cells[nIndex].SetValue(i + 1);
                num++;
            }
        }
        return num;
    };


    int nReplacements = 0;  //stores the number of cells whose value was calculated in this function (solve step).

    ///Now check if a ROW has only one cell which can contain a number (Eg: if the number 9 is only possible at one place on the row, then u can set the value of that Cell to 9)
    for (int y = 0; y < 9; y++)
    {
        memset(pairs, 0, sizeof(Pair) * 9);
        for (int x = 0; x < 9; x++)
        {
            CalculatePossibilityCount(x, y);
        }
        nReplacements += SetValuesFromPossibilityCount();
    }


    //Now check if a Column has only one cell which can contain a number (Eg: if the number 9 is only possible at one place on the column, then u can set the value of that Cell to 9)

    for (int x = 0; x < 9; x++)
    {
        memset(pairs, 0, sizeof(Pair) * 9);
        for (int y = 0; y < 9; y++)
        {
            CalculatePossibilityCount(x, y);
        }

        //Pairs have been calculated, now check if any of the occurences if 1
        //check if theres any digit which can only exist in one cell, if so then assign it
        nReplacements += SetValuesFromPossibilityCount();
    }

    //Now check if a 3X3 block has only one cell which can contain a number (Eg: if the number 9 is only possible at one place on the block, then u can set the value of that Cell to 9)
    for (int outerY = 0; outerY <= 6; outerY += 3)
    {
        for (int outerX = 0; outerX <= 6; outerX += 3)
        {
            memset(pairs, 0, sizeof(Pair) * 9);

            for (int innerY = 0; innerY < 3; innerY++)
            {
                const int y = outerY + innerY;
                for (int innerX = 0; innerX < 3; innerX++)
                {
                    const int x = outerX + innerX;
                    CalculatePossibilityCount(x, y);
                }
            }

            //Pairs have been calculated, now check if any of the occurences if 1
            //check if theres any digit which can only exist in one cell, if so then assign it
            nReplacements += SetValuesFromPossibilityCount();
        }
    }

    if (!nReplacements)
    {
        m_nMiss++;
    }
    else
    {
        m_nMiss = 0;
    }
}


/////////////////////////////////////////////////////////////////////////
//                                                                     //
//                                                                     //
//                                                                     //
//                           THIS IS IT....                            //
//         The main function which ties everything together            //
//                                                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
SolveState Board::SolveStep()
{
    if (CheckContradiction())   return SolveState::Contradiction;
    if (IsSolved())             return SolveState::Solved;
    
    EliminatePossibleValues();

    //Now check if a cell contains only one possibility.. If that is the case then you can set its value to the only possible value.
    for (int i = 0; i < 81; i++)
    {
        m_Cells[i].UpdateValue();
    }
    
    //This function also calculates the m_nMiss
    SetUniqueCellValue();
    
    const unsigned char nMissMax = 3;
    if (m_nMiss >= nMissMax)
    {
        m_nMiss = 0;
        return SolveState::CreateGuess;
    }
    return SolveState::None;
}

BoardAssumption Board::CreateAssumption() const
{
    // stores a pointer to a cell which contains 2 to 9 possibilities
    const Cell* cells[8];
    unsigned char posX[8];
    unsigned char posY[8];

    for (int i = 0; i < 8; cells[i] = 0, i++);

    int nAdded = 0;

    for (unsigned char y = 0; y < 9 && nAdded < 8; y++)
    {
        for (unsigned char x = 0; x < 9 && nAdded < 8; x++)
        {
            if (m_Cells[x + 9 * y].GetValue())
            {
                continue;
            }
            //this variable will vary from 0 to 7 (GetNumPos will vary from 1-9, AND it cannot be 1 because this function would be called only when its time to make an assumption)
            unsigned char nPossibilities = m_Cells[x + 9*y].GetNumPossibilities();
            ASSERT((nPossibilities >= 0 && nPossibilities <= 9), "Invalid sum");

            if (nPossibilities >= 2 && nPossibilities <= 9 && !cells[nPossibilities-2])
            {
                nPossibilities -= 2;        //index 0 of cells array stores a Cell which has 2 possibilities.         
                cells[nPossibilities] = &m_Cells[x+9*y];
                posX[nPossibilities] = x;
                posY[nPossibilities] = y;
                nAdded++;
                if (!nPossibilities) { nAdded = 10; }   //this is just a hack to break from inner AND outer for loop at the same time.
            }
        }
    }

    
    for (unsigned char i = 0; i < 8; i++)
    {
        if (cells[i])
        {
            const unsigned short n = cells[i]->GetPossitbleL();
            char digit = 1;
            for (unsigned short bitMask = 1; bitMask <= 0x01FF; bitMask <<= 1)
            {
                if (bitMask & n)
                {
                    return BoardAssumption{posX[i], posY[i], bitMask, digit};
                }
                digit++;
            }
        }
    }

    ASSERT(false, "Board Assumption could not be created");
    return BoardAssumption{ 0xFF,0xFF, 0xFFFF, (char)(-1) };
}