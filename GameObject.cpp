#include "GameObject.hpp"
#include"TextureManager.hpp"

GameObject::GameObject(const char* texturesheet, int x, int y) {
   objTexture = TextureManager::LoadTexture(texturesheet);

   xpos = x;
   ypos = y;
}
