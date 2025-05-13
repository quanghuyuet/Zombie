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
    // Constructor riêng, chỉ gọi từ GetInstance() để đảm bảo Singleton.

    ~Map();
    // Destructor để giải phóng tài nguyên khi đối tượng bị hủy.

    int m_Width;
    // Biến lưu chiều rộng của bản đồ (số ô theo chiều ngang).

    int m_Height;
    // Biến lưu chiều cao của bản đồ (số ô theo chiều dọc).

    int m_TileSize;
    // Biến lưu kích thước của mỗi ô (pixel).

    vector<vector<int>> m_TileMap;
    // Mảng 2 chiều lưu ID của các ô trên bản đồ.

    SDL_Renderer* m_Renderer;
    // Con trỏ tới renderer của SDL, dùng để vẽ đồ họa.

    SDL_Texture* m_TilesetTexture;
    // Texture chứa hình ảnh tileset (tập hợp các ô).

    static Map* s_Instance;
    // Biến tĩnh lưu instance duy nhất của lớp Map (Singleton).
};
