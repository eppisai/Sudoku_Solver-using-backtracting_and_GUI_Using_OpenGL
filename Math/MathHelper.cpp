#include "MathHelper.h"

namespace MathR
{
   double Lerp(double min, double max, double percent)
   {
      return min + (max - min) * percent;
   }
   double Clamp(double min, double max, double value)
   {
      if (value < min)        return min;
      else if (value > max)   return max;
      else                    return value;
   }
   double Clamp01(double value)
   {
      return Clamp(0.0, 1.0, value);
   }
   double GetPercent(double min, double max, double value)
   {
      return (value - min) / (max - min);
   }
   glm::vec4 LerpColor(const glm::vec4& col1, const glm::vec4& col2, double percent)
   {
      glm::vec4 col;
      col.r = Lerp(col1.r, col2.r, percent);
      col.g = Lerp(col1.g, col2.g, percent);
      col.b = Lerp(col1.b, col2.b, percent);
      col.a = Lerp(col1.a, col2.a, percent);
      return col;
   }
}