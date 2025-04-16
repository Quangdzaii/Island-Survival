#include "Map.hpp"
#include "Game.hpp"
#include <fstream>
#include "ECS.hpp"
#include "Components.hpp"

using namespace std;

extern Manager manager;

Map::Map(const char* mfp, float ms, int ts) : mapFilePath(mfp), mapScale(ms), tileSize(ts) {
    scaledSize = ms * ts;
}

Map::~Map() {

}

void Map::LoadMap(string path, int sizeX,int sizeY) {
    char c;
    fstream mapFile;
    mapFile.open(path);

    int srcX, srcY;

    for(int y = 0; y < sizeY; y++) {
        for(int x = 0; x < sizeX; x++) {
            mapFile.get(c);
            srcY = atoi(&c) * tileSize; // atoi: chuyển kí tự số ('1') thành số nguyên (1)
            mapFile.get(c);
            srcX = atoi(&c) * tileSize;
            AddTile(srcX, srcY, static_cast<int>(x * scaledSize), static_cast<int>(y * scaledSize));
            mapFile.ignore();
        }
    }

    mapFile.ignore();

    for(int y = 0; y < sizeY; y++) {
        for(int x = 0; x < sizeX; x++) {
            mapFile.get(c);
            if(c == '1') {
                auto& tcol(manager.addEntity());
                tcol.addComponent<ColliderComponent>("terrain", static_cast<int>(x * scaledSize), static_cast<int>(y * scaledSize), static_cast<int>(scaledSize));
                tcol.addGroup(Game::groupColliders);
            }
            mapFile.ignore();
        }
    }

    mapFile.close();
}

void Map::AddTile(int srcX, int srcY, int xpos, int ypos) {
    auto& tile(manager.addEntity());
    tile.addComponent<TileComponent>(srcX, srcY, static_cast<int>(xpos), static_cast<int>(ypos), tileSize, mapScale, mapFilePath);
    tile.addGroup(Game::groupMap);
}
