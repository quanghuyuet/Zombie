#pragma once

#include <SDL.h>
#include <vector>
#include "Camera.h"
using namespace std;

class Map {
public:
    static Map* GetInstance() {
        return Map::s_Instance = (Map::s_Instance != nullptr) ? Map::s_Instance : new Map();
    }
    void Init(SDL_Renderer* renderer);
    void Render();
    void Update();
    int GetTileID(int x, int y) const;
    const vector<vector<int>>& GetTileMap() const;
    void Clean();

private:
    Map();
    ~Map();
    int m_Width;
    int m_Height;
    int m_TileSize;
    vector<vector<int>> m_TileMap;
    SDL_Renderer* m_Renderer;
    SDL_Texture* m_TilesetTexture;
    static Map* s_Instance;
};
