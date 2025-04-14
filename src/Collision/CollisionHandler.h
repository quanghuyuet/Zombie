#pragma once

#include <SDL.h>
#include <vector>
#include "Map.h"

class CollisionHandler {
public:
    static CollisionHandler* GetInstance();
    bool MapCollision(SDL_Rect a);
    bool CheckCollision(SDL_Rect a, SDL_Rect b);
    ~CollisionHandler();

private:
    CollisionHandler();
    std::vector<std::vector<int>> m_CollisionTilemap;
    static CollisionHandler* s_Instance;
};
