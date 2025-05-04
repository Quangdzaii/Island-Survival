#include "Game.hpp"
#include "TextureManager.hpp"
#include "Map.hpp"
#include "Components.hpp"
#include "Vector2D.hpp"
#include "Collision.hpp"

using namespace std;

const int mapWidth = 25;
const int mapHeight = 20;
int walkableMap[mapHeight][mapWidth]; // 0 = đi được, 1 = bị chặn

int Game::score = 0;

Map* gameMap;
Manager manager;
Entity* currentZonePtr = nullptr;

Mix_Chunk* Game::tickSound = nullptr;
Mix_Music* Game::backgroundMusic = nullptr;
TTF_Font* Game::font = nullptr;
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event; // Biến lưu trữ tác động

SDL_Rect Game::camera = {0, 0, 800, 640};

bool inSafeZone = false;

bool Game::isRunning = false;

static const void* lastZonePtr = nullptr; // lưu SafeZone cuối cùng đã ở trong
static bool wasInSafeZone = false;
int currentZoneId = 0; // Tăng dần cho mỗi vùng SafeZone tạo ra

bool damageCountdownStarted = false;
size_t lastSafeZoneTime = 0;
size_t damageStartTime = 0;
size_t lastDamageTime = 0; // Thời điểm lần trừ máu gần nhất
size_t damageInterval = 1000; // Trừ máu mỗi 1 giây
size_t safeZoneInterval = 15000; // Mỗi 15 giây tạo vùng an toàn
size_t lastHealTime = 0;

auto& character(manager.addEntity());

auto& tiles(manager.getGroup(Game::groupMap));
auto& characters(manager.getGroup(Game::groupCharacters));
auto& colliders(manager.getGroup(Game::groupColliders));

Game::Game()
{}
Game::~Game()
{}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
     int flags = 0; // Cửa sổ ban đầu bình thường

     if(fullscreen) {
         flags = SDL_WINDOW_FULLSCREEN;
}
     if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
         window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
         renderer = SDL_CreateRenderer(window, -1, 0); //(cửa sổ sẽ vẽ lên, chọn card đồ họa tốt nhất, flags)
         if(renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Đặt màu vẽ cho renderer
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
      logErrorAndExit( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    backgroundMusic = Mix_LoadMUS("music.mp3");
    Mix_PlayMusic(backgroundMusic, -1); // -1: phát lặp vô hạn
    tickSound = Mix_LoadWAV("safezone.wav");

    if(TTF_Init() == -1) {
      logErrorAndExit("SDL_ttf could not initialize! SDL_ttf Error: ", TTF_GetError());
    }
    font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24); // Gán vào Game::font

    isRunning = true;
}

SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // Ngăn filter mờ


gameMap = new Map("terrain_ss.png", 2.3, 32);

gameMap -> LoadMap("map.map", 25, 20);

character.addComponent<TransformComponent>(165, 1200, 32, 32, 2);
character.addComponent<SpriteComponent>("character_anims.png", true);
character.addComponent<KeyboardController>();
character.addComponent<ColliderComponent>("character");
character.addComponent<CharacterComponent>();
character.addGroup(groupCharacters);

// Tạo SafeZone ngay khi bắt đầu game
int initialX = 1200;
int initialY = 640;

// Tạo 1 vùng SafeZone 100x100 tại vị trí (1200, 640)
auto& zone = manager.addEntity();
zone.addComponent<SafeZoneComponent>(initialX, initialY, 100, 100, 15000, currentZoneId++); // tồn tại 15 giây
zone.addGroup(groupSafeZones);
}

void Game::handleEvents() {
SDL_PollEvent(&event); // Truyền địa chỉ của event vào biến
switch(event.type) {
    case SDL_QUIT:
       isRunning = false;
       break;
    default:
       break;
}
}

bool isAreaFreeOfColliders(int x, int y, int w, int h) {
    SDL_Rect zoneRect = {x, y, w, h};
    for(auto& c : colliders) {
        SDL_Rect rect = c -> getComponent<ColliderComponent>().collider;
        if(SDL_HasIntersection(&zoneRect, &rect)) {
            return false; // Có va chạm
        }
    }
    return true;
}

