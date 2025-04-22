#include "Bullet.h"
#include "RigidBody.h"
// Hàm vẽ hình tròn với màu đồng nhất
void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        // Vẽ 8 điểm đối xứng trên vòng tròn
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

// Hàm vẽ hình tròn với hiệu ứng gradient (chuyển màu từ tâm ra rìa)
void DrawGradientCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius,
                        Uint8 centerR, Uint8 centerG, Uint8 centerB, Uint8 centerA,
                        Uint8 edgeR, Uint8 edgeG, Uint8 edgeB, Uint8 edgeA) {
    const int steps = radius; // Số vòng tròn (mỗi vòng cách 1 pixel)
    for (int r = 1; r <= radius; r++) {
        float ratio = static_cast<float>(r) / radius; // Tỷ lệ từ tâm ra rìa
        Uint8 colorR = static_cast<Uint8>(centerR + (edgeR - centerR) * ratio);
        Uint8 colorG = static_cast<Uint8>(centerG + (edgeG - centerG) * ratio);
        Uint8 colorB = static_cast<Uint8>(centerB + (edgeB - centerB) * ratio);
        Uint8 colorA = static_cast<Uint8>(centerA + (edgeA - centerA) * ratio);
        DrawCircle(renderer, centerX, centerY, r, colorR, colorG, colorB, colorA);
    }
}

// Hàm khởi tạo viên đạn
Bullet::Bullet(float x, float y, Vector2D vel, const string& texID) {
    position = Vector2D(x, y); // Đặt vị trí ban đầu
    velocity = vel; // Đặt vận tốc
    collider = new Collider(); // Tạo vùng va chạm
    collider->Set(static_cast<int>(x), static_cast<int>(y), 26, 26); // Thiết lập vùng va chạm (26x26, phù hợp với bán kính 13)
    active = true; // Đạn đang hoạt động
    textureID = texID; // ID texture (không dùng trong mã này)
}

// Hàm cập nhật trạng thái viên đạn
void Bullet::Update(float dt) {
    if (!active) return; // Nếu đạn không hoạt động, bỏ qua

    position = position + velocity * dt; // Cập nhật vị trí dựa trên vận tốc

    const float colliderWidth = 26.0f; // Chiều rộng vùng va chạm
    const float colliderHeight = 26.0f; // Chiều cao vùng va chạm
    collider->Set(static_cast<int>(position.X), static_cast<int>(position.Y), colliderWidth, colliderHeight); // Cập nhật vùng va chạm
}

// Hàm vẽ viên đạn với hiệu ứng gradient và ánh sáng
void Bullet::Draw() {
    if (!active) return;
    SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
    Vector2D cam = Camera::GetInstance()->GetPosition();
    float renderX = position.X - cam.X;
    float renderY = position.Y - cam.Y;
    // Chỉ vẽ nếu đạn nằm trong màn hình
    if (renderX >= -30 && renderX <= SCREEN_WIDTH && renderY >= -30 && renderY <= SCREEN_HEIGHT) {
        // Vẽ vòng sáng (glow)
        DrawGradientCircle(renderer, renderX + 10, renderY + 10, 15,
                           255, 100, 0, 80,   // Màu cam nhạt, trong suốt ở tâm
                           255, 50, 0, 0);    // Trong suốt hoàn toàn ở rìa
        // Vẽ viên đạn chính
        DrawGradientCircle(renderer, renderX + 10, renderY + 10, 10,
                           255, 255, 100, 255, // Màu vàng sáng ở tâm
                           255, 100, 0, 255);  // Màu cam đậm ở rìa
    }
}
