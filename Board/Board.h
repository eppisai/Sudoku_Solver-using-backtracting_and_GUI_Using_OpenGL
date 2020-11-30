#pragma once
#include "Cell.h"

enum class SolveState : unsigned int 
{
	None = 0,
	Contradiction,
	Solved,
	CreateGuess
};

struct BoardAssumption
{
	unsigned char m_nBoardX;
	unsigned char m_nBoardY;
	unsigned short m_nAssumption;
	char m_ndigit;
};

class Board
{
public:
   Board();
   Board(const Board& board);

   void OnRender();

   inline Board* GetNext() const { return m_nextBoard; }
   inline void SetNext(Board* pNext) { m_nextBoard = pNext; }

   inline Cell* GetCells() { return m_Cells; }

   void ClearBoard();
   void ClearSelection() { m_pCurrentCell = nullptr;  }
   void ResetPossibleValues(unsigned short val);
   
   //Events
   void OnMouseDown(double posX, double posY);
   void OnKey(int key, bool bSolving);
   
   void LoadBoard(unsigned char data[81]);
   SolveState SolveStep();

   BoardAssumption CreateAssumption() const;

   BoardAssumption GetAssumption() const { return m_Assumption; }
   void SetAssumption(const BoardAssumption& assumption) { m_Assumption = assumption; }

   void SetMiss(unsigned char c) { m_nMiss = c; }
private:
	//the nCount stores the number of possible occurences for a particular number. The x,y coordinates store the Cell location. If the count is 1, then Cell[x][y] must contain that digit (and that digit is given by the index position in the array)
	struct Pair
	{
		unsigned char nCount;
		unsigned char nx;
		unsigned char ny;
	};

	void EliminatePossibleValues();//eliminates the possibilities of each cell
	void SetUniqueCellValue();//Set the value of a block if the number can only exist in that block

private:
	bool IsSolved() const;
	bool CheckContradiction() const;
	//The pairs stores the number of places where a particular number is possible within a row/column/block. So in an entire row if the number 9 can exist in only one place, then that cell must contain a 9. (consequently the nCount would be one.) The index into the array stores the digit value (Its like a map with the digit as the key). So the pair with value nCount = 2 would be stored at index 8 (because the value of the number was 9 in the example)
	//void CalculateUniqueCells(int x, int y, Pair pairs[]);
private:
   Cell m_Cells[81];
   
   //For highlighting the current cell
   Cell* m_pCurrentCell;
   int m_nSelectedX;
   int m_nSelectedY;

   //stores the assumption of the CURRENT board
   BoardAssumption m_Assumption;

   unsigned char m_nMiss;	//This stores the number of times that nothing was evaluated by the solveStep... It gets reset if a number was deduced. If it gets too high then make an assumption and continue solving

   //The boardManager manages the addition and removal of boards. When no more guesses can be made, then a new board is created (copy of the original) and an assumption is made. The link list has the following structure (new board -> original -> nullptr).  
   Board* m_nextBoard;
};