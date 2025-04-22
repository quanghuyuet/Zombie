#pragma once
#include <iostream>
#include "Character.h"
#include "GameObject.h"
#include "Animation.h"
#include "RigidBody.h"
#include "Vector2D.h"
#include "Collider.h"
#include "Point.h"
#include "Transform.h"
#include <SDL_mixer.h>

// Hằng số cho cơ chế của hiệp sĩ
#define JUMP_TIME 15.0f    // Thời gian tối đa để nhảy
#define JUMP_FORCE 10.0f   // Lực áp dụng khi nhảy
#define RUN_FORCE 4.0f     // Lực áp dụng khi chạy
#define ATTRACK_TIME 10.0f // Thời gian của hoạt hình tấn công

using namespace std;

class Knight : public Character {
public:
    // Hàm khởi tạo hiệp sĩ với các thuộc tính
    Knight(Properties* props);

    // Vẽ hiệp sĩ và giao diện (thanh máu, mana) lên màn hình
    virtual void Draw();

    // Cập nhật trạng thái hiệp sĩ dựa trên đầu vào người chơi và vật lý
    virtual void Update(float dt);

    // Giải phóng tài nguyên khi hiệp sĩ bị hủy
    virtual void Clean();

    // Lấy thành phần vị trí và kích thước của hiệp sĩ
    inline Transform* GetTransform() { return m_Transform; }

    // Lấy thành phần va chạm của hiệp sĩ
    inline Collider* GetCollider() { return m_Collider; }

    // Lấy vị trí an toàn trước khi va chạm
    inline Vector2D GetLastSafePosition() { return m_LastSafePosition; }

    // Lấy điểm trung tâm của hiệp sĩ
    Point* GetOrigin() { return m_Origin; }

    // Giảm máu và mana khi hiệp sĩ nhận sát thương
    void TakeDamage(int damage);

    // Lấy giá trị máu hiện tại
    int GetHealth() const { return m_Health; }

    // Lấy giá trị mana hiện tại
    int GetMana() const { return m_Mana; }

private:
    // Cập nhật hoạt hình dựa trên trạng thái của hiệp sĩ
    void AnimationState();

    // Các biến trạng thái
    bool m_IsRunning;      // Hiệp sĩ có đang chạy không?
    bool m_IsJumping;      // Hiệp sĩ có đang nhảy không?
    bool m_IsFalling;      // Hiệp sĩ có đang rơi không?
    bool m_IsGrounded;     // Hiệp sĩ có đang trên mặt đất không?
    bool m_IsAtracking;    // Hiệp sĩ có đang tấn công không?
    bool m_IsCrouching;    // Hiệp sĩ có đang ngồi không?

    // Cơ chế nhảy
    float m_JumpTime;      // Thời gian còn lại để nhảy
    float m_JumpForce;     // Lực áp dụng để nhảy

    // Cơ chế tấn công
    float m_AtrackTime;    // Thời gian còn lại của hoạt hình tấn công

    // Máu và mana
    int m_Health;          // Điểm máu hiện tại
    int m_MaxHealth;       // Điểm máu tối đa
    int m_Mana;            // Điểm mana hiện tại
    int m_MaxMana;         // Điểm mana tối đa
    float m_ManaRegenTime; // Thời gian chờ để hồi mana
    float m_ManaRegenRate; // Tốc độ hồi mana (giây)

    // Các thành phần
    Collider* m_Collider;          // Thành phần va chạm để phát hiện va chạm
    Vector2D m_LastSafePosition;   // Vị trí an toàn trước khi va chạm
    RigidBody* m_RigidBody;        // Thành phần vật lý để quản lý di chuyển
    Animation* m_Animation;        // Thành phần hoạt hình để hiển thị hình ảnh
    Point* m_Origin;               // Điểm trung tâm của hiệp sĩ

    // Hiệu ứng âm thanh
    Mix_Chunk* m_RunSound;         // Âm thanh khi hiệp sĩ chạy
    Mix_Chunk* m_AttackSound;      // Âm thanh khi hiệp sĩ tấn công
    Mix_Chunk* m_JumpSound;        // Âm thanh khi hiệp sĩ nhảy

    bool m_WasRunning;             // Theo dõi trạng thái chạy trước đó để phát âm thanh
};