void Game::restartGame() {
    // Reset lại vị trí và máu của nhân vật
    character.getComponent<TransformComponent>().position = Vector2D(165, 1200);
    character.getComponent<CharacterComponent>().currentHP = 100;

    // Reset camera theo vị trí mới
    camera.x = character.getComponent<TransformComponent>().position.x - 400;
    camera.y = character.getComponent<TransformComponent>().position.y - 320;

    if(camera.x < 0) {camera.x = 0;}
    if(camera.y < 0) {camera.y = 0;}
    if(camera.x > camera.w) {camera.x = camera.w;}
    if(camera.y > camera.h) {camera.y = camera.h;}

    // Xóa tất cả SafeZone cũ
    auto& safeZones = manager.getGroup(groupSafeZones);
    for(auto& zone : safeZones) {
        zone -> destroy();
    }

    // Tạo lại SafeZone ban đầu
    int initialX = 1200;
    int initialY = 640;

    auto& zone = manager.addEntity();
    zone.addComponent<SafeZoneComponent>(initialX, initialY, 100, 100, 15000, currentZoneId++);
    zone.addGroup(groupSafeZones);

    Mix_PlayMusic(backgroundMusic, -1);

    Game::score = 0;

    // Đặt lại các biến thời gian
    lastSafeZoneTime = SDL_GetTicks();
    lastDamageTime = SDL_GetTicks();
}

