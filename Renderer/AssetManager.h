#include <unordered_map>
#include "Log/Log.h"

struct Texture
{
   unsigned int id;
   int width;
   int height;
};
class AssetManager
{
public:
   static Texture* LoadTexture(const char* const filePath, int slot);
   static Texture* LoadTexture(int x, int y, unsigned char* buff, int slot);

   static Texture* GetTexture(int n) { ASSERT(n < m_nCount, "Array out of bounds"); return m_textures[n]; }
   static void OnExit();

private:
   static Texture* m_textures[5];
   static int m_nCount;
};