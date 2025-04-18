#include "Bullet.h"

// Hàm vẽ hình tròn
void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            int dx = w;
            int dy = h;
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

Bullet::Bullet(float x, float y, Vector2D vel, const string& texID) {
    position = Vector2D(x, y);
    velocity = vel;
    collider = new Collider();
    collider->Set(static_cast<int>(x), static_cast<int>(y), 32, 32);
    active = true;
    textureID = texID;
}

void Bullet::Update(float dt) {
    if (!active) return;

    position = position + velocity * dt;

    const float colliderWidth = 32.0f;
    const float colliderHeight = 32.0f;
    collider->Set(static_cast<int>(position.X), static_cast<int>(position.Y), colliderWidth, colliderHeight);
}

void Bullet::Draw() {
    if (!active) return;

    SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
    Vector2D cam = Camera::GetInstance()->GetPosition();

    float worldX = position.X;
    float worldY = position.Y;
    int renderX = static_cast<int>(worldX - cam.X);
    int renderY = static_cast<int>(worldY - cam.Y);

    const int circleRadius = 16;
    const int colliderWidth = 32;
    const int colliderHeight = 32;

    // Vẽ hình tròn màu cam đậm hơn
    DrawCircle(renderer, renderX + circleRadius, renderY + circleRadius, circleRadius, 255, 100, 0, 255);
/*
    // Vẽ hộp va chạm (debug)
    SDL_Rect hitbox = { renderX, renderY, colliderWidth, colliderHeight };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(renderer, &hitbox);
    */
}
