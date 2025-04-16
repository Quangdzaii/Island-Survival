#pragma once
#include <string>

using namespace std;

class Map {
public:
    Map(const char* mfp, float ms, int ts);
    ~Map();

    void LoadMap(string path, int sizeX, int sizeY);
    void AddTile(int srcX, int srcY, int xpos, int ypos);

private:
    const char* mapFilePath;
    float mapScale;
    int tileSize;
    float scaledSize;
};
