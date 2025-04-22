#pragma once
#include "Vector2D.h"

// Định nghĩa các hằng số cho vật lý và trò chơi
#define UNI_MASS 1.0f           // Khối lượng mặc định của đối tượng
#define GRAVITY 9.8f            // Gia tốc trọng lực mặc định
#define FORWARD 1               // Hướng tiến (sang phải)
#define BACKWARD -1             // Hướng lùi (sang trái)
#define UPWARD -1               // Hướng lên (trong hệ tọa độ SDL, Y âm là lên)
#define DOWNWARD 1              // Hướng xuống
#define ATTACK_TIME 0.5f        // Thời gian thực hiện tấn công
#define HIT_TIME 0.3f           // Thời gian hiệu ứng bị đánh
#define DEAD_TIME 1.0f          // Thời gian hiệu ứng chết
#define ATTACK_RANGE 50.0f      // Phạm vi tấn công
#define MAX_VELOCITY_X 150.0f   // Vận tốc tối đa theo trục X
#define SCREEN_WIDTH 1200       // Chiều rộng màn hình
#define SCREEN_HEIGHT 640       // Chiều cao màn hình

// Lớp RigidBody để quản lý vật lý của đối tượng
class RigidBody {
public:
    // Hàm khởi tạo RigidBody với các giá trị mặc định
    RigidBody() {
        m_Mass = UNI_MASS;              // Khối lượng mặc định (1.0f)
        m_Gravity = GRAVITY;            // Trọng lực mặc định (9.8f)
        m_FrictionCoefficient = 1.0f;   // Hệ số ma sát mặc định
    }

    // Thiết lập khối lượng của đối tượng
    inline void SetMass(float mass) { m_Mass = mass; }

    // Thiết lập gia tốc trọng lực
    inline void SetGravity(float gravity) { m_Gravity = gravity; }

    // Thiết lập hệ số ma sát
    inline void SetFrictionCoefficient(float friction) { m_FrictionCoefficient = friction; }

    // Áp dụng lực tổng quát (Vector2D) cho đối tượng
    inline void ApplyForce(Vector2D F) { m_Force = F; }

    // Áp dụng lực theo trục X
    inline void ApplyForceX(float Fx) { m_Force.X = Fx; }

    // Áp dụng lực theo trục Y
    inline void ApplyForceY(float Fy) { m_Force.Y = Fy; }

    // Xóa lực hiện tại
    inline void UnSetForce() { m_Force = Vector2D(0, 0); }

    // Áp dụng lực ma sát (Vector2D)
    inline void ApplyFriction(Vector2D Fr) { m_Friction = Fr; }

    // Xóa lực ma sát hiện tại
    inline void UnSetFriction() { m_Friction = Vector2D(0, 0); }

    // Thiết lập vận tốc theo trục X
    inline void SetVelocityX(float vx) { m_Velocity.X = vx; }

    // Thiết lập vận tốc theo trục Y
    inline void SetVelocityY(float vy) { m_Velocity.Y = vy; }

    // Lấy khối lượng của đối tượng
    inline float GetMass() { return m_Mass; }

    // Lấy vị trí hiện tại của đối tượng
    inline Vector2D Position() { return m_Position; }

    // Lấy gia tốc hiện tại của đối tượng
    inline Vector2D Acceleration() { return m_Acceleration; }

    // Lấy vận tốc hiện tại của đối tượng
    inline Vector2D Velocity() { return m_Velocity; }

    // Cập nhật trạng thái vật lý (vị trí, vận tốc, gia tốc) dựa trên thời gian
    void Update(float dt) {
        // Tính gia tốc theo trục X: (lực - ma sát * vận tốc) / khối lượng
        m_Acceleration.X = (m_Force.X - m_FrictionCoefficient * m_Velocity.X) / m_Mass;
        // Tính gia tốc theo trục Y: trọng lực + lực dọc / khối lượng
        m_Acceleration.Y = m_Gravity + (m_Force.Y / m_Mass);

        // Cập nhật vận tốc: vận tốc cũ + gia tốc * thời gian * 60
        m_Velocity = m_Velocity + m_Acceleration * dt * 60.0f;

        // Giới hạn vận tốc tối đa
        const float MAX_VEL_X = 200.0f; // Vận tốc tối đa trục X
        const float MAX_VEL_Y = 1000.0f; // Vận tốc tối đa trục Y
        if (m_Velocity.X > MAX_VEL_X) m_Velocity.X = MAX_VEL_X;
        if (m_Velocity.X < -MAX_VEL_X) m_Velocity.X = -MAX_VEL_X;
        if (m_Velocity.Y > MAX_VEL_Y) m_Velocity.Y = MAX_VEL_Y;
        if (m_Velocity.Y < -MAX_VEL_Y) m_Velocity.Y = -MAX_VEL_Y;

        // Cập nhật vị trí: vị trí cũ + vận tốc * thời gian
        m_Position = m_Position + m_Velocity * dt;
    }

private:
    // Khối lượng của đối tượng
    float m_Mass;
    // Gia tốc trọng lực
    float m_Gravity;
    // Hệ số ma sát
    float m_FrictionCoefficient;

    // Lực tổng tác dụng lên đối tượng
    Vector2D m_Force;
    // Lực ma sát
    Vector2D m_Friction;

    // Vị trí hiện tại
    Vector2D m_Position;
    // Vận tốc hiện tại
    Vector2D m_Velocity;
    // Gia tốc hiện tại
    Vector2D m_Acceleration;
};
