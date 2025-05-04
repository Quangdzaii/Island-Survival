#pragma once
#include "ECS.hpp"
#include <SDL.h>

class SafeZoneComponent : public Component {
public:
    SDL_Rect area;
    size_t createdTime;
    size_t duration;
    SDL_Color color;
    int zoneId;
    bool scored = false;

    SafeZoneComponent(int x, int y, int w, int h, size_t durationMs, int id)
                      : duration(durationMs), zoneId(id){
        area = {x, y, w, h};
        createdTime = SDL_GetTicks();
        color = {255, 255, 255, 255};
    }

    bool isExpired(size_t currentTime) const {
        return (currentTime - createdTime) >= duration;
    }

    bool shouldRender(size_t currentTime) const {
        size_t elapsed = currentTime - createdTime;
        size_t remaining = (duration > elapsed) ? (duration - elapsed) : 0;

        // Nhấp nháy nếu gần hết thời gian
        if (remaining < 1000) {
            return ((currentTime / 200) % 2 == 0);
        }
        return true;
    }

    void render(SDL_Renderer* renderer, size_t currentTime, SDL_Rect& camera) {
        if (!shouldRender(currentTime)) return;

        SDL_Rect screenRect = {
            area.x - camera.x,
            area.y - camera.y,
            area.w,
            area.h
        };

        // Vẽ viền trắng
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &screenRect);
    }

    bool contains(const SDL_Rect& target) const {
        return SDL_HasIntersection(&area, &target);
    }
};
