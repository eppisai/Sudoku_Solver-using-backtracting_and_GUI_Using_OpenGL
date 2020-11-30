
#include "FileManager.h"
#include <fstream>
#include "Log/Log.h"

namespace String
{
   static bool strcmpi(const char* const a, const char* const b, int n)
   {
      for (int i = 0; i < n; i++)
      {
         if (a[i] != b[i])
         {
            return false;
         }

         if ((a[i] == '\0') || (b[i] == '\0'))
         {
            return ((a[i] == '\0') && (b[i] == '\0'));
         }
      }
      return true;
   }
}
namespace FileManager
{


   std::string ReadFile(const char* const path, bool bAddEndLineChar) {
      std::ifstream file;
      file.open(path);
      if (!file.is_open()) { LOG_WARN("Could not find file at path: {0}", path); return ""; }

      const int bufSize = 300;
      char buff[bufSize];
      std::string strFile;
      strFile.reserve(2000);
      
      while (!file.eof())
      {
         file.getline(buff, bufSize);
         strFile += buff;
         if (bAddEndLineChar && !file.eof()) strFile += '\n';
      }

      file.close();
      return strFile;
   }

   void ParseShader(const char* const path, std::string outShader[2])
   {
      std::ifstream file;
      file.open(path);
      if (!file.is_open()) { LOG_WARN("Could not find file at path: {0}", path); return; }

      const int bufSize = 300;
      
      char buff[bufSize];
      std::string* pCurrentShader = nullptr;
      
      for (int i = 0; i < 2; outShader[i].reserve(2000), i++);

      while (!file.eof())
      {
         file.getline(buff, bufSize);
         
         if (String::strcmpi(buff, "#shader", 7))
         {
            if (String::strcmpi(buff + 8, "vertex", 6))
            {
               pCurrentShader = outShader + e_VertexShaderIndex;
            }
            else if (String::strcmpi(buff + 8, "fragment", 8))
            {
               pCurrentShader = outShader + e_FragmentShaderIndex;
            }
            else ASSERT(false, "Error: Invalid shader mode");
         }
         else if (pCurrentShader)
         {
            pCurrentShader->append(buff);
            pCurrentShader->push_back('\n');
         }
         else
         {
            ASSERT(false, "Error: Current Shader is null");
         }
      }

      file.close();
   }
}