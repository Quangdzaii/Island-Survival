#include "Game.hpp"

Game *game = nullptr;

// argc: Số lượng tham số khi chạy chương trình | argv[]: Danh sách các tham số dưới dạng chuỗi
int main(int argc, char *argv[]) {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    game = new Game();
    game -> init("Island Survival", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 720, false);

    while (game -> running()) {
        frameStart = SDL_GetTicks();

        game -> handleEvents();
        game -> update();
        game -> render();

        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    game -> clean();
    return 0;
}
