#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include "spdlog/fmt/ostr.h"

namespace Log
{
   extern std::shared_ptr <spdlog::logger> g_CoreLogger;

   void Init();
}


//Core log macros
#define LOG_TRACE(...)       ::Log::g_CoreLogger->trace(__VA_ARGS__)
#define LOG_INFO(...)        ::Log::g_CoreLogger->info(__VA_ARGS__)
#define LOG_WARN(...)        ::Log::g_CoreLogger->warn(__VA_ARGS__)
#define LOG_ERROR(...)       ::Log::g_CoreLogger->error(__VA_ARGS__)
#define LOG_FATAL(...)       ::Log::g_CoreLogger->critical(__VA_ARGS__)

#define ASSERT(x, ...)   if (!(x)) { LOG_ERROR(__VA_ARGS__); __debugbreak(); __debugbreak(); }


#define glcall(x) GLClearErrorMacro();\
                  x;\
                  ASSERT(GLCheckErrorMacro(), "OpenGl Error: Line({0}), Command({1}), File({2})", __LINE__, #x, __FILE__)


#define GlClearError()  GLClearErrorMacro()
#define GlCheckError()  ASSERT(GLCheckErrorMacro(), "OpenGl Error: Line({0}), File({1})", __LINE__, __FILE__)


void GLClearErrorMacro();
bool GLCheckErrorMacro();