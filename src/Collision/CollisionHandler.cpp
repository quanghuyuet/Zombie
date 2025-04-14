#include "CollisionHandler.h"

CollisionHandler* CollisionHandler::s_Instance = nullptr;

CollisionHandler::CollisionHandler() {
    // Lấy tile map từ Map
    m_CollisionTilemap = Map::GetInstance()->GetTileMap();
}

CollisionHandler::~CollisionHandler() {
    delete s_Instance;
    s_Instance = nullptr;
}

CollisionHandler* CollisionHandler::GetInstance() {
    if (s_Instance == nullptr) {
        s_Instance = new CollisionHandler();
    }
    return s_Instance;
}

bool CollisionHandler::MapCollision(SDL_Rect a) {
    int tileSize = TILE_SIZE;
    int RowCount = m_CollisionTilemap.size();
    int ColCount = (RowCount > 0) ? m_CollisionTilemap[0].size() : 0;

    int left_tile = a.x / tileSize;
    int right_tile = (a.x + a.w - 1) / tileSize;
    int top_tile = a.y / tileSize;
    int bottom_tile = (a.y + a.h - 1) / tileSize;

    if (left_tile < 0) left_tile = 0;
    if (right_tile >= ColCount) right_tile = ColCount - 1;
    if (top_tile < 0) top_tile = 0;
    if (bottom_tile >= RowCount) bottom_tile = RowCount - 1;

    for (int i = left_tile; i <= right_tile; i++) {
        for (int j = top_tile; j <= bottom_tile; j++) {
            if (m_CollisionTilemap[j][i] > 0) { // Tile rắn
                return true;
            }
        }
    }
    return false;
}

bool CollisionHandler::CheckCollision(SDL_Rect a, SDL_Rect b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}
