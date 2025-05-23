#pragma once
#include "ECS.hpp"
#include <SDL.h>

class TileComponent : public Component {
public:
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;
    Vector2D position;

    TileComponent() = default;

    ~TileComponent() {
       SDL_DestroyTexture(texture);
    }

    TileComponent(int srcX, int srcY, int xpos, int ypos, int tsize, float tscale, const char* path) {
       texture = TextureManager::LoadTexture(path);

       position.x = xpos;
       position.y = ypos;

       srcRect.x = srcX;
       srcRect.y = srcY;
       srcRect.w = srcRect.h = tsize;

       destRect.w = destRect.h = static_cast<int>(tsize * tscale) + 1;
    }

    void update() override {
       destRect.x = position.x - Game::camera.x;
       destRect.y = position.y - Game::camera.y;
    }

    void draw() override {
       TextureManager::Draw(texture, srcRect, destRect, SDL_FLIP_NONE);
    }
};
