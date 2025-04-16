#pragma once
#include "ECS.hpp"
#include <SDL.h>

class SafeZoneComponent : public Component {
public:
    SDL_Rect area;
    Uint32 createdTime;
    Uint32 duration;
    SDL_Color color;
    int zoneId;
    bool scored = false;

    SafeZoneComponent(int x, int y, int w, int h, Uint32 durationMs, int id)
                      : duration(durationMs), zoneId(id){
        area = {x, y, w, h};
        createdTime = SDL_GetTicks();
        color = {255, 255, 255, 255};
    }

    bool isExpired(Uint32 currentTime) const {
        return (currentTime - createdTime) >= duration;
    }

    bool shouldRender(Uint32 currentTime) const {
        Uint32 elapsed = currentTime - createdTime;
        Uint32 remaining = (duration > elapsed) ? (duration - elapsed) : 0;

        // Nhấp nháy nếu gần hết thời gian
        if (remaining < 1000) {
            return ((currentTime / 200) % 2 == 0);
        }
        return true;
    }

    void render(SDL_Renderer* renderer, Uint32 currentTime, SDL_Rect& camera) {
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
