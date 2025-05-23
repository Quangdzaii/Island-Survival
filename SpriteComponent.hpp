#pragma once
#include "Components.hpp"
#include <SDL.h>
#include "TextureManager.hpp"
#include "Animation.hpp"
#include <map>//Duyệt mảng qua key(ko cân là số nguyên)

using namespace std;

class SpriteComponent : public Component {
public:
    int animIndex = 0;

    map<const char*, Animation> animations;

    SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

    SpriteComponent() = default;
    SpriteComponent(const char* path) {
        setTex(path);
    }

    SpriteComponent(const char* path, bool isAnimated) {
        animated = isAnimated;

        Animation idle = Animation(0, 3, 100);
        Animation walk = Animation(1, 8, 100);

        animations.emplace("Idle", idle);
        animations.emplace("Walk", walk);

        Play("Idle");

        setTex(path);
    }

    ~SpriteComponent() {
        SDL_DestroyTexture(texture);
    }

    void setTex(const char* path) {
        texture = TextureManager::LoadTexture(path);
    }

    void init() override {
        transform = &entity -> getComponent<TransformComponent>();

        srcRect.x = srcRect.y = 0;
        srcRect.w = transform -> width;
        srcRect.h = transform -> height;
    }

    void update() override {
        if(animated) {
            srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
        }

        srcRect.y = animIndex * transform -> height;

        destRect.x = transform -> position.x - Game::camera.x;
        destRect.y = transform -> position.y - Game::camera.y;
        destRect.w = transform -> width * transform -> scale;
        destRect.h = transform -> height * transform -> scale;
    }

    void draw() override {
        TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
    }

    void Play(const char* animName) {
        frames = animations[animName].frames;
        animIndex = animations[animName].index;
        speed = animations[animName].speed;
    }

private:
    TransformComponent *transform;
    SDL_Texture *texture;
    SDL_Rect srcRect, destRect;

    bool animated = false;
    int frames = 0;
    int speed = 100;
};