void drawScore(SDL_Renderer* renderer, int score) {
    SDL_Color white = {255, 255, 255};
    string text = "Score: " + to_string(Game::score);

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Game::font, text.c_str(), white);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect = {10, 10, surfaceMessage -> w, surfaceMessage -> h};
    SDL_RenderCopy(renderer, message, nullptr, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void Game::update() {
   SDL_Rect characterCol = character.getComponent<ColliderComponent>().collider;
   Vector2D characterPos = character.getComponent<TransformComponent>().position;

   manager.refresh();
   manager.update();

   for(auto& c : colliders) {
    SDL_Rect cCol = c -> getComponent<ColliderComponent>().collider;
    if(Collision::AABB(cCol, characterCol)) {
        character.getComponent<TransformComponent>().position = characterPos;
    }
   }

   camera.x = character.getComponent<TransformComponent>().position.x - 400;
   camera.y = character.getComponent<TransformComponent>().position.y - 320;

   if(camera.x < 0) {
    camera.x = 0;
   }
   if(camera.y < 0) {
    camera.y = 0;
   }
   if(camera.x > camera.w) {
    camera.x = camera.w;
   }
   if(camera.y > camera.h) {
    camera.y = camera.h;
   }

   size_t currentTime = SDL_GetTicks();

// Tạo safezone mới mỗi vài giây
if(currentTime - lastSafeZoneTime >= safeZoneInterval) {
    lastSafeZoneTime = currentTime;

    bool placed = false;
    int attempts = 0;

    while(!placed && attempts < 20) {  // thử tối đa 20 lần
        int randX = rand() % (25 * 32 * 2 - 100); // map width
        int randY = rand() % (20 * 32 * 2 - 100); // map height

        if(isAreaFreeOfColliders(randX, randY, 100, 100)) {
          auto& zone = manager.addEntity();
          zone.addComponent<SafeZoneComponent>(randX, randY, 100, 100, 15000, currentZoneId++);
          zone.addGroup(groupSafeZones);
          placed = true;
        }
        attempts++;
    }
}

// Xóa vùng hết giờ
auto& safeZones = manager.getGroup(groupSafeZones);
for(auto& zone : safeZones) {
    auto& comp = zone -> getComponent<SafeZoneComponent>();
    if(comp.isExpired(currentTime)) {
        zone -> destroy();
    }
}

// Kiểm tra xem nhân vật có đứng trong vùng không
SDL_Rect characterRect = {
    static_cast<int>(character.getComponent<TransformComponent>().position.x),
    static_cast<int>(character.getComponent<TransformComponent>().position.y),
    character.getComponent<TransformComponent>().width * character.getComponent<TransformComponent>().scale,
    character.getComponent<TransformComponent>().height * character.getComponent<TransformComponent>().scale
};

inSafeZone = false;
for(auto& zone : safeZones) {
    if(zone -> getComponent<SafeZoneComponent>().contains(characterRect)) {
        inSafeZone = true;
        currentZonePtr = zone;

        auto& comp = zone->getComponent<SafeZoneComponent>();
         if(!comp.scored) {
            Game::score += 1; // Cộng điểm 1 lần duy nhất
            Mix_PlayChannel(-1, tickSound, 0);
            comp.scored = true; // Đánh dấu đã cộng
        }
        break;
    }
}

if(!inSafeZone) {
    if(currentTime - lastDamageTime >= damageInterval) {
        character.getComponent<CharacterComponent>().currentHP -= 3;
        lastDamageTime = currentTime;
    }
    lastZonePtr = nullptr; // reset để lần sau vào lại sẽ tính điểm
}
else {
    // Nếu đang đứng trong SafeZone thì reset lại thời gian
    if(currentTime > lastHealTime + 1000) {
      character.getComponent<CharacterComponent>().currentHP += 1;
    if(character.getComponent<CharacterComponent>().currentHP >= character.getComponent<CharacterComponent>().maxHP) {
        character.getComponent<CharacterComponent>().currentHP = character.getComponent<CharacterComponent>().maxHP;
      }
      lastHealTime = currentTime;
    }
    wasInSafeZone = true;
}

if(character.getComponent<CharacterComponent>().currentHP <= 0) {
    Mix_HaltMusic();
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Play agian" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Exit" },
    };

    const SDL_MessageBoxColorScheme colorScheme = {
        {
            { 255,   0,   0 }, // SDL_MESSAGEBOX_COLOR_BACKGROUND
            { 255, 255, 255 }, // SDL_MESSAGEBOX_COLOR_TEXT
            {   0,   0,   0 }, // SDL_MESSAGEBOX_COLOR_BUTTON_BORDER
            { 255,   0,   0 }, // SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND
            { 255, 255, 255 }  // SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED
        }
    };

    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        NULL,
        "Game Over",
        "You Lost",
        SDL_arraysize(buttons),
        buttons,
        &colorScheme
    };

    int buttonid;
    SDL_ShowMessageBox(&messageboxdata, &buttonid);

    if(buttonid == 1) {
        // Chơi lại
        restartGame(); // <-- Sẽ tạo hàm này ngay dưới đây
    } else {
        // Thoát
        isRunning = false;
    }
}
}

void Game::render() {
   SDL_RenderClear(renderer); // Xóa toàn bộ những gì đã vẽ (lấp đầy bằng màu trắng với hàm SDL_SetRenderDrawColor/line 21)

   for(auto& t : tiles) {
      t -> draw();
   }

   for(auto& c : characters) {
      c -> draw();
   }

   character.getComponent<CharacterComponent>().drawHealthBar(renderer, camera);

   auto& safeZones = manager.getGroup(groupSafeZones);
   for(auto& zone : safeZones) {
      zone -> getComponent<SafeZoneComponent>().render(renderer, SDL_GetTicks(), camera);
   }

   drawScore(renderer, Game::score);

   SDL_RenderPresent(renderer);
}

void Game::clean() {
   SDL_DestroyWindow(window);
   SDL_DestroyRenderer(renderer);
   SDL_Quit();

   if(tickSound) {
     Mix_FreeChunk(tickSound);
     tickSound = nullptr;
   }

   if(backgroundMusic) {
     Mix_FreeMusic(backgroundMusic);
     backgroundMusic = nullptr;
   }
   Mix_CloseAudio();
   Mix_Quit();
   if(font) {
    TTF_CloseFont(font);
    font = nullptr;
   }
   TTF_Quit();
}
