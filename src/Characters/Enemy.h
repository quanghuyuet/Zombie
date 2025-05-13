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
// Khai báo hàm khởi tạo của lớp Enemy
Enemy(Properties* props); // Hàm khởi tạo, nhận con trỏ Properties để thiết lập thuộc tính kẻ thù (vị trí, texture, v.v.)

// Khai báo các hàm ảo được ghi đè từ lớp cha (Character)
virtual void Draw() override; // Hàm vẽ kẻ thù, thanh máu và đạn lên màn hình, ghi đè từ lớp cha
virtual void Update(float dt) override; // Hàm cập nhật trạng thái kẻ thù (di chuyển, tấn công, hoạt ảnh) mỗi frame, nhận delta time, ghi đè từ lớp cha
virtual void Clean() override; // Hàm dọn dẹp tài nguyên (texture, đạn, collider, animation) khi kẻ thù bị xóa, ghi đè từ lớp cha

// Khai báo hàm xử lý sát thương
void TakeDamage(int damage); // Hàm xử lý khi kẻ thù nhận sát thương, giảm máu và kiểm tra trạng thái chết

// Phương thức getter kiểm tra trạng thái chết
bool IsDead() const { return m_IsDead; } // Trả về true nếu kẻ thù đã chết, const đảm bảo không thay đổi đối tượng

// Các phương thức getter khác
float GetDeathTime() const { return m_DeathTime; } // Trả về thời gian còn lại của hoạt ảnh chết, const đảm bảo không thay đổi đối tượng
Collider* GetCollider() const { return m_Collider; } // Trả về con trỏ đến vùng va chạm của kẻ thù, const đảm bảo không thay đổi đối tượng

private:
// Các hàm private hỗ trợ nội bộ
void UpdateBullets(float dt); // Cập nhật trạng thái các viên đạn (vị trí, va chạm), nhận delta time
void Shoot(); // Xử lý việc bắn đạn, tạo đạn mới hướng về người chơi
void AnimationState(); // Chọn hoạt ảnh phù hợp (bay, tấn công, chết) dựa trên trạng thái kẻ thù

private:
    Collider* m_Collider;
    Animation* m_Animation;
    std::vector<Bullet*> m_Bullets;
    float m_ShootCooldown; // Thời gian chờ giữa các lần bắn
    bool m_IsDead;
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
