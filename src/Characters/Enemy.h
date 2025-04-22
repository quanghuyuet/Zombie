#pragma once

#include <SDL.h>
#include <vector>
#include <string>
#include "Character.h"
#include "Collider.h"
#include "Animation.h"
#include "Vector2D.h"
#include "Engine.h"
#include "Camera.h"
#include "Bullet.h"
#include "TextureManager.h"
#include "Timer.h"
#include "CollisionHandler.h"
#define DEAD_TIME 1.0f
using namespace std;
class Bullet;
class Enemy : public Character {
public:
    Enemy(Properties* props);

    virtual void Draw() override;
    virtual void Update(float dt) override;
    virtual void Clean() override;

    void TakeDamage(int damage);
    bool IsDead() const { return m_IsDead; }

    // Add getter methods
    float GetDeathTime() const { return m_DeathTime; }
    Collider* GetCollider() const { return m_Collider; }

private:
    void UpdateBullets(float dt);
    void Shoot();
    void AnimationState();

private:
    Collider* m_Collider; // Đối tượng quản lý va chạm
    Animation* m_Animation; // Đối tượng quản lý animation
    std::vector<Bullet*> m_Bullets; // Danh sách các viên đạn kẻ thù bắn ra
    float m_ShootCooldown; // Thời gian chờ giữa các lần bắn
    bool m_IsDead; // Trạng thái chết/sống
    float m_DeathTime; // Thời gian còn lại để hiển thị animation chết
    int m_Health; // Máu hiện tại
    int m_MaxHealth; // Máu tối đa
    float m_DetectionRange; // Tầm phát hiện người chơi
    float m_FlyingSpeed; // Tốc độ di chuyển
    float m_FlyingRange; // Phạm vi di chuyển qua lại
    Vector2D m_OriginPoint; // Điểm gốc ban đầu của kẻ thù
    bool m_MovingRight; // Hướng di chuyển (true = phải, false = trái)
    bool m_IsAttacking; // Đang tấn công hay không
};
