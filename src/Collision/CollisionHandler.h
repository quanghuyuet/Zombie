#pragma once
#include <SDL.h>
#include <vector>
#include "Map.h"

// Định nghĩa lớp CollisionHandler
class CollisionHandler {
public:
    // Lấy instance duy nhất của CollisionHandler (singleton)
    static CollisionHandler* GetInstance();

    // Kiểm tra va chạm giữa một hình chữ nhật và bản đồ
    bool MapCollision(SDL_Rect a);

    // Kiểm tra va chạm giữa hai hình chữ nhật
    bool CheckCollision(SDL_Rect a, SDL_Rect b);

    // Hàm hủy để giải phóng tài nguyên
    ~CollisionHandler();

private:
    // Hàm khởi tạo riêng tư (chỉ gọi bởi GetInstance)
    CollisionHandler();

    // Tilemap chứa thông tin va chạm của bản đồ
    std::vector<std::vector<int>> m_CollisionTilemap;

    // Con trỏ tĩnh đến instance duy nhất của lớp
    static CollisionHandler* s_Instance;
};
