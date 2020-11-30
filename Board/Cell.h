#pragma once
#include "Log/Log.h"

extern glm::vec4 g_rectBoard;

class Cell
{
public:
   Cell();
   Cell(const Cell& cell);

   //High Level Get, Set value functions
   inline bool GetPossible(int digit) const { 
      ASSERT(digit >= 1 && digit <= 9, "Invalid digit"); 
      return m_nPossibleValues & (1 << (digit - 1));
   }
   inline void SetPossible (int digit, bool state) { 
      ASSERT(digit >= 1 && digit <= 9, "Invalid digit"); 
      if (state) m_nPossibleValues |= 1 << (digit - 1);
      else       m_nPossibleValues &= ~(1 << (digit - 1));
   }

   //Low Level Get, Set... Use only if you know what youre doing
   inline unsigned short GetPossitbleL() const { return m_nPossibleValues; }
   inline void SetPossibleL(unsigned short sPossible) { m_nPossibleValues = sPossible; }

   //can remove one or multiple possibilities in one go
   inline void RemovePossibleL(unsigned short sPossible) {
       m_nPossibleValues &= ~sPossible; 
       int test = 1;
   }

   inline unsigned char GetValue() const { return m_nValue; }
   inline void SetValue(unsigned char c) { m_nValue = c; }

   void OnRender(int x, int y);
   void OnRenderSelected(int x, int y);
   void OnRenderSubNumbers(int x, int y);
   

   void Clear();

   //if theres only one possibility then set it
   int UpdateValue();

   unsigned char GetNumPossibilities() const;

private:
private:
   //bitmask... LSB stores digit 1. Value at that bit is true if its value is possible
   unsigned short m_nPossibleValues;
   unsigned char m_nValue;  //stores 0 if it is empty 
};