#pragma once
#include "Game.hpp"

class GameObject {
public:
    GameObject(const char*texturesheet, int x, int y);
    ~GameObject();

private:
    int xpos;
    int ypos;

    SDL_Texture* objTexture;
    SDL_Rect srcRect, destRect;
};
