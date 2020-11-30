#pragma once
#include "glm.hpp"


namespace MathR
{
   double Lerp(double min, double max, double percent);
   double Clamp(double min, double max, double value);
   double Clamp01(double value);
   double GetPercent(double min, double max, double value);

   glm::vec4 LerpColor(const glm::vec4& col1, const glm::vec4& col2, double percent);
}