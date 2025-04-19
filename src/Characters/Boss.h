#pragma once
#include "Character.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Animation.h"
#include "Vector2D.h"
#include "Knight.h"
#include "CollisionHandler.h"
using namespace std;

class Boss : public Character {
public:
    Boss(Properties* props); // Hàm khởi tạo
    virtual void Draw(); // Vẽ boss
    virtual void Update(float dt); // Cập nhật trạng thái
    virtual void Clean() {} // Dọn dẹp (hiện tại không dùng)
    virtual void TakeDamage(int damage); // Nhận sát thương
    Collider* GetCollider() { return m_Collider; } // Lấy collider
    virtual float GetDeathTime() { return m_DeathTime; } // Lấy thời gian chết
    virtual bool IsDead() { return m_IsDead; } // Kiểm tra boss đã chết

private:
    void AnimationState(); // Cập nhật trạng thái animation
    void Attack(Knight* player); // Tấn công người chơi
    RigidBody* m_RigidBody; // Vật lý
    Collider* m_Collider; // Collider cho va chạm
    Animation* m_Animation; // Animation
    float m_WalkingSpeed; // Tốc độ di chuyển
    float m_DetectionRange; // Phạm vi phát hiện người chơi
    float m_AttackCooldown; // Thời gian hồi chiêu tấn công
    bool m_IsDead; // Trạng thái chết
    int m_Health; // Máu hiện tại
    int m_MaxHealth; // Máu tối đa
    float m_DeathTime; // Thời gian chờ trước khi xóa boss
    bool m_MovingRight; // Hướng di chuyển
    bool m_IsAttacking; // Đang tấn công
    bool m_IsGrounded; // Đang trên mặt đất
    Vector2D m_OriginPoint; // Điểm gốc (tâm) của boss
    Vector2D m_LastSafePosition; // Vị trí an toàn trước đó
    float m_AttackRange; // Phạm vi tấn công
};
