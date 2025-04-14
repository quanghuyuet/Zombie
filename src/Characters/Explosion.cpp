#include "Explosion.h"
#include "Boss.h" // Để dùng dynamic_cast kiểm tra Boss
#include <cmath>

Explosion::Explosion() {
    x = 0;
    y = 0;
    frame = 0;
    active = false;
    timer = 0;
}

void Explosion::Start(float posX, float posY) {
    x = posX;
    y = posY;
    frame = 0;
    active = true;
    timer = 0;
}

void Explosion::Update(float deltaTime, std::vector<Character*>& characters) {
    if (!active) return;

    timer += deltaTime;
    if (timer >= EXPLOSION_DURATION) {
        active = false;
        return;
    }

    // Cập nhật frame cho animation
    frame = static_cast<int>(timer / (EXPLOSION_DURATION / 9)); // Giả sử 4 frame

    // Kiểm tra va chạm với Boss
    for (Character* character : characters) {
        // Chỉ xử lý nếu nhân vật là Boss
        Boss* boss = dynamic_cast<Boss*>(character);
        if (boss && !boss->IsDead()) {
            // Lấy vị trí và kích thước của collider
            SDL_Rect bossBox = boss->GetCollider()->Get();
            // Tính khoảng cách từ tâm vụ nổ đến tâm của Boss
            float bossCenterX = bossBox.x + bossBox.w / 2.0f;
            float bossCenterY = bossBox.y + bossBox.h / 2.0f;
            float dx = bossCenterX - x;
            float dy = bossCenterY - y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < 30.0f) { // Bán kính vụ nổ
                boss->TakeDamage(EXPLOSION_DAMAGE);
            }
        }
    }
}

void Explosion::Render(SDL_Renderer* renderer, SDL_Texture* texture) {
    if (!active) return;

    SDL_Rect srcRect = { frame * 200, 0, 200, 200 }; // Giả sử mỗi frame 64x64
    SDL_Rect dstRect = { static_cast<int>(x - 100), static_cast<int>(y - 100), 200, 200};

    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
}
