#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "GL/glew.h"

namespace Log
{
   std::shared_ptr <spdlog::logger> g_CoreLogger;

   void Init()
   {
      spdlog::set_pattern("%^[%T] %n: %v%$");
      g_CoreLogger = spdlog::stdout_color_mt("Core");
      g_CoreLogger->set_level(spdlog::level::trace);
   }
}

void GLClearErrorMacro()
{
   
   while (glGetError() != GL_NO_ERROR);
}

//returns false if an error occurred
bool GLCheckErrorMacro()
{
   return glGetError() == GL_NO_ERROR;
}