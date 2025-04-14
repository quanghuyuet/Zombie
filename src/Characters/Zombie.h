#pragma once
#include "Character.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Animation.h"
#include "Vector2D.h"
#include "Knight.h"
#include "CollisionHandler.h"
using namespace std;

class Zombie : public Character {
public:
    Zombie(Properties* props);
    virtual void Draw();
    virtual void Update(float dt);
    virtual void Clean() {}
    virtual void TakeDamage(int damage);
    Collider* GetCollider() { return m_Collider; }
    virtual float GetDeathTime() { return m_DeathTime; }
    virtual bool IsDead() { return m_IsDead; }

private:
    void AnimationState();
    void Attack(Knight* player);
    RigidBody* m_RigidBody;
    Collider* m_Collider;
    Animation* m_Animation;
    float m_WalkingSpeed;
    float m_WalkingRange;
    float m_DetectionRange;
    float m_AttackCooldown;
    bool m_IsDead;
    float m_DeathTime;
    int m_Health;
    int m_MaxHealth;
    bool m_MovingRight;
    bool m_IsAttacking;
    bool m_IsGrounded;
    Vector2D m_OriginPoint;
    Vector2D m_LastSafePosition;
    float m_AttackRange;
};
