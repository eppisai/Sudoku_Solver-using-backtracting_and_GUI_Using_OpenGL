#pragma once
#include <string>

enum : unsigned int
{
   e_VertexShaderIndex = 0,
   e_FragmentShaderIndex = 1
};
namespace FileManager
{
   extern std::string ReadFile(const char* const path, bool bAddEndLineChar);
   extern void ParseShader(const char* const path, std::string outShader[2]);
}