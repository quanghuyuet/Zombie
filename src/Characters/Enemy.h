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

// Định nghĩa hằng số nếu cần
#define DEAD_TIME 1.0f // Thời gian hiển thị animation chết
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
    Collider* m_Collider;
    Animation* m_Animation;
    std::vector<Bullet*> m_Bullets;
    float m_ShootCooldown;
    bool m_IsDead;
    float m_DeathTime;
    int m_Health;
    int m_MaxHealth;
    float m_DetectionRange; // Tầm phát hiện knight
    float m_FlyingSpeed; // Tốc độ di chuyển của enemy
    float m_FlyingRange; // Phạm vi di chuyển cố định
    Vector2D m_OriginPoint; // Điểm gốc ban đầu của enemy
    bool m_MovingRight; // Hướng di chuyển (phải/trái)
    bool m_IsAttacking; // Đang tấn công hay không
};
