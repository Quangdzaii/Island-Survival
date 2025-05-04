#pragma once
#include "ECS.hpp"
#include "TransformComponent.hpp"
#include <SDL.h>

class CharacterComponent : public Component {
public:
    int maxHP, currentHP;
    TransformComponent* transform;

    CharacterComponent() : maxHP(100), currentHP(100) {}

    void init() override {
        if(!entity -> hasComponent<TransformComponent>()) {
            entity -> addComponent<TransformComponent>();
        }
        transform = &entity -> getComponent<TransformComponent>();
    }

    void drawHealthBar(SDL_Renderer* renderer, SDL_Rect& camera) {
        int barWidth = transform -> width * transform -> scale;
        int barHeight = 8;

        int screenX = static_cast<int>(transform -> position.x) - camera.x;
        int screenY = static_cast<int>(transform -> position.y) - camera.y;

        int barX = screenX;
        int barY = screenY - barHeight - 5; // phía trên đầu nhân vật

        float hpPercent = static_cast<float>(currentHP) / maxHP;

        SDL_Rect bg = {barX, barY, barWidth, barHeight};//Nền
        SDL_Rect hp = {barX, barY, static_cast<int>(barWidth * hpPercent), barHeight};//Máu

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); //Màu xám tối
        SDL_RenderFillRect(renderer, &bg);//Vẽ nền máu

        // Chọn màu thanh máu theo %HP
        if (hpPercent > 0.5f) {
           SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Xanh
        }
        else if (hpPercent > 0.25f) {
           SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Cam
        }
        else {
           SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Đỏ
        }

        // Vẽ thanh máu
        SDL_RenderFillRect(renderer, &hp);

        // Vẽ viền đen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &bg);
    }
};

