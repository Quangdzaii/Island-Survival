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

        // Tắt scale mờ
        SDL_SetTextureScaleMode(texture, SDL_ScaleModeNearest);

       position.x = static_cast<int>(xpos);
       position.y = static_cast<int>(ypos);

       srcRect.x = srcX;
       srcRect.y = srcY;
       srcRect.w = srcRect.h = tsize;

       position.x = static_cast<int>(xpos);
       position.y = static_cast<int>(ypos);
       destRect.w = destRect.h = static_cast<int>(tsize * tscale) + 1;
    }

    void update() override {
       destRect.x = static_cast<int>(position.x - Game::camera.x);
       destRect.y = static_cast<int>(position.y - Game::camera.y);
    }

    void draw() override {
       TextureManager::Draw(texture, srcRect, destRect, SDL_FLIP_NONE);
    }
};
