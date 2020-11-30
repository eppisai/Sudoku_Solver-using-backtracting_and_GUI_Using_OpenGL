#include "pch.h"
#include "AssetManager.h"
#include <stb_image.h>

Texture* AssetManager::m_textures[5];
int AssetManager::m_nCount = 0;

Texture* AssetManager::LoadTexture(int x, int y, unsigned char* buff, int slot)
{
   Texture* pTexture = new Texture;
   pTexture->height = y;
   pTexture->width = x;
   glcall(glGenTextures(1, &pTexture->id));
   glcall(glActiveTexture(GL_TEXTURE0 + slot));
   glcall(glBindTexture(GL_TEXTURE_2D, pTexture->id));

   glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
   glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
   glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
   glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

   glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buff));

   glcall(glActiveTexture(GL_TEXTURE0 + slot));
   glcall(glBindTexture(GL_TEXTURE_2D, pTexture->id));

   m_textures[m_nCount] = pTexture;
   m_nCount++;
   return pTexture;
}
Texture* AssetManager::LoadTexture(const char* const filePath, int slot)
{
   stbi_set_flip_vertically_on_load(1);
   int x, y, bpp;
   unsigned char* buffer = stbi_load(filePath, &x, &y, &bpp, 4);
     
   Texture* ret = LoadTexture(x, y, buffer, slot);
   if (buffer)
   {
      stbi_image_free(buffer);
   }
   return ret;
}

void AssetManager::OnExit()
{
   for (int i = 0; i < m_nCount; i++)
   {
      delete m_textures[i];
   }
}
