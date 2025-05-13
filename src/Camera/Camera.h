#pragma once
#include <SDL.h>
#include <iostream>
#include "Vector2D.h"
#include "Point.h"
#include "Engine.h"
#include "RigidBody.h"
#define TILE_SIZE 32
#define MAP_ROWS 20  // Số hàng của bản đồ
#define MAP_COLS 100 // Số cột của bản đồ
#define MAP_WIDTH  (MAP_COLS * TILE_SIZE)  // Chiều rộng bản đồ
#define MAP_HEIGHT (MAP_ROWS * TILE_SIZE)  // Chiều cao bản đồ

class Camera
{
public:
    void Update(float dt);
    inline SDL_Rect GetViewBox() { return m_ViewBox; }
    // Lấy vị trí hiện tại của camera
    inline Vector2D GetPosition() { return m_Position; }
    inline Point* GetTarget() { return m_Target; }
    // Thiết lập mục tiêu cho camera theo dõi
    inline void SetTarget(Point* target) { m_Target = target; }
    // Lấy instance của Camera (singleton pattern)
    inline static Camera* GetInstance() { return s_Instance = (s_Instance == nullptr) ? new Camera() : s_Instance; }

private:
    Camera() {
        m_ViewBox = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    }

    Point* m_Target;
    Vector2D m_Position;
    SDL_Rect m_ViewBox;
    static Camera* s_Instance;
};
