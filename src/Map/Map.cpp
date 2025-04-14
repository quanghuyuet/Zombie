#include "Map.h"
#include <iostream>
#include <fstream>
#include <SDL_image.h>
#include <SDL.h>

Map* Map::s_Instance = nullptr;
Map::Map() : m_Renderer(nullptr), m_TilesetTexture(nullptr) {
    m_TileSize = TILE_SIZE; // Lấy từ Camera.h


    std::ifstream file("img/Map/Map_Dark.txt");
    if (!file.is_open()) {
        cout << "Khong mo duoc file map.txt!" << endl;
        m_Width = 0;
        m_Height = 0;
        return;
    }

    // Đọc kích thước map
    file >> m_Width >> m_Height;

    // Khởi tạo m_TileMap với kích thước đọc được
    m_TileMap.resize(m_Height, vector<int>(m_Width));

    // Đọc dữ liệu tile map
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            if (file.eof()) {
               cout << "File map.txt khong du du lieu!" <<endl;
                break;
            }
            file >> m_TileMap[y][x];
        }
    }

    file.close();
}

void Map::Init(SDL_Renderer* renderer) {
    m_Renderer = renderer;

    // Load tileset
      SDL_Surface* tilesetSurface = IMG_Load("img/Map/0909.png");
         if (!tilesetSurface) {
        cout << "Khong load duoc tileset: " << SDL_GetError() << endl;
        return;
    }
    m_TilesetTexture = SDL_CreateTextureFromSurface(m_Renderer, tilesetSurface);
    if (!m_TilesetTexture) {
        std::cout << "Khong tao duoc texture: " << SDL_GetError() << std::endl;
    }
    SDL_FreeSurface(tilesetSurface);
}

void Map::Clean() {
    SDL_DestroyTexture(m_TilesetTexture);
    m_TilesetTexture = nullptr;
    m_Renderer = nullptr;
    delete s_Instance;
    s_Instance = nullptr;
}

void Map::Render() {
    // Lấy thông tin từ camera
    Vector2D cameraPos = Camera::GetInstance()->GetPosition();
    SDL_Rect viewBox = Camera::GetInstance()->GetViewBox();

    // Tính toán tile bắt đầu và kết thúc để vẽ (dựa trên camera)
    int startX = cameraPos.X / m_TileSize;
    int startY = cameraPos.Y / m_TileSize;
    int endX = (cameraPos.X + viewBox.w) / m_TileSize + 1;
    int endY = (cameraPos.Y + viewBox.h) / m_TileSize + 1;

    // Giới hạn trong phạm vi map
    startX = max(0, startX);
    startY = max(0, startY);
    endX = min(m_Width, endX);
    endY = min(m_Height, endY);

    // Vẽ các tile trong vùng nhìn thấy
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            int tileID = m_TileMap[y][x];
            if (tileID == 0) continue;
            // Tính toán vị trí vẽ trên màn hình (đã trừ offset của camera)
            SDL_Rect srcRect = {0, 0, m_TileSize, m_TileSize};
            SDL_Rect dstRect = {x * m_TileSize - static_cast<int>(cameraPos.X), y * m_TileSize - static_cast<int>(cameraPos.Y), m_TileSize, m_TileSize};
            SDL_RenderCopy(m_Renderer, m_TilesetTexture, &srcRect, &dstRect);
        }
    }
}

void Map::Update() {
    // Nếu có logic cập nhật (như animation), thêm vào đây
}

int Map::GetTileID(int x, int y) const {
    if (x >= 0 && x < m_Width && y >= 0 && y < m_Height) {
        return m_TileMap[y][x];
    }
    return 0;
}

const vector<vector<int>>& Map::GetTileMap() const {
    return m_TileMap;
}

Map::~Map() {
    SDL_DestroyTexture(m_TilesetTexture);
}
