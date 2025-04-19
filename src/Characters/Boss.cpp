#include "Boss.h"
#include "Engine.h"
#include "CollisionHandler.h"
#include "Timer.h"
#include "Camera.h"

Boss::Boss(Properties* props) : Character(props) {
    m_RigidBody = new RigidBody();
    m_RigidBody->SetGravity(100.0f);
    m_Collider = new Collider();
    m_Animation = new Animation();

    m_WalkingSpeed = 60.0f;
    m_DetectionRange = 200.0f;
    m_AttackRange = 100.0f;
    m_AttackCooldown = 2.0f;
    m_IsDead = false;
    m_DeathTime = 0.0f;
    m_MovingRight = true;
    m_IsAttacking = false;
    m_IsGrounded = false;
    m_OriginPoint = Vector2D(props->X, props->Y);

    m_Transform->X = props->X;
    m_Transform->Y = props->Y;
    m_LastSafePosition.X = m_Transform->X;
    m_LastSafePosition.Y = m_Transform->Y;

    m_Health = 20;
    m_MaxHealth = 20;

    m_Collider->Set(m_Transform->X, m_Transform->Y, m_Width, m_Height);
}

void Boss::TakeDamage(int damage) {
    if (!m_IsDead) {
        m_Health -= damage;
        if (m_Health <= 0) {
            m_IsDead = true;
            m_DeathTime = 0.5f;
            m_RigidBody->SetVelocityX(0);
            m_RigidBody->SetVelocityY(0);
            AnimationState();
            // Phát âm thanh khi boss chết
            Mix_Chunk* deathSound = Engine::GetInstance()->GetBossDeathSound();
            if (deathSound) {
                Mix_PlayChannel(-1, deathSound, 0);
            }
        }
    }
}

void Boss::Update(float dt) {
    if (m_IsDead) {
        m_DeathTime -= dt;
        if (m_DeathTime > 0) {
            AnimationState();
            m_Animation->Update();
        }
        return;
    }

    Knight* player = Engine::GetInstance()->GetPlayer();
    m_IsAttacking = false;

    m_RigidBody->Update(dt);
    Vector2D velocity = m_RigidBody->Velocity();

    float newY = m_Transform->Y + velocity.Y * dt;
    m_Collider->Set(m_Transform->X, newY, m_Width, m_Height * 0.5f);

    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
        if (velocity.Y > 0) {
            m_Transform->Y = m_LastSafePosition.Y;
            m_RigidBody->SetVelocityY(0);
            m_IsGrounded = true;
            m_Collider->Set(m_Transform->X, m_Transform->Y, m_Width, m_Height * 0.5f);
        }
    } else {
        m_Transform->Y = newY;
        m_LastSafePosition.Y = m_Transform->Y;
        m_IsGrounded = false;
    }

    if (m_IsGrounded) {
        float speed = m_WalkingSpeed * dt;
        float newX = m_Transform->X + (m_MovingRight ? speed : -speed);

        float leftBoundary = m_OriginPoint.X - 200.0f;
        float rightBoundary = m_OriginPoint.X + 200.0f;

        if (newX < leftBoundary) {
            newX = leftBoundary;
            m_MovingRight = true;
        } else if (newX > rightBoundary) {
            newX = rightBoundary;
            m_MovingRight = false;
        }

        m_Collider->Set(newX, m_Transform->Y, m_Width, m_Height * 0.5f);
        if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
            m_Transform->X = m_LastSafePosition.X;
            m_MovingRight = !m_MovingRight;
        } else {
            m_Transform->X = newX;
            m_LastSafePosition.X = m_Transform->X;
        }
        m_Flip = m_MovingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        if (player) {
            float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                                 pow(player->GetOrigin()->Y - m_Origin->Y, 2));
            if (distance < m_AttackRange) {
                m_IsAttacking = true;
                m_Flip = (player->GetOrigin()->X < m_Origin->X) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                if (m_AttackCooldown <= 0) {
                    Attack(player);
                    m_AttackCooldown = 2.0f;
                }
            }
        }
    }

    if (m_AttackCooldown > 0) m_AttackCooldown -= dt;

    m_Origin->X = m_Transform->X + m_Width / 2;
    m_Origin->Y = m_Transform->Y + m_Height / 2;

    AnimationState();
    m_Animation->Update();
}

void Boss::Draw() {
    if (!m_IsDead || m_DeathTime > 0) {
        m_Animation->Draw(m_Transform->X, m_Transform->Y - 60, m_Width, m_Height, m_Flip);
        SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
        Vector2D cameraPos = Camera::GetInstance()->GetPosition();
        float cameraX = cameraPos.X;
        float cameraY = cameraPos.Y;
        int screenX = static_cast<int>(m_Transform->X - cameraX);
        int screenY = static_cast<int>(m_Transform->Y - cameraY - 60);
        int barWidth = 180;
        int barHeight = 17;
        int padding = -110;

        int barX = screenX + (m_Width - barWidth) / 2;
        int barY = screenY - barHeight - padding;

        float hpRatio = (float)m_Health / m_MaxHealth;
        if (hpRatio < 0) hpRatio = 0;
        if (hpRatio > 1) hpRatio = 1;
        SDL_Rect background = {barX, barY, barWidth, barHeight};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &background);

        SDL_Rect healthBar = {barX, barY, static_cast<int>(barWidth * hpRatio), barHeight};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthBar);
    }
}

void Boss::Attack(Knight* player) {
    if (player) {
        float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                             pow(player->GetOrigin()->Y - m_Origin->Y, 2));
        if (distance < m_AttackRange) {
            player->TakeDamage(2);
        }
    }
}

void Boss::AnimationState() {
    if (m_IsDead) {
        m_Animation->SetProps("boss_death", 0, 5, 120);
    } else if (m_IsAttacking) {
        m_Animation->SetProps("boss_attack", 0, 8, 100);
    } else {
        m_Animation->SetProps("boss_run", 0, 8, 100);
    }
}
