//Cơ chế header guard để ngăn việc file header (Game.hpp) bị include nhiều lần trong cùng một chương trình
#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>

using namespace std;



class Game {
// Phần có thể truy cập từ bên ngoài
public:
    Game(); // Khởi động Game
    ~Game(); // Tắt Game

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

    void handleEvents(); // Xử lí tác động của người chơi(click chuột, bấm phím)
    void update(); // Cập nhật những hoạt động diễn ra trong game
    bool running() {return isRunning;}
    void render(); // Xử lí hình ảnh hiển thị trên màn hình
    void clean(); //Giải phóng bộ nhớ khi kết thúc game

    void restartGame();

    void logErrorAndExit(const char* msg, const char* sdlError) {
       cerr << msg << sdlError << endl;
       exit(1);
    }

    static SDL_Renderer* renderer;
    static SDL_Event event;
    static bool isRunning;
    static SDL_Rect camera;
    static Mix_Music* backgroundMusic;
    static Mix_Chunk* tickSound;
    static TTF_Font* font;
    static int score;

    enum groupLabels : size_t {
    groupMap,
    groupCharacters,
    groupColliders,
    groupSafeZones
};

// Phần chỉ có thể truy cập trong class;
private:
    int count = 0; // Bộ đếm hỗ trợ cho hàm update
    SDL_Window *window;
};

