
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
#define JUMP_TIME 15.0f
#define JUMP_FORCE 10.0f

#define RUN_FORCE 4.0f
#define ATTRACK_TIME 20.0f
using namespace std;

class Knight : public Character
{
    public:
        Knight(Properties* props); //constructor
        virtual void Draw();
        virtual void Update(float dt );
        virtual void Clean();
        inline Transform* GetTransform() { return m_Transform; }
        inline Collider* GetCollider() { return m_Collider; }
        inline Vector2D GetLastSafePosition() { return m_LastSafePosition; }
        Point* GetOrigin() { return m_Origin; }
        void TakeDamage(int damage);
        int GetHealth() const { return m_Health; } // Thêm phương thức GetHealth()
       // float GetY() const { return m_Transform->Y; } // Thêm phương thức GetY()
    private:
        void AnimationState();
    private:
        bool m_IsRunning;
        bool m_IsJumping;
        bool m_IsFalling;
        bool m_IsGrounded;
        bool m_IsAtracking;
        bool m_IsCrouching;
        float m_JumpTime;
        float m_JumpForce;
        float m_AtrackTime;
        int m_Health; // HP hiện tại
        int m_MaxHealth; // HP tối đa
        int m_Mana;   // MP hiện tại
        int m_MaxMana; // MP tối đa
        float m_ManaRegenTime; // Thời gian chờ để hồi MP
        float m_ManaRegenRate; // Tốc độ hồi MP (giây)
        Collider* m_Collider;
        Vector2D  m_LastSafePosition;
        RigidBody* m_RigidBody;
        Animation* m_Animation;
        Point* m_Origin;
       // Transform* m_Transform;

};
