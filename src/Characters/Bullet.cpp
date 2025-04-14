#include "Bullet.h"

Bullet::Bullet(float x, float y, Vector2D vel, const string& texID) {
    position = Vector2D(x, y);
    velocity = vel;
    collider = new Collider();
    // Giả sử texture 53x53, căn giữa collider tại tâm của đạn
    collider->Set(position.X - 2.5f, position.Y - 1.5f, 5, 3); // Kích thước nhỏ để khớp với hitbox
    active = true;
    textureID = texID;
}

void Bullet::Update(float dt) {
    if (!active) return; // Không cập nhật nếu đạn không hoạt động

    position = position + velocity * dt;
    // Cập nhật collider, căn giữa hitbox tại tâm của đạn
    collider->Set(position.X - 2.5f, position.Y - 1.5f, 5, 3);
}

void Bullet::Draw() {
    if (!active) return;

    Vector2D camPos = Camera::GetInstance()->GetPosition();
    // Chuyển đổi sang tọa độ màn hình và sử dụng static_cast để đảm bảo chính xác pixel
    int renderX = static_cast<int>(position.X - camPos.X);
    int renderY = static_cast<int>(position.Y - camPos.Y);

    // Giả sử texture 53x53, căn giữa texture tại position
    renderX -= 26; // Offset để căn giữa (53 / 2 ≈ 26)
    renderY -= 26;

    TextureManager::GetInstance()->Draw(textureID, renderX, renderY, 53, 53, SDL_FLIP_NONE);
}
