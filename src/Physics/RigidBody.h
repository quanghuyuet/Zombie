#pragma once
#include "Vector2D.h"
#define UNI_MASS 1.0f
#define GRAVITY 9.8f
#define FORWARD 1
#define BACKWARD -1
#define UPWARD -1
#define DOWNWARD 1
#define ATTACK_TIME 0.5f
#define HIT_TIME 0.3f
#define DEAD_TIME 1.0f
#define ATTACK_RANGE 50.0f
#define MAX_VELOCITY_X 150.0f
#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT  640
class RigidBody {
public:
    RigidBody() {
        m_Mass = UNI_MASS;
        m_Gravity = GRAVITY;
        m_FrictionCoefficient = 1.0f;
    }

    inline void SetMass(float mass) { m_Mass = mass; }
    inline void SetGravity(float gravity) { m_Gravity = gravity; }
    inline void SetFrictionCoefficient(float friction) { m_FrictionCoefficient = friction; }

    // Force
    inline void ApplyForce(Vector2D F) { m_Force = F; }
    inline void ApplyForceX(float Fx) { m_Force.X = Fx; }
    inline void ApplyForceY(float Fy) { m_Force.Y = Fy; }
    inline void UnSetForce() { m_Force = Vector2D(0, 0); }

    // Friction
    inline void ApplyFriction(Vector2D Fr) { m_Friction = Fr; }
    inline void UnSetFriction() { m_Friction = Vector2D(0, 0); }

    inline void SetVelocityX(float vx) { m_Velocity.X = vx; }
    inline void SetVelocityY(float vy) { m_Velocity.Y = vy; }

    // Getters
    inline float GetMass() { return m_Mass; }
    inline Vector2D Position() { return m_Position; }
    inline Vector2D Acceleration() { return m_Acceleration; }
    inline Vector2D Velocity() { return m_Velocity; }

    // Update
    void Update(float dt) {
        m_Acceleration.X = (m_Force.X - m_FrictionCoefficient * m_Velocity.X) / m_Mass;
        m_Acceleration.Y = m_Gravity + (m_Force.Y / m_Mass);

        m_Velocity = m_Velocity + m_Acceleration * dt * 60.0f;

        const float MAX_VEL_X = 200.0f;
        const float MAX_VEL_Y = 1000.0f;
        if (m_Velocity.X > MAX_VEL_X) m_Velocity.X = MAX_VEL_X;
        if (m_Velocity.X < -MAX_VEL_X) m_Velocity.X = -MAX_VEL_X;
        if (m_Velocity.Y > MAX_VEL_Y) m_Velocity.Y = MAX_VEL_Y;
        if (m_Velocity.Y < -MAX_VEL_Y) m_Velocity.Y = -MAX_VEL_Y;

        m_Position = m_Position + m_Velocity * dt;

    }

private:
    float m_Mass;
    float m_Gravity;
    float m_FrictionCoefficient;

    Vector2D m_Force;
    Vector2D m_Friction;

    Vector2D m_Position;
    Vector2D m_Velocity;
    Vector2D m_Acceleration;

};
